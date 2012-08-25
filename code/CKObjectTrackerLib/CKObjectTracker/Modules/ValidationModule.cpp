//
//  ValidationModule.cpp
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 22.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#include "ValidationModule.h"
#include "Profiler.h"
#include "Utils.h"

using namespace std;
using namespace cv;

ValidationModule::ValidationModule() : AbstractModule(MODULE_TYPE_VALIDATION)
{
    Ptr<Feature2D> orb = new ORB();
    _detector = FeatureDetector::create("GFTT");
    _extractor = DescriptorExtractor::create("BRIEF");
    _matcher = new BFMatcher(NORM_HAMMING);
}

ValidationModule::~ValidationModule()
{

}

void ValidationModule::initWithObjectImage(const cv::Mat &objectImage)
{
    
}

bool ValidationModule::internalProcess(ModuleInOutParams& params, ModuleDebugParams& debugInfo)
{
    return true;
}

//bool ValidationModule::validate(const Mat& frame, const CKObjectData& objData, CKTrackingResult& previous, CKTrackingResult& current, CKTrackingInfo& info)
//{
//    Profiler* profiler = Profiler::Instance();
//    vector<Point2f> objectCoordinates;
//    vector<Point2f> sceneCoordinates;
//    vector<KeyPoint> sceneKeypoints;
//    vector<DMatch> filteredMatches;
//    vector<DMatch> allMatches;
//    Mat sceneDescriptors;
//    Mat homography;
//    
//    Mat regionOfInterest = frame(previous.candidates[0]);
//    // detect and extract
//    detectAndExtract(regionOfInterest, sceneKeypoints, sceneDescriptors);
//    if (sceneKeypoints.size() < _minKeyPoints) {
//        cout << "Not enough keypoints. " << endl;
//        return false;
//    }
//    
//    // find matches
//    profiler->startTimer(MATCHING_TIMER_NAME);
//    Utils::matchSimple(_matcher, objData.descriptors, sceneDescriptors, allMatches, true);
//    if (allMatches.size() < _minMatches) {
//        profiler->stopTimer(MATCHING_TIMER_NAME);
//        cout << "Not enough matches. " << endl;
//        return false;
//    }
//    
//    // filter matches
//    //filteredMatches = Utils::nBestMatches(allMatches, 64);
//    filteredMatches = allMatches;
//    
//    profiler->stopTimer(MATCHING_TIMER_NAME);
//        
//    // get coordinates of matches in object and scene image to calculate homographic transformation
//    Utils::get2DCoordinatesOfMatches(filteredMatches, objData.keyPoints, sceneKeypoints,
//                                       objectCoordinates, sceneCoordinates);
//
//    int method = CV_RANSAC;
//    profiler->startTimer(HOMOGRAPHY_TIMER_NAME);
//    Utils::calcHomography(objectCoordinates, sceneCoordinates, homography, method, _ransacThreshold);
//    profiler->stopTimer(HOMOGRAPHY_TIMER_NAME);
//
//    // TODO validate mit umlauf, determinate, singularität
//    int width = objData.image.cols;
//    int height = objData.image.rows;
//    vector<Point2f> objectCorners(4);
//    vector<Point2f> objectCornersTransformed;
//    
//    objectCorners[0] = cvPoint(    0,      0);
//    objectCorners[1] = cvPoint(width,      0);
//    objectCorners[2] = cvPoint(width, height);
//    objectCorners[3] = cvPoint(    0, height);
//    perspectiveTransform(objectCorners, objectCornersTransformed, homography);
//    
//    current.valid = true;
//    
//    // output
//    current.searchRect = previous.candidates[0];
//    current.points = sceneCoordinates;
//    current.homography = homography;
//    
//    vector<char> mask = Utils::createMaskWithOriginalPointSet(objectCoordinates, sceneCoordinates, homography, _ransacThreshold);
//    info.outliers = Utils::filteredMatchesWithMask(allMatches, Utils::invertedMask(mask));
//    info.inliers = Utils::filteredMatchesWithMask(allMatches, mask);
//    info.corners = objectCornersTransformed;
//    info.keyPoints = sceneKeypoints;
//    
//    return true;
//}
//
//void ValidationModule::detectAndExtract(const Mat& image, vector<KeyPoint>& keypoints, Mat& descriptors)
//{
//    detectAndExtract(image, _detector, _extractor, keypoints, descriptors);
//}
//
//void ValidationModule::detectAndExtract(const Mat& image, const Ptr<FeatureDetector>& detector, const Ptr<DescriptorExtractor>& extractor, vector<KeyPoint>& keypoints, Mat& descriptors)
//{
//    Profiler* profiler = Profiler::Instance();
//    
//    // detect key points
//    profiler->startTimer(DETECTION_TIMER_NAME);
//    detector->detect(image, keypoints);
//    profiler->stopTimer(DETECTION_TIMER_NAME);
//    
//    // extract descriptors
//    profiler->startTimer(EXTRACTION_TIMER_NAME);
//    extractor->compute(image, keypoints, descriptors);
//    profiler->stopTimer(EXTRACTION_TIMER_NAME);
//}

// detector, descriptor, matcher