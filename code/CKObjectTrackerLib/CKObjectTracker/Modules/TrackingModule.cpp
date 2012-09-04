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
    // SL: success labs (adapted version of LK) TODO: see: website
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

#pragma mark
    
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
    // declaration
    Profiler* profiler;
    vector<float> error;
    vector<uchar> status;
    vector<Point2f> pointsIn;
    vector<Point2f> pointsOut;
    vector<Point2f> objectCornersTransformed;
    float avgError, avgDistance, maxDistance, minDistance;
    bool isHomographyValid;
    Rect boundingRect;
    Mat previousImage;
    Mat currentImage;
    Mat homography;

    // clear module specific debug information
    debugInfo.namedPointSets.clear();
    
    // don't go any further, if tracker is not enabled
    if (!_enabled) {
        return false;
    }
    
    // initialization
    profiler = Profiler::Instance();
    pointsIn = params.points;
    homography = params.homography;
    currentImage = params.sceneImageCurrent;
    previousImage = params.sceneImagePrevious;
    
    // perform necessary steps in order to proceed
    if(!prepareForProcessing(homography, pointsIn, debugInfo)) {
        // stop tracking and prepare for a new initial call to this module in the future
        _isInitialCall = true;
        return false;
    }
    
    // convert to gray
    profiler->startTimer(TIMER_CONVERT);
    utils::bgrOrBgra2Gray(previousImage, previousImage, COLOR_CONV_CV);
    utils::bgrOrBgra2Gray(currentImage, currentImage, COLOR_CONV_CV);
    profiler->stopTimer(TIMER_CONVERT);
    
    // calculate sub pixel locations if desired
    if (_useSubPixels) {
        profiler->startTimer(TIMER_OPTIMIZE);
        cornerSubPix(previousImage, pointsIn, _winSizeSubPix, _zeroZone, _terminationCriteria);
        profiler->stopTimer(TIMER_OPTIMIZE);
    }
    
    // calculate optical flow
    profiler->startTimer(TIMER_TRACK);
    calcOpticalFlowPyrLK(previousImage, currentImage, pointsIn, pointsOut, status, error, _winSizeFlow, _maxLevel, _terminationCriteria, _lkFlags, _minEigenThreshold);
    removeUntrackedPoints(pointsIn, pointsOut, _initialPointSet, status, error, avgError, avgDistance, maxDistance, minDistance);
    profiler->stopTimer(TIMER_TRACK);
    
    // store remaining points and other metrics in debug info
    debugInfo.namedPointSets.push_back(make_pair(POINTS_TRACKED, pointsOut));
    debugInfo.movementVariation = maxDistance - minDistance;
    debugInfo.avgError = avgError;
    
    // filter new points 
    if (_filterDistortions) {
        profiler->startTimer(TIMER_FILTER);
        filterPointsByMovingDistance(pointsIn, pointsOut, _initialPointSet, avgDistance, _distortionThreshold);
        profiler->stopTimer(TIMER_FILTER);
    }
    // store remaining points in debug info
    debugInfo.namedPointSets.push_back(make_pair(POINTS_FILTERED, pointsOut));

    int currentCount = (int)pointsOut.size();
    // check if there are enough points left, stop tracking if not
    if (currentCount < _minPointsAbsolute || currentCount < _initialPointCount * _minPointsRelative) {
        cout << "Lost too many points." << endl;
        // stop tracking and prepare for a new initial call to this module in the future
        _isInitialCall = true;
        return false;
    }
    
    // compute homography
    profiler->startTimer(TIMER_ESTIMATE);
    if (_calcHomRelToFrame) {
        // multiply homography with transformation between current points and previous points (from last frame)
        homography *= findHomography(pointsIn, pointsOut);
    } else {
        // multiply homography with transformation between current points and initial point set
        homography = findHomography(_initialPointSet, pointsOut) * _initialHomography;
    }
    profiler->stopTimer(TIMER_ESTIMATE);

    // validate homography matrix
    profiler->startTimer(TIMER_VALIDATE);
    isHomographyValid = SanityCheck::validate(homography, Size(currentImage.cols, currentImage.rows), _objectCorners, objectCornersTransformed, boundingRect, true);
    profiler->stopTimer(TIMER_VALIDATE);

    // set output params
    params.sceneImageCurrent.copyTo(params.sceneImagePrevious);
    params.isObjectPresent = isHomographyValid;
    params.homography = homography;
    params.points = pointsOut;

    // set debug info values
    debugInfo.jitterAmount = utils::averageDistance(objectCornersTransformed, debugInfo.objectCornersTransformed);
    debugInfo.objectCornersTransformed = objectCornersTransformed;
    debugInfo.badHomography = !isHomographyValid;
    debugInfo.homography = homography;
    
    if (isHomographyValid) {
        params.searchRect = boundingRect;
        debugInfo.searchRect = boundingRect;
    }
    
    return isHomographyValid;
}

