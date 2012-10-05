//
//  ModuleParams.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 25.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_ModuleParams_h
#define CKObjectTrackerLib_ModuleParams_h

#include "TransformationBuilder.h"
#include "ModuleTypes.h"

namespace ck {
    
    struct ModuleParams {
        ModuleType successor;            // OUT: abstract             IN: abstract
        cv::Mat sceneImageCurrent;       // OUT: -                    IN: detection, validation, tracking
        cv::Rect searchRect;             // OUT:  detection, tracking IN: detection, validation
        bool isObjectPresent;            // OUT: validation, tracking IN: -
        Transform objectInfo;            // OUT: validation, tracking IN: -
        cv::Mat homography;              // OUT: validation, tracking IN: tracking
        cv::Mat sceneImagePrevious;      // OUT: validation, tracking IN: tracking
        std::vector<cv::Point2f> points; // OUT: validation, tracking IN: tracking
        std::vector<cv::Point2f> previosTransformedCorners; // OUT: tracking IN: tracking
        
        ModuleParams() : successor(MODULE_TYPE_EMPTY), homography(cv::Mat()) {};
    };
    
} // end of namespace

#endif
