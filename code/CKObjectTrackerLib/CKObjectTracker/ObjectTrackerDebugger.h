//
//  CKObjectTrackerDebugger.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 24.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_CKObjectTrackerDebugger_h
#define CKObjectTrackerLib_CKObjectTrackerDebugger_h

#include "CKObjectTrackerTypeDefinitions.h"

class CKObjectTrackerDebugger {
    
public:
    static std::string debugString(TrackerDebugInfo info);
    static std::vector<cv::Mat> debugImages(TrackerDebugInfo info);
};

#endif
