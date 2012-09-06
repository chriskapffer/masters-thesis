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

#define TIMER_CONVERT "converting"
#define TIMER_DETECT "detecting"
#define TIMER_EXTRACT "extracting"
#define TIMER_ESTIMATE "estimating"
#define TIMER_VALIDATE "validating"

using namespace std;
using namespace cv;

namespace ck {
    
template<typename T>
static bool vectorContains(const vector<T>& vector, const T& object)
{
    return find(vector.begin(), vector.end(), object) != vector.end();
}
    
static string getAlgorithmName(const Algorithm& algorithm)
{
    string name = algorithm.name();
    string::iterator begin = name.begin();
    size_t pos = name.find_last_of('.') + 1;
    name.erase(begin, begin + pos);
    return name;
}
    
#pragma mark
    
void ValidationModule::setDetector(const string& value)
{
    _busy = true; // TODO: change _extractor appropriately
    if (value == "FAST") {
        _detector = new FastFeatureDetector(_fastThreshold);
    } else if (value == "GFTT") {
        _detector = new GoodFeaturesToTrackDetector(_maxFeatures);
    } else if (value == "SIFT") {
        _detector = new SiftFeatureDetector(_maxFeatures);
    } else if (value == "SURF") {
        _detector = new SurfFeatureDetector(_hessianThreshold, 4, 2, false);
    } else if (value == "ORB") {
        _detector = new OrbFeatureDetector(_maxFeatures);
    } else {
        throw "Extractor not supported.";
    }
    _busy = false;
}

std::string ValidationModule::getDetector() const
{
    return getAlgorithmName(*_detector);
}
    
void ValidationModule::setExtractor(const string &value)
{
    setExtractor(value, true);
}
    
void ValidationModule::setExtractor(const string& value, bool updateMatcher)
{
    _busy = true; // TODO: change _detector appropriately
    if (value == "SIFT") {
        _extractor = new SiftDescriptorExtractor(_maxFeatures);
        if (updateMatcher) { _matcher = new BFMatcher(NORM_L2); }
    } else if (value == "SURF") {
        _extractor = new SurfFeatureDetector(_hessianThreshold, 4, 2, false);
        if (updateMatcher) { _matcher = new BFMatcher(NORM_L2); }
    } else if (value == "ORB") {
        _extractor = new OrbDescriptorExtractor(_maxFeatures);
        if (updateMatcher) { _matcher = new BFMatcher(NORM_HAMMING); }
    } else if (value == "FREAK") {
        _extractor = new FREAK();
        if (updateMatcher) { _matcher = new BFMatcher(NORM_HAMMING); }
    } else {
        throw "Extractor not supported.";
    }
    _busy = false;
}

std::string ValidationModule::getExtractor() const
{
    return getAlgorithmName(*_extractor);
}
    
void ValidationModule::setMaxFeatures(const int& value)
{
    _maxFeatures = value;
    string detectorName = getAlgorithmName(*_detector);
    string extractorName = getAlgorithmName(*_extractor);
    if (detectorName != "FAST" && detectorName != "SURF") {
        setDetector(detectorName); // re-init detector
    }
    if (extractorName != "FREAK" && extractorName != "SURF") {
        setExtractor(extractorName, false); // re-init extractor
    }
}

void ValidationModule::setFastThreshold(const int& value)
{
    _fastThreshold = value;
    if (getAlgorithmName(*_detector) == "FAST") {
        setDetector("FAST"); // re-init detector
    }
}

void ValidationModule::setHessianThreshold(const float& value)
{
    _hessianThreshold = value;
    if (getAlgorithmName(*_detector) == "SURF") {
        setDetector("SURF"); // re-init detector
    }
    if (getAlgorithmName(*_extractor) == "SURF") {
        setExtractor("SURF", false); // re-init extractor
    }
}
    
void ValidationModule::setCropMatches(const bool& value)
{
    vector<FilterFlag>::iterator pos = find(_filterFlags.begin(), _filterFlags.end(), FILTER_FLAG_CROP);
    bool isPresent = pos != _filterFlags.end();
    if (value && !isPresent) {
        _filterFlags.push_back(FILTER_FLAG_CROP);
    } else if (!value && isPresent) {
        _filterFlags.erase(pos);
    }
}
    
bool ValidationModule::getCropMatches() const
{
    return vectorContains(_filterFlags, FILTER_FLAG_CROP);
}
    
void ValidationModule::setRatioTestEnabled(const bool& value)
{
    vector<FilterFlag>::iterator pos = find(_filterFlags.begin(), _filterFlags.end(), FILTER_FLAG_RATIO);
    bool isPresent = pos != _filterFlags.end();
    if (value && !isPresent) {
        _filterFlags.push_back(FILTER_FLAG_RATIO);
    } else if (!value && isPresent) {
        _filterFlags.erase(pos);
    }
}
    
bool ValidationModule::getRatioTestEnabled() const
{
    return vectorContains(_filterFlags, FILTER_FLAG_RATIO);
}
    
void ValidationModule::setSymmetryTestEnabled(const bool& value)
{
    vector<FilterFlag>::iterator pos = find(_filterFlags.begin(), _filterFlags.end(), FILTER_FLAG_SYMMETRY);
    bool isPresent = pos != _filterFlags.end();
    if (value && !isPresent) {
        _filterFlags.push_back(FILTER_FLAG_SYMMETRY);
    } else if (!value && isPresent) {
        _filterFlags.erase(pos);
    }
}
bool ValidationModule::getSymmetryTestEnabled() const
{
    return vectorContains(_filterFlags, FILTER_FLAG_SYMMETRY);
}
    
void ValidationModule::setEstimationMethod(const string& value)
{
    if (value == EST_METHOD_RANSAC) {
        _estimationMethod = CV_RANSAC;
    } else if (value == EST_METHOD_LMEDS) {
        _estimationMethod = CV_LMEDS;
    } else if (value == EST_METHOD_DEFAULT) {
        _estimationMethod = 0;
    } else {
        throw "Invalid Estimation Method";
    }
}

string ValidationModule::getEstimationMethod() const
{
    string result;
    switch (_estimationMethod) {
        case CV_RANSAC:
            result = EST_METHOD_RANSAC;
            break;
        case CV_LMEDS:
            result = EST_METHOD_LMEDS;
            break;
        case 0:
            result = EST_METHOD_DEFAULT;
            break;
        default:
            throw "Invalid Estimation Method";
            break;
    }
    return result;
}
    
#pragma mark
    
ValidationModule::ValidationModule(const vector<FilterFlag>& filterFlags, int estimationMethod, bool refineHomography, bool convertToGray, bool sortMatches, int ransacThreshold, float ratio, int nBestMatches) : AbstractModule(MODULE_TYPE_VALIDATION)
{
    _convertToGray = convertToGray;
    _sortMatches = sortMatches;
    
    // extracting params
    _maxFeatures = 500;
    _fastThreshold = 10;
    _hessianThreshold = 400;
    
    // filtering params
    _filterFlags = filterFlags;
    _nBestMatches = nBestMatches;
    _ratio = ratio;
    
    // estimation params
    _estimationMethod = estimationMethod;
    _ransacThreshold = ransacThreshold;
    _refineHomography = refineHomography;
    
    // detector, extractor, matcher params
    _detector = new OrbDescriptorExtractor(_maxFeatures);
    _extractor = new OrbDescriptorExtractor(_maxFeatures);
    _matcher = new BFMatcher(NORM_HAMMING);
    
    _busy = false;
}

ValidationModule::~ValidationModule()
{
    // nothing to do here, because we are using opencv's smart pointers
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
    
    // wait if detector or extractor is being changed
    while (_busy) { }
    
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

    // wait if detector or extractor is being changed
    while (_busy) { }
    
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
    
    // wait if matcher is being changed
    while (_busy) { }
    
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
