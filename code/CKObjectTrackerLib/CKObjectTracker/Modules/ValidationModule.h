//
//  ValidationModule.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 22.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_ValidationModule_h
#define CKObjectTrackerLib_ValidationModule_h

#include <opencv2/nonfree/nonfree.hpp>

#include "AbstractModule.h"

#include "DescriptorMatcherFilterer.h"

namespace ck {

class ValidationModule : public AbstractModule {

public:
    ValidationModule(const std::vector<FilterFlag>& filterFlags, int estimationMethod = CV_RANSAC, bool refineHomography = true, bool convertToGray = false, bool sortMatches = false, int ransacThreshold = 3, float ratio = 0.7f, int nBestMatches = 20);
    ~ValidationModule();
    void initWithObjectImage(const cv::Mat& objectImage);
    bool internalProcess(ModuleParams& params, TrackerDebugInfo& debugInfo);
    
#pragma mark
    
    void setDetector(const std::string& value);
    std::string getDetector() const;
    
    void setExtractor(const std::string& value);
    std::string getExtractor() const;
    
    void setMaxFeatures(const int& value);
    inline int getMaxFeatures() const { return _maxFeatures; }
    
    void setFastThreshold(const int& value);
    inline int getFastThreshold() const { return _fastThreshold; }
    
    void setHessianThreshold(const float& value);
    inline float getHessianThreshold() const { return _hessianThreshold; }
    
    void setCropMatches(const bool& value);
    bool getCropMatches() const;
    
    void setRatioTestEnabled(const bool& value);
    bool getRatioTestEnabled() const;
    
    void setSymmetryTestEnabled(const bool& value);
    bool getSymmetryTestEnabled() const;
    
    void setEstimationMethod(const std::string& value);
    std::string getEstimationMethod() const;
    
    inline void setFilterFlags(const std::vector<FilterFlag>& value) { _filterFlags = value; }
    inline std::vector<FilterFlag> getFilterFlags() const { return _filterFlags; }
    
    inline void setConvertToGray(const bool& value) { _convertToGray = value; }
    inline bool getConvertToGray() const { return _convertToGray; }
    
    inline void setSortMatches(const bool& value) { _sortMatches = value; }
    inline bool getSortMatches() const { return _sortMatches; }
    
    inline void setNBestMatches(const int& value) { _nBestMatches = value; }
    inline int getNBestMatches() const { return _nBestMatches; }
    
    inline void setRatio(const float& value) { _ratio = value; }
    inline float getRatio() const { return _ratio; }
    
    inline void setRansacThreshold(const int& value) { _ransacThreshold = value; }
    inline int setRansacThreshold() const { return _ransacThreshold; }
    
    inline void setRefineHomography(const bool& value) { _refineHomography = value; }
    inline bool getRefineHomography() const { return _refineHomography; }
    
    inline void setUseAllKeyPointsForOutput(const bool& value) { _useAllKeyPointsForOutput = value; }
    inline bool getUseAllKeyPointsForOutput() const { return _useAllKeyPointsForOutput; }
    
#pragma mark
    
private:
    void setExtractor(const std::string& value, bool updateMatcher);
    
    bool _convertToGray;
    bool _sortMatches;
    
    // extracting params
    int _maxFeatures;
    int _fastThreshold;
    float _hessianThreshold;
    
    // filtering params
    std::vector<FilterFlag> _filterFlags;
    int _nBestMatches;
    float _ratio;
    
    // estimation params
    int _estimationMethod;
    int _ransacThreshold;
    bool _refineHomography;
    bool _useAllKeyPointsForOutput;

    // detector, extractor, matcher params    
    cv::Ptr<cv::FeatureDetector> _detector;
    cv::Ptr<cv::DescriptorExtractor> _extractor;
    cv::Ptr<cv::DescriptorMatcher> _matcher;
    
    // internal data
    std::vector<cv::Point2f> _objectCorners;
    std::vector<cv::KeyPoint> _objectKeyPoints;
    cv::Mat _objectDescriptors;
    cv::Mat _objectImage;
    bool _isDirty;
};
    
} // end of namespace

#endif