#pragma mark
    
bool TrackingModule::prepareForProcessing(const Mat& homography, vector<Point2f>& pointsIn, TrackerDebugInfo& debugInfo)
{
    // strip end of list, if there are more input points than desired
    if (pointsIn.size() > _maxPointsAbsolute) {
        pointsIn.erase(pointsIn.begin() + _maxPointsAbsolute, pointsIn.end());
    }
    // store current input points in debug info
    debugInfo.namedPointSets.push_back(make_pair(POINTS_TOTAL, pointsIn));
    
    // if this is the initial call to the tracking module, store current points
    // and homography, in order to calculate their relative transformation
    if (_isInitialCall) {
        // reset frame count (used for maxSuccessiveFrames)
        _succFrameCount = 0;
        _isInitialCall = false;
        _initialPointSet = pointsIn;
        _initialPointCount = (int)pointsIn.size(); // store inital point count seperately,
        // cause initial point set will be changed over time
        homography.copyTo(_initialHomography);
        
        // store initial point count in debug info
        debugInfo.initialPointCount = _initialPointCount;
    }

    _succFrameCount++;
    
    // stop tracking after max successive frames (used for homography refinemend via validation module)
    // -1 indicates, that there is no frame limit
    return _maxSuccessiveFrames == -1 || _succFrameCount <= _maxSuccessiveFrames;
}

void TrackingModule::removeUntrackedPoints(vector<Point2f>& pointsIn, vector<Point2f>& pointsOut, vector<Point2f>& initialPoints, const vector<uchar>& status, const vector<float>& error, float& avgError, float& avgDistance, float& maxDistance, float& minDistance)
{
    avgError = 0;
    avgDistance = 0;
    maxDistance = 0;
    minDistance = FLT_MAX;
    
    for (int i = (int)pointsOut.size() - 1; i >= 0; i--) {
        if (!status[i]) {
            // if point could not be tracked remove it (from all lists, to keep them in sync)
            initialPoints.erase(initialPoints.begin() + i);
            pointsOut.erase(pointsOut.begin() + i);
            pointsIn.erase(pointsIn.begin() + i);
            continue;
        }
        // calculate metrics for remaining points (distance between previous and current points, etc.)
        Point2f vec = pointsOut[i] - pointsIn[i];
        float distanceSquared = vec.x * vec.x + vec.y * vec.y;
        maxDistance = MAX(maxDistance, distanceSquared);
        minDistance = MIN(minDistance, distanceSquared);
        avgDistance += distanceSquared;
        avgError += error[i];
    }
    avgDistance /= pointsOut.size();
    avgError /= pointsOut.size();
    
    // apply square root to distances
    maxDistance = sqrt(maxDistance);
    minDistance = sqrt(minDistance);
    avgDistance = sqrt(avgDistance);
}
    
void TrackingModule::filterPointsByMovingDistance(vector<Point2f>& pointsIn, vector<Point2f>& pointsOut, vector<Point2f>& initialPoints, float averageDistance, float distortionThreshold)
{
    float averageDistanceSquared = averageDistance * averageDistance;
    for (int i = (int)pointsOut.size(); i >= 0; i--) {
        Point2f vec = pointsOut[i] - pointsIn[i];
        float distanceSquared = vec.x * vec.x + vec.y * vec.y; // moving distance (squared)
        // remove points if their moving dinstance - average is greater than average times distortionThreshold
        if (fabs(distanceSquared - averageDistanceSquared) > averageDistanceSquared * distortionThreshold) {
            initialPoints.erase(initialPoints.begin() + i);
            pointsOut.erase(pointsOut.begin() + i);
            pointsIn.erase(pointsIn.begin() + i);
        }
    }
}
    
} // end of namespace
