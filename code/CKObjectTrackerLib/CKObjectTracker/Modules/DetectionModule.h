//
//  DetectionModule.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 22.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_DetectionModule_h
#define CKObjectTrackerLib_DetectionModule_h

#include "AbstractModule.h"

namespace ck {

class DetectionModule : public AbstractModule {
    
private:
    cv::Mat _objectImage;
    std::vector<cv::Vec4i> _objectHierarchy;
    std::vector<std::vector<cv::Point> > _objectContours;
    
    cv::MatND _objectHist;
    
    float _detectionThreshold;
    bool _preProcess;
    bool _byPass;
    
    void initEdges(const cv::Mat &objectImage);
    void initHist(const cv::Mat &objectImage);
    bool matchEdges(ModuleParams& params, TrackerDebugInfo& debugInfo);
    bool matchHist(ModuleParams& params, TrackerDebugInfo& debugInfo);
    
public:
    DetectionModule();
    ~DetectionModule();
    void initWithObjectImage(const cv::Mat& objectImage);
    bool internalProcess(ModuleParams& params, TrackerDebugInfo& debugInfo);
    
};

} // end of namespace
    
#endif
