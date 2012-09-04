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

//#include <opencv2/nonfree/nonfree.hpp>

#define TIMER_CONVERT "converting"
#define TIMER_DETECT "detecting"
#define TIMER_EXTRACT "extracting"
#define TIMER_ESTIMATE "estimating"
#define TIMER_VALIDATE "validating"

using namespace std;
using namespace cv;

namespace ck {

    ValidationModule::ValidationModule(const vector<FilterFlag>& filterFlags, int estimationMethod, bool refineHomography, bool convertToGray, bool sortMatches, int ransacThreshold, float ratio, int nBestMatches) : AbstractModule(MODULE_TYPE_VALIDATION)
{
    // validator params
    _filterFlags = filterFlags;
    _convertToGray = convertToGray;
    _sortMatches = sortMatches;
    _refineHomography = refineHomography;
    
    _estimationMethod = estimationMethod;
    _ransacThreshold = ransacThreshold;
    _nBestMatches = nBestMatches;
    _ratio = ratio;
    
    // detector, extractor, matcher params
    Ptr<Feature2D> orb = new ORB();
    _detector = orb;
    _extractor = orb;
    _matcher = new BFMatcher(NORM_HAMMING);
}

ValidationModule::~ValidationModule()
{

}

void ValidationModule::initWithObjectImage(const cv::Mat &objectImage) // TODO: debug info
{
    Profiler* profiler = Profiler::Instance();

    if (_convertToGray) {
        profiler->startTimer(TIMER_CONVERT);
        utils::bgrOrBgra2Gray(objectImage, _objectImage, COLOR_CONV_CV);
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
    // declaration
    Profiler* profiler;
    Mat homography;
    Mat sceneImagePart;
    Mat sceneImageFull;
    Mat sceneDescriptors;
    vector<KeyPoint> sceneKeyPoints;
    vector<Point2f> sceneCoordinates;
    vector<Point2f> objectCoordinates;
    vector<Point2f> objectCornersTransformed;
    vector<unsigned char> mask;
    vector<DMatch> matches;
    bool isHomographyValid;
    Rect searchRect;
    
    // initialization
    profiler = Profiler::Instance();
    searchRect = params.searchRect;
    sceneImageFull = params.sceneImageCurrent;
    sceneImagePart = sceneImageFull(searchRect);
    
    // clear module specific debug information
    debugInfo.objectCornersTransformed.clear();
    debugInfo.namedMatches.clear();
    debugInfo.badHomography = false;
    
    // get region of interest and convert to gray if desired
    if (_convertToGray) {
        profiler->startTimer(TIMER_CONVERT);
        utils::bgrOrBgra2Gray(sceneImagePart, sceneImagePart, COLOR_CONV_CV);
        profiler->stopTimer(TIMER_CONVERT);
    }

    // detect keypoints and extract features (part scene image space)
    profiler->startTimer(TIMER_DETECT);
    _detector->detect(sceneImagePart, sceneKeyPoints);
    profiler->stopTimer(TIMER_DETECT);
    profiler->startTimer(TIMER_EXTRACT);
    _extractor->compute(sceneImagePart, sceneKeyPoints, sceneDescriptors);
    profiler->stopTimer(TIMER_EXTRACT);

    // store available metrics in debug info
    debugInfo.objectImage = _objectImage;
    debugInfo.objectKeyPoints = _objectKeyPoints;
    debugInfo.sceneImagePart = sceneImagePart;
    debugInfo.sceneKeyPoints = sceneKeyPoints;
    
    // match and filter descriptors (uses internal profiling) (partial scene image space)
    MatcherFilter::getFilteredMatches(*_matcher, _objectDescriptors, sceneDescriptors, matches, _filterFlags, _sortMatches, _ratio, _nBestMatches, debugInfo.namedMatches);
    
    // check if there are enough matches left, stop validation if not
    if (matches.size() < MIN_MATCHES) {
        cout << "Not enough matches!" << endl;
        return false;
    }

    // compute homography from matches (full scene image space)
    profiler->startTimer(TIMER_ESTIMATE);
    utils::get2DCoordinatesOfMatches(matches, _objectKeyPoints, sceneKeyPoints, objectCoordinates, sceneCoordinates, Point2f(), searchRect.tl());
    homography = findHomography(objectCoordinates, sceneCoordinates, _estimationMethod, _ransacThreshold, mask);
    profiler->stopTimer(TIMER_ESTIMATE);
    
    // recalculate homography using only inliers from first calculation (full scene image space)
    if (_refineHomography) {
        vector<DMatch> noOutliers;
        // filter matches with mask and compute homography again (uses internal profiling)
        MatcherFilter::filterMatchesWithMask(matches, mask, noOutliers, debugInfo.namedMatches);
        profiler->startTimer(TIMER_ESTIMATE);
        utils::get2DCoordinatesOfMatches(noOutliers, _objectKeyPoints, sceneKeyPoints, objectCoordinates, sceneCoordinates, Point2f(), searchRect.tl());
        homography = findHomography(objectCoordinates, sceneCoordinates, 0); // default method, because there are no outliers
        profiler->stopTimer(TIMER_ESTIMATE);
    }
    
    // validate homography matrix
    profiler->startTimer(TIMER_VALIDATE);
    isHomographyValid = SanityCheck::validate(homography, Size(sceneImageFull.cols, sceneImageFull.rows), _objectCorners, objectCornersTransformed);
    profiler->stopTimer(TIMER_VALIDATE);
    
    // get positions of all scene keypoints again (sceneCoordinates contained positions of matches only) (full scene image space)
    utils::get2DCoordinatesOfKeyPoints(sceneKeyPoints, sceneCoordinates, searchRect.tl());

    // set output params
    params.sceneImageCurrent.copyTo(params.sceneImagePrevious);
    params.isObjectPresent = isHomographyValid;
    params.homography = homography;
    params.points = sceneCoordinates;

    // set debug info values
    debugInfo.jitterAmount = utils::averageDistance(objectCornersTransformed, debugInfo.objectCornersTransformed);
    debugInfo.objectCornersTransformed = objectCornersTransformed;
    debugInfo.badHomography = !isHomographyValid;
    debugInfo.homography = homography;

    return isHomographyValid;
}

} // end of namespace
