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

class ValidationModule : public AbstractModule {

private:
    cv::Ptr<cv::FeatureDetector> _detector;
    cv::Ptr<cv::DescriptorExtractor> _extractor;
    cv::Ptr<cv::DescriptorMatcher> _matcher;
    
    std::vector<cv::KeyPoint> _objectKeypoints;
    cv::Mat _objectDescriptors;
    cv::Mat _objectImage;
    
public:
    ValidationModule();
    ~ValidationModule();
    void initWithObjectImage(const cv::Mat& objectImage);
    bool internalProcess(ModuleInOutParams& params, ModuleDebugParams& debugInfo);

};
    
} // end of namespace

#endif
