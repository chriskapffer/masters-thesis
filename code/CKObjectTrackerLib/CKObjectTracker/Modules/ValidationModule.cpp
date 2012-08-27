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

namespace ck {

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
    //timer
    _detector->detect(objectImage, _objectKeypoints);
    //timer
    _extractor->compute(objectImage, _objectKeypoints, _objectDescriptors);
}  
   

    
    void getFilteredMatches(const DescriptorMatcher& matcher, const Mat& descriptors1, const Mat& descriptors2, vector<DMatch>& result, const vector<FilterFlag>& flags, bool sortMatches, float ratio, int nBestMatches) {

        if(contains(flags, RATIO_FILTER)) {
            if (contains(flags, SYMMETRY_FILTER)) {
                // both, ratio- and symmetry test are set
                vector<vector<DMatch> > matches12;
                vector<vector<DMatch> > matches21;
                matcher.knnMatch(descriptors1, descriptors2, matches12, 2);
                matcher.knnMatch(descriptors2, descriptors1, matches21, 2);
                if (sortMatches) {
                    sort(matches12.begin(), matches12.end(), compareKnnMatch);
                    sort(matches21.begin(), matches21.end(), compareKnnMatch);
                }
                vector<vector<DMatch> > tmp;
                bool didSymmetryMatch = false;
                for (int i = 0; i < flags.size(); i++) {
                    switch (flags[i]) {
                        case CROP_FILTER:
                            utils::nBestMatches(matches12, tmp, nBestMatches, sortMatches);
                            matches12 = tmp;
                            if (!didSymmetryMatch) {
                                utils::nBestMatches(matches21, tmp, nBestMatches, sortMatches);
                                matches21 = tmp;
                            }
                            break;
                        case RATIO_FILTER:
                            utils::ratioTest(matches12, tmp, ratio);
                            matches12 = tmp;
                            if (!didSymmetryMatch) {
                                utils::ratioTest(matches21, tmp, ratio);
                                matches21 = tmp;
                            }
                            break;
                        case SYMMETRY_FILTER:
                            utils::symmetryTest(matches12, matches21, tmp);
                            matches12 = tmp;
                            didSymmetryMatch = true;
                            break;
                    }
                }
                utils::stripNeighbors(matches12, result);
            } else {
                vector<vector<DMatch> > matches;
                matcher.knnMatch(descriptors1, descriptors2, matches, 2);
                if (sortMatches) { sort(matches.begin(), matches.end(), compareKnnMatch); }
                vector<vector<DMatch> > tmp;
                for (int i = 0; i < flags.size(); i++) {
                    switch (flags[i]) {
                        case CROP_FILTER:
                            utils::nBestMatches(matches, tmp, nBestMatches, sortMatches);
                            matches = tmp;
                            break;
                        case RATIO_FILTER:
                            utils::ratioTest(matches, tmp, ratio);
                            matches = tmp;
                            break;
                        default:
                            break;
                    }
                }
                utils::stripNeighbors(matches, result);
            }
        } else {
            if (contains(flags, SYMMETRY_FILTER)) {
                vector<DMatch> matches12;
                vector<DMatch> matches21;
                matcher.match(descriptors1, descriptors2, matches12);
                matcher.match(descriptors2, descriptors1, matches21);
                if (sortMatches) {
                    sort(matches12.begin(), matches12.end());
                    sort(matches21.begin(), matches21.end());
                }
                vector<DMatch> tmp;
                bool didSymmetryMatch = false;
                for (int i = 0; i < flags.size(); i++) {
                    switch (flags[i]) {
                        case CROP_FILTER:
                            utils::nBestMatches(matches12, tmp, nBestMatches, sortMatches);
                            matches12 = tmp;
                            if (!didSymmetryMatch) {
                                utils::nBestMatches(matches21, tmp, nBestMatches, sortMatches);
                                matches21 = tmp;
                            }
                            break;
                        case SYMMETRY_FILTER:
                            utils::symmetryTest(matches12, matches21, tmp);
                            matches12 = tmp;
                            didSymmetryMatch = true;
                            break;
                        default:
                            break;
                    }
                }
                result = matches12;
            } else {
                vector<DMatch> matches;
                matcher.match(descriptors1, descriptors2, matches);
                if (sortMatches) { sort(matches.begin(), matches.end()); }
                
                vector<DMatch> tmp;
                if (!flags.empty()) { // can not be anything else than crop
                    utils::nBestMatches(matches, tmp, nBestMatches, sortMatches);
                    matches = tmp;
                }
                
                result = matches;
            }
        }
    }

bool ValidationModule::internalProcess(ModuleParams& params, TrackerDebugInfo& debugInfo)
{
    float ratio = 0.65f;
    
    Mat sceneDescriptors;
    vector<KeyPoint> sceneKeyPoints;
    
    //timer
    _detector->detect(params.sceneImage, sceneKeyPoints);
    //timer
    _extractor->compute(params.sceneImage, sceneKeyPoints, sceneDescriptors);
    
    int nBest = 20;
    
    // crop, ratio, sym
    {
        vector<vector<DMatch> > matches12;
        vector<vector<DMatch> > matches21;
        _matcher->knnMatch(_objectDescriptors, sceneDescriptors, matches12, 2);
        _matcher->knnMatch(sceneDescriptors, _objectDescriptors, matches21, 2);
        
        vector<vector<DMatch> > nBestMatches12;
        vector<vector<DMatch> > nBestMatches21;
        utils::nBestMatches(matches12, nBestMatches12, nBest, false);
        utils::nBestMatches(matches21, nBestMatches21, nBest, false);
        
        vector<DMatch> afterRatioTest12;
        vector<DMatch> afterRatioTest21;
        utils::ratioTest(nBestMatches12, afterRatioTest12, ratio);
        utils::ratioTest(nBestMatches21, afterRatioTest21, ratio);
        
        vector<DMatch> afterSymTest;
        utils::symmetryTest(afterRatioTest12, afterRatioTest21, afterSymTest);
    }
    
    
    if (true) { // ratio present;
        vector<vector<DMatch> > matches;
        vector<vector<DMatch> > filtered;
        _matcher->knnMatch(_objectDescriptors, sceneDescriptors, matches, 2);
        utils::ratioTest(matches, filtered, ratio);
    }
    
    
    
    
    if (true) { // ratio test
        vector<vector<DMatch> > matches;
        vector<DMatch> filtered;
        _matcher->knnMatch(_objectDescriptors, sceneDescriptors, matches, 2);
        utils::ratioTest(matches, filtered, ratio);
    }
    
    if (true) { // cross check
        vector<DMatch> matchesObj2Scn;
        vector<DMatch> matchesScn2Obj;
        vector<DMatch> filtered;
        _matcher->match(_objectDescriptors, sceneDescriptors, matchesObj2Scn);
        _matcher->match(sceneDescriptors, _objectDescriptors, matchesScn2Obj);
        utils::symmetryTest(matchesObj2Scn, matchesScn2Obj, filtered);
    }
    
    
    
//    if ((_filterStrategies & FILTER_RATIOTEST) == FILTER_RATIOTEST) {
//        _matcher->match(_objectDescriptors, sceneDescriptors, <#vector<cv::DMatch> &matches#>)
//    }
//    
//    if ((_filterStrategies & FILTER_RATIOTEST) == FILTER_RATIOTEST
//        || (_filterStrategies & FILTER_CROSSCHECK) == FILTER_CROSSCHECK)){
//    
//    } else {
//    
//    }
    
    
    return true;
}

} // end of namespace
    
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