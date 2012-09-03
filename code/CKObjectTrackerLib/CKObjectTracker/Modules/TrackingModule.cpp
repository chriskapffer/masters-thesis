//
//  TrackingModule.cpp
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 22.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#include "TrackingModule.h"
#include "SanityCheck.h"
#include "Profiler.h"
#include "Utils.h"

#define TIMER_CONVERT "converting"
#define TIMER_OPTIMIZE "optimizing"
#define TIMER_TRACK "tracking"
#define TIMER_FILTER "filtering"
#define TIMER_ESTIMATE "estimating"
#define TIMER_VALIDATE "validating"

#define POINTS_TOTAL "initializing"
#define POINTS_TRACKED "tracking"
#define POINTS_FILTERED "filtering"

using namespace std;
using namespace cv;

namespace ck {

TrackingModule::TrackingModule(int maxPoints, int minPointsAbs, float minPointsRel, bool useSubPixels, bool calcHomRelToFrame, bool filterDistortions, float distortionThreshold)
    : AbstractModule(MODULE_TYPE_TRACKING)
{
    // tracker params
    _enabled = true;
    
    _maxSuccessiveFrames = -1;
    _maxPointsAbsolute = maxPoints;
    _minPointsAbsolute = minPointsAbs;
    _minPointsRelative = minPointsRel;
    
    _useSubPixels = useSubPixels;
    _calcHomRelToFrame = calcHomRelToFrame;
    _filterDistortions = filterDistortions;
    _distortionThreshold = distortionThreshold;
    
    // LK: opencv lucas kanade sample project
    // SL: success labs (adapted version of LK) see: TODO: website
    // RM: rui marques on http://answers.opencv.org/question/176/which-values-for-window-size-and-number-of/

    // optical flow and sub pix params
    _terminationCriteria = TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 30, 0.01); // LK/SL: 20, 0.03
    _winSizeSubPix = Size(10, 10); // LK/SL: 10
    _winSizeFlow = Size(21, 21); // RM: 41 LK: 31 SL: 3-5
    _zeroZone = Size(-1, -1);
    _minEigenThreshold = 0.0001f; // LK: 0.001f
    _maxLevel = 3; // RM: 2
    _lkFlags = 0;
    
    // internal data
    _initialPointCount = 0;
    _isInitialCall = true;
    _succFrameCount = 0;
}

TrackingModule::~TrackingModule()
{
    
}

void TrackingModule::initWithObjectImage(const cv::Mat &objectImage)
{
    _objectCorners = vector<Point2f>(4); // clock wise
    _objectCorners[0] = cvPoint(               0,                0); // top left
    _objectCorners[1] = cvPoint(objectImage.cols,                0); // top right
    _objectCorners[2] = cvPoint(objectImage.cols, objectImage.rows); // bottom right
    _objectCorners[3] = cvPoint(               0, objectImage.rows); // bottom left
}

bool TrackingModule::internalProcess(ModuleParams& params, TrackerDebugInfo& debugInfo)
{
    Mat previousImage;
    Mat currentImage;
    Mat homography = params.homography;
    vector<Point2f> pointsIn = params.points;
    vector<Point2f> pointsOut;
    
    Profiler* profiler = Profiler::Instance();
    
    // clear module specific debug information
    debugInfo.namedPoints.clear();
    
    // don't go any further, if tracker is set not enabled
    if (!_enabled || pointsIn.empty()) {
        return false;
    }
    
    // strip end of list, if there are too many points
    if (pointsIn.size() > _maxPointsAbsolute) {
        pointsIn.erase(pointsIn.begin() + _maxPointsAbsolute, pointsIn.end());
    }
    
    // store number of points, if they are the initial point set to start with
    // (previous module was not tracker itself)
    if (_isInitialCall) {
        _isInitialCall = false;
        _initialPointSet = pointsIn;
        _initialPointCount = (int)pointsIn.size();
        params.homography.copyTo(_initialHomography);
        debugInfo.initialPointCount = _initialPointCount;
        _succFrameCount = 0;
    }
    
    // stop tracking after max successive frames (used for homography refinemend via validation module)
    _succFrameCount++;
    if (_maxSuccessiveFrames > 0 && _succFrameCount > _maxSuccessiveFrames) {
        _isInitialCall = true;
        return false;
    }
    
    // store original points array
    debugInfo.namedPoints.push_back(make_pair(POINTS_TOTAL, pointsIn));
    
    // convert to gray
    profiler->startTimer(TIMER_CONVERT);
    utils::bgr_a_2Gray(params.sceneImagePrevious, previousImage, COLOR_CONV_CV);
    utils::bgr_a_2Gray(params.sceneImageCurrent, currentImage, COLOR_CONV_CV);
    profiler->stopTimer(TIMER_CONVERT);
    
    // calculate sub pixel locations
    if (_useSubPixels) {
        profiler->startTimer(TIMER_OPTIMIZE);
        cornerSubPix(previousImage, pointsIn, _winSizeSubPix, _zeroZone, _terminationCriteria);
        profiler->stopTimer(TIMER_OPTIMIZE);
    }
    
    // calculate optical flow
    float avgError = 0;
    float avgDistSq = 0;
    float maxDistSq = 0;
    float minDistSq = MAX(currentImage.cols, currentImage.rows);
    vector<uchar> status;
    vector<float> error;
    
    profiler->startTimer(TIMER_TRACK);
    calcOpticalFlowPyrLK(previousImage, currentImage, pointsIn, pointsOut, status, error, _winSizeFlow, _maxLevel, _terminationCriteria, _lkFlags, _minEigenThreshold);
    for (int i = (int)pointsOut.size() - 1; i >= 0; i--) {
        if (!status[i]) {
            // if point could not be tracked remove it (from both lists, to keep them in sync)
            _initialPointSet.erase(_initialPointSet.begin() + i);
            pointsOut.erase(pointsOut.begin() + i);
            pointsIn.erase(pointsIn.begin() + i);
            continue;
        }
        Point2f vec = pointsOut[i] - pointsIn[i];
        float distSq = vec.x * vec.x + vec.y * vec.y;
        maxDistSq = MAX(maxDistSq, distSq);
        minDistSq = MIN(minDistSq, distSq);
        avgDistSq += distSq;
        avgError += error[i];
    }
    avgDistSq /= pointsOut.size();
    avgError /= pointsOut.size();
    profiler->stopTimer(TIMER_TRACK);
    
    // store remaining points after tracking
    debugInfo.namedPoints.push_back(make_pair(POINTS_TRACKED, pointsOut));
    debugInfo.distanceRange = sqrt(maxDistSq - minDistSq);
    debugInfo.avgError = avgError;
    
    // filter new points
    if (_filterDistortions) {
        profiler->startTimer(TIMER_FILTER);
        for (int i = (int)pointsOut.size(); i >= 0; i--) {
            Point2f vec = pointsOut[i] - pointsIn[i];
            float distanceSquared = vec.x * vec.x + vec.y * vec.y;
            if (fabs(distanceSquared - avgDistSq) / avgDistSq > _distortionThreshold) {
                _initialPointSet.erase(_initialPointSet.begin() + i);
                pointsOut.erase(pointsOut.begin() + i);
                pointsIn.erase(pointsIn.begin() + i);
            }
        }
        profiler->stopTimer(TIMER_FILTER);
    }
    debugInfo.namedPoints.push_back(make_pair(POINTS_FILTERED, pointsOut));

    // check if there are enough points left
    int currentCount = (int)pointsOut.size();
    if (currentCount < _minPointsAbsolute || currentCount < _initialPointCount * _minPointsRelative) {
        cout << "Lost too many points." << endl;
        _isInitialCall = true;
        return false;
    }
    
    // compute homography from points
    profiler->startTimer(TIMER_ESTIMATE);
    if (_calcHomRelToFrame) {
        homography *= findHomography(pointsIn, pointsOut);
    } else {
        if (_initialPointSet.size() == pointsOut.size()) {
            homography = findHomography(_initialPointSet, pointsOut) * _initialHomography;
        } else {
            cout << "This should not happen!!" << endl; //TODO throw
        }
    }
    profiler->stopTimer(TIMER_ESTIMATE);

    // validate homography matrix
    profiler->startTimer(TIMER_VALIDATE);
    vector<Point2f> objectCornersTransformed;
    perspectiveTransform(_objectCorners, objectCornersTransformed, homography);
    bool validHomography = true
        //&& SanityCheck::checkBoundaries(objectCornersTransformed, sceneImage.cols, sceneImage.rows)
        && SanityCheck::checkRectangle(objectCornersTransformed);
    profiler->stopTimer(TIMER_VALIDATE);
        
    // set out params
    currentImage.copyTo(params.sceneImagePrevious);
    params.isObjectPresent = validHomography;
    params.homography = homography;
    params.points = pointsOut;
    
    // set debug info values
    float divergence = 0;
    vector<Point2f> prevObjectCornersTrans = debugInfo.transformedObjectCorners;
    if (objectCornersTransformed.size() == prevObjectCornersTrans.size()) {
        for (int i = 0; i < prevObjectCornersTrans.size(); i++) {
            Point2f vec = (objectCornersTransformed[i] - prevObjectCornersTrans[i]);
            divergence += sqrt(vec.x * vec.x + vec.y * vec.y);
        }
        divergence /= prevObjectCornersTrans.size();
    }
    debugInfo.divergence = divergence;
    debugInfo.transformedObjectCorners = objectCornersTransformed;
    debugInfo.badHomography = !validHomography;
    debugInfo.homography = homography;
    
    return validHomography;
}

} // end of namespace
