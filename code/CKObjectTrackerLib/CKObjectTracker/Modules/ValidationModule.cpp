//
//  ValidationModule.cpp
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 22.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#include "ValidationModule.h"
#include "SanityCheck.h"
#include "Profiler.h"
#include "Utils.h"

#include <opencv2/nonfree/nonfree.hpp>

#define TIMER_CONVERT "converting"
#define TIMER_DETECT "detecting"
#define TIMER_EXTRACT "extracting"
#define TIMER_ESTIMATE "estimating"
#define TIMER_VALIDATE "validating"

using namespace std;
using namespace cv;

namespace ck {

ValidationModule::ValidationModule() : AbstractModule(MODULE_TYPE_VALIDATION)
{
    Ptr<Feature2D> orb = new ORB();
    Ptr<Feature2D> sift = new SIFT();
    Ptr<Feature2D> surf = new SURF();
    _detector = orb;
    _extractor = orb;
    _matcher = new BFMatcher(NORM_HAMMING);
    
    _filterFlags = vector<FilterFlag>();
    _filterFlags.push_back(FILTER_FLAG_RATIO);
    //_filterFlags.push_back(FILTER_FLAG_CROP);
    //_filterFlags.push_back(FILTER_FLAG_SYMMETRY);

    _convertToGray = false;
    _sortMatches = true;
    _nBestMatches = 8;
    _ratio = 0.7f;
    
    _estimationMethod = CV_RANSAC;
    _ransacThreshold = 3;
    _refineHomography = true;
}

ValidationModule::~ValidationModule()
{

}

void ValidationModule::initWithObjectImage(const cv::Mat &objectImage) // TODO debug info
{
    Profiler* profiler = Profiler::Instance();

    if (_convertToGray) {
        profiler->startTimer(TIMER_CONVERT);
        utils::bgr_a_2Gray(objectImage, _objectImage, COLOR_CONV_CV);
        profiler->stopTimer(TIMER_CONVERT);
    } else {
        objectImage.copyTo(_objectImage);
    }
    
    profiler->startTimer(TIMER_DETECT);
    _detector->detect(_objectImage, _objectKeyPoints);
    profiler->stopTimer(TIMER_DETECT);
    profiler->startTimer(TIMER_EXTRACT);
    _extractor->compute(_objectImage, _objectKeyPoints, _objectDescriptors);
    profiler->stopTimer(TIMER_EXTRACT);
    
    _objectCorners = vector<Point2f>(4); // clock wise
    _objectCorners[0] = cvPoint(                0,                 0); // top left
    _objectCorners[1] = cvPoint(_objectImage.cols,                 0); // top right
    _objectCorners[2] = cvPoint(_objectImage.cols, _objectImage.rows); // bottom right
    _objectCorners[3] = cvPoint(                0, _objectImage.rows); // bottom left
}

bool ValidationModule::internalProcess(ModuleParams& params, TrackerDebugInfo& debugInfo)
{
    Mat sceneImage;
    Mat sceneDescriptors;
    vector<KeyPoint> sceneKeyPoints;
    vector<Point2f> sceneCoordinates;
    vector<Point2f> objectCoordinates;
    vector<unsigned char> mask;
    vector<DMatch> matches;
    Mat homography;
    
    Profiler* profiler = Profiler::Instance();
    
    // clear module specific debug information
    debugInfo.namedMatches.clear();
    
    // get region of interest and convert if desired
    if (_convertToGray) {
        profiler->startTimer(TIMER_CONVERT);
        utils::bgr_a_2Gray(params.sceneImageCurrent(params.searchRect), sceneImage, COLOR_CONV_CV);
        profiler->stopTimer(TIMER_CONVERT);
    } else {
        sceneImage = params.sceneImageCurrent(params.searchRect);
    }

    // detect keypoints and extract features
    profiler->startTimer(TIMER_DETECT);
    _detector->detect(sceneImage, sceneKeyPoints);
    profiler->stopTimer(TIMER_DETECT);
    profiler->startTimer(TIMER_EXTRACT);
    _extractor->compute(sceneImage, sceneKeyPoints, sceneDescriptors);
    profiler->stopTimer(TIMER_EXTRACT);

    // set first bit of debug info values
    debugInfo.objectKeyPoints = _objectKeyPoints;
    debugInfo.objectImage = _objectImage;
    debugInfo.sceneKeyPoints = sceneKeyPoints;
    debugInfo.sceneImagePart = sceneImage;
    
    // match and filter descriptors
    MatcherFilter::getFilteredMatches(*_matcher, _objectDescriptors, sceneDescriptors, matches, _filterFlags, _sortMatches, _ratio, _nBestMatches, debugInfo.namedMatches);
    if (matches.size() < MIN_MATCHES) {
        cout << "Not enough matches!" << endl;
        // set rest of debug info values in case of failure
        map<string, double> timerValues = profiler->getCurrentTimerValues();
        map<string, double>::const_iterator iter;
        for (iter = timerValues.begin(); iter != timerValues.end(); iter++) {
            debugInfo.subTaskProcessingTimes.push_back(make_pair((*iter).first, (*iter).second));
        }
        debugInfo.transformedObjectCorners.clear();
        debugInfo.badHomography = false;
        return false;
    }

    // compute homography from matches
    profiler->startTimer(TIMER_ESTIMATE);
    utils::get2DCoordinatesOfMatches(matches, _objectKeyPoints, sceneKeyPoints, objectCoordinates, sceneCoordinates);
    homography = findHomography(objectCoordinates, sceneCoordinates, _estimationMethod, _ransacThreshold, mask);
    profiler->stopTimer(TIMER_ESTIMATE);
    if (_refineHomography) {
        vector<DMatch> noOutliers;
        // filter matches with mask and compute homography again
        MatcherFilter::filterMatchesWithMask(matches, mask, noOutliers, debugInfo.namedMatches);
        profiler->startTimer(TIMER_ESTIMATE);
        utils::get2DCoordinatesOfMatches(noOutliers, _objectKeyPoints, sceneKeyPoints, objectCoordinates, sceneCoordinates);
        homography = findHomography(objectCoordinates, sceneCoordinates, 0); // default method, because there are no outliers
        profiler->stopTimer(TIMER_ESTIMATE);
    }
    
    // validate homography matrix
    profiler->startTimer(TIMER_VALIDATE);
    vector<Point2f> objectCornersTransformed;
    perspectiveTransform(_objectCorners, objectCornersTransformed, homography);
    bool validHomography = true
        //&& SanityCheck::checkBoundaries(objectCornersTransformed, sceneImage.cols, sceneImage.rows)
        && SanityCheck::checkRectangle(objectCornersTransformed);
    profiler->stopTimer(TIMER_VALIDATE);

    // set out params
    //params.points = sceneCoordinates; // These are only matches, but using the whole keypoint set is ok
                                      // since tracker calculates relative homographies and not absolute ones.
    utils::get2DCoordinatesOfKeyPoints(sceneKeyPoints, params.points);
    params.sceneImageCurrent.copyTo(params.sceneImagePrevious);
    params.isObjectPresent = validHomography;
    params.homography = homography;
    
    // set rest of debug info values
    map<string, double> timerValues = profiler->getCurrentTimerValues();
    map<string, double>::const_iterator iter;
    for (iter = timerValues.begin(); iter != timerValues.end(); iter++) {
        debugInfo.subTaskProcessingTimes.push_back(make_pair((*iter).first, (*iter).second));
    }
    debugInfo.transformedObjectCorners = objectCornersTransformed;
    debugInfo.badHomography = !validHomography;
    debugInfo.homography = homography;

    return validHomography;
}

} // end of namespace
