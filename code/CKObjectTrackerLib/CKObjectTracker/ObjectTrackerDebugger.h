//
//  ObjectTrackerDebugger.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 24.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_ObjectTrackerDebugger_h
#define CKObjectTrackerLib_ObjectTrackerDebugger_h

#include "ObjectTrackerTypeDefinitions.h"

namespace ck {

class ObjectTrackerDebugger {
    
public:
    static std::string debugString(TrackerDebugInfo info);
    static std::vector<cv::Mat> debugImages(TrackerDebugInfo info);
};

} // end of namespace
    
#endif
