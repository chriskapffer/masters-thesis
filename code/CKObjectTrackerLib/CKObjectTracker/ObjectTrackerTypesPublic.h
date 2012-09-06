//
//  ObjectTrackerTypesPublic.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 06.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_ObjectTrackerTypesPublic_h
#define CKObjectTrackerLib_ObjectTrackerTypesPublic_h

#include "SettingsPublicScope.h"
#include "ObjectTrackerDebugInfo.h"

namespace ck {
    
    struct TrackerOutput {
        cv::Mat homography;
        bool isObjectPresent;
        bool failed;
    };
    
} // end of namespace

#endif
