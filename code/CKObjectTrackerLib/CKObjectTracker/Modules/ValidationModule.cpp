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

#define TIMER_DETECT "detection"
#define TIMER_EXTRACT "extraction"
#define TIMER_ESTIMATE "estimation"
#define TIMER_VALIDATE "validation"

using namespace std;
using namespace cv;

namespace ck {

ValidationModule::ValidationModule() : AbstractModule(MODULE_TYPE_VALIDATION)
{
    Ptr<Feature2D> orb = new ORB();
    _detector = orb;
    _extractor = orb;
    _matcher = new BFMatcher(NORM_HAMMING);
    
    _filterFlags = vector<FilterFlag>();
    _filterFlags.push_back(FILTER_FLAG_RATIO);
    //_filterFlags.push_back(FILTER_FLAG_SYMMETRY);
    _filterFlags.push_back(FILTER_FLAG_CROP);
    _sortMatches = true;
    _nBestMatches = 32;
    _ratio = 0.65f;
    
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
    _objectImage = objectImage;
    
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
    
    // get region of interest
    sceneImage = params.sceneImage(params.searchRect);
    
    // detect keypoints and extract features
    Profiler* profiler = Profiler::Instance();
    profiler->startTimer(TIMER_DETECT);
    _detector->detect(params.sceneImage, sceneKeyPoints);
    profiler->stopTimer(TIMER_DETECT);
    profiler->startTimer(TIMER_EXTRACT);
    _extractor->compute(params.sceneImage, sceneKeyPoints, sceneDescriptors);
    profiler->stopTimer(TIMER_EXTRACT);

    // set first bit of debug info values
    debugInfo.objectKeyPoints = _objectKeyPoints;
    debugInfo.objectImage = _objectImage;
    debugInfo.sceneKeyPoints = sceneKeyPoints;
    debugInfo.sceneImage = sceneImage;
    
    // match and filter descriptors
    MatcherFilter::getFilteredMatches(*_matcher, _objectDescriptors, sceneDescriptors, matches, _filterFlags, _sortMatches, _ratio, _nBestMatches, debugInfo.namedMatches);
    if (matches.size() < MIN_MATCHES) {
        cout << "Not enough matches!" << endl;
        // set rest of debug info values in case of failure
        debugInfo.subTaskProcessingTimes.push_back(make_pair(TIMER_DETECT, profiler->elapsedTime(TIMER_DETECT)));
        debugInfo.subTaskProcessingTimes.push_back(make_pair(TIMER_EXTRACT, profiler->elapsedTime(TIMER_EXTRACT)));
        debugInfo.subTaskProcessingTimes.push_back(make_pair(TIMER_MATCH, profiler->elapsedTime(TIMER_MATCH)));
        debugInfo.subTaskProcessingTimes.push_back(make_pair(TIMER_FILTER, profiler->elapsedTime(TIMER_FILTER)));
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
    bool validHomography = SanityCheck::checkRectangle(objectCornersTransformed);
    profiler->stopTimer(TIMER_VALIDATE);

    // set out params
    params.points = sceneCoordinates;
    params.isObjectPresent = validHomography;
    params.homography = homography;
    
    // set rest of debug info value
    debugInfo.subTaskProcessingTimes.push_back(make_pair(TIMER_DETECT, profiler->elapsedTime(TIMER_DETECT)));
    debugInfo.subTaskProcessingTimes.push_back(make_pair(TIMER_EXTRACT, profiler->elapsedTime(TIMER_EXTRACT)));
    debugInfo.subTaskProcessingTimes.push_back(make_pair(TIMER_MATCH, profiler->elapsedTime(TIMER_MATCH)));
    debugInfo.subTaskProcessingTimes.push_back(make_pair(TIMER_FILTER, profiler->elapsedTime(TIMER_FILTER)));
    debugInfo.subTaskProcessingTimes.push_back(make_pair(TIMER_ESTIMATE, profiler->elapsedTime(TIMER_ESTIMATE)));
    debugInfo.subTaskProcessingTimes.push_back(make_pair(TIMER_VALIDATE, profiler->elapsedTime(TIMER_VALIDATE)));
    debugInfo.transformedObjectCorners = objectCornersTransformed;
    debugInfo.badHomography = !validHomography;
    debugInfo.homography = homography;

    return validHomography;
}

} // end of namespace
