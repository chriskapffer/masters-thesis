//
//  ValidationModule.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 22.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_ValidationModule_h
#define CKObjectTrackerLib_ValidationModule_h

#include "AbstractModule.h"
#include "MatchAndFilterDescriptors.h"

#define MIN_MATCHES 4

namespace ck {

class ValidationModule : public AbstractModule {

public:
    ValidationModule(const std::vector<FilterFlag>& filterFlags, int estimationMethod = CV_RANSAC, bool refineHomography = true, bool convertToGray = false, bool sortMatches = false, int ransacThreshold = 3, float ratio = 0.7f, int nBestMatches = 20);
    ~ValidationModule();
    void initWithObjectImage(const cv::Mat& objectImage);
    bool internalProcess(ModuleParams& params, TrackerDebugInfo& debugInfo);
    
#pragma mark
    
    inline void setFilterFlags(const std::vector<FilterFlag>& value) { _filterFlags = value; }
    inline std::vector<FilterFlag> getFilterFlags() const { return _filterFlags; }
    
    inline void setConvertToGray(bool value) { _convertToGray = value; initWithObjectImage(_objectImage); }
    inline bool getConvertToGray() const { return _convertToGray; }
    
    inline void setSortMatches(bool value) { _sortMatches = value; }
    inline bool getSortMatches() const { return _sortMatches; }
    
    inline void setRefineHomography(bool value) { _refineHomography = value; }
    inline bool getRefineHomography() const { return _refineHomography; }
    
    inline void setEstimationMethod(int value) { _estimationMethod = value; }
    inline int getEstimationMethod() const { return _estimationMethod; }
    
    inline void setRansacThreshold(int value) { _ransacThreshold = value; }
    inline int setRansacThreshold() const { return _ransacThreshold; }
    
    inline void setNBestMatches(int value) { _nBestMatches = value; }
    inline int getNBestMatches() const { return _nBestMatches; }
    
    void setRatio(float value) { _ratio = value; }
    float getRatio() const { return _ratio; }
    
    // TODO: getter and setter for detector, extractor, matcher
    
//    inline void setDetector(const std::string& value) { _detector = cv::FeatureDetector::create(value); initWithObjectImage(_objectImage); }
//    inline std::string getDetector() const { return typeid(_detector).name(); }
//    
//    inline void setExtractor(const std::string& value) { _extractor = cv::DescriptorExtractor::create(value); initWithObjectImage(_objectImage); }
//    inline std::string getExtractor() const { return typeid(_extractor).name(); }
    
private:
    // validator params
    std::vector<FilterFlag> _filterFlags;
    bool _convertToGray;
    bool _sortMatches;
    bool _refineHomography;

    int _estimationMethod;
    int _ransacThreshold;
    int _nBestMatches;
    float _ratio;
    
    // detector, extractor, matcher params
    cv::Ptr<cv::FeatureDetector> _detector;
    cv::Ptr<cv::DescriptorExtractor> _extractor;
    cv::Ptr<cv::DescriptorMatcher> _matcher;
    
    // internal data
    std::vector<cv::Point2f> _objectCorners;
    std::vector<cv::KeyPoint> _objectKeyPoints;
    cv::Mat _objectDescriptors;
    cv::Mat _objectImage;
};
    
} // end of namespace

#endif
