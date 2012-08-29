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

private:
    std::vector<cv::Point2f> _objectCorners;
    std::vector<cv::KeyPoint> _objectKeyPoints;
    cv::Mat _objectDescriptors;
    cv::Mat _objectImage;
    
    cv::Ptr<cv::FeatureDetector> _detector;
    cv::Ptr<cv::DescriptorExtractor> _extractor;
    cv::Ptr<cv::DescriptorMatcher> _matcher;
    
    std::vector<FilterFlag> _filterFlags;
    int _ransacThreshold;
    int _nBestMatches;
    bool _sortMatches;
    bool _convertGray;
    float _ratio;
    
    int _estimationMethod;
    bool _refineHomography;
    
public:
    ValidationModule();
    ~ValidationModule();
    void initWithObjectImage(const cv::Mat& objectImage);
    bool internalProcess(ModuleParams& params, TrackerDebugInfo& debugInfo);

};
    
} // end of namespace

#endif
