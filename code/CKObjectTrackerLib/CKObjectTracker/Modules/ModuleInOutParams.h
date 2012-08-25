//
//  ModuleInOutParams.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 25.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_ModuleInOutParams_h
#define CKObjectTrackerLib_ModuleInOutParams_h

#include "ModuleTypes.h"

struct ModuleInOutParams {
    ModuleType successor;
    cv::Mat sceneImage;
    cv::Mat homography;
    bool isObjectPresent;
    
    ModuleInOutParams() : successor(MODULE_TYPE_EMPTY), homography(cv::Mat()) {};
};

#endif
