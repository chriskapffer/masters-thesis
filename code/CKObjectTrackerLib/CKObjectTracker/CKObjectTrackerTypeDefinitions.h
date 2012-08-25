//
//  CKObjectTrackerTypeDefinitions.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 25.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_CKObjectTrackerTypeDefinitions_h
#define CKObjectTrackerLib_CKObjectTrackerTypeDefinitions_h

#include "ModuleDebugParams.h"

struct TrackerOutput {
    cv::Mat homography;
    bool isObjectPresent;
    bool failed;
};

struct TrackerDebugInfo {
    ModuleDebugParams moduleInfo;
    cv::Mat sceneImage;
};

#endif
