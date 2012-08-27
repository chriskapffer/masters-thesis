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

namespace ck {

enum FilterFlag {
    CROP_FILTER,
    RATIO_FILTER,
    SYMMETRY_FILTER,
};
    
class ValidationModule : public AbstractModule {

private:
    std::vector<cv::KeyPoint> _objectKeypoints;
    cv::Mat _objectDescriptors;
    cv::Mat _objectImage;
    
    cv::Ptr<cv::FeatureDetector> _detector;
    cv::Ptr<cv::DescriptorExtractor> _extractor;
    cv::Ptr<cv::DescriptorMatcher> _matcher;
    
    std::vector<FilterFlag> _filterFlags;
    bool _sortMatches;
    
    int _estimationMethod;
    int _validationMethod;
    
    bool _refineHomography;
    
public:
    ValidationModule();
    ~ValidationModule();
    void initWithObjectImage(const cv::Mat& objectImage);
    bool internalProcess(ModuleParams& params, TrackerDebugInfo& debugInfo);

};
    
} // end of namespace

#endif
