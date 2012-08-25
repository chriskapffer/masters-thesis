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

class DetectionModule : public AbstractModule {
    
private:
    float _detectionThreshold;
    
public:
    DetectionModule();
    ~DetectionModule();
    void initWithObjectImage(const cv::Mat& objectImage);
    bool internalProcess(ModuleInOutParams& params, ModuleDebugParams& debugInfo);
    
};

#endif
