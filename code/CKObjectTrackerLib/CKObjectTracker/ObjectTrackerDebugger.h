//
//  ObjectTrackerDebugger.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 24.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_ObjectTrackerDebugger_h
#define CKObjectTrackerLib_ObjectTrackerDebugger_h

#include "ObjectTrackerDebugInfo.h"

namespace ck {

class ObjectTrackerDebugger {
    
public:
    static std::string debugString(TrackerDebugInfoStripped info);
    
    // only usefull in videos module settings should not have changed during playback
    static std::string debugString(std::vector<TrackerDebugInfoStripped> info);

    static std::vector<std::pair<std::string, cv::Mat> > debugImages(TrackerDebugInfo info, bool drawTransformedRect = true, bool drawFilteredMatches = false, bool drawAllMatches = false, bool drawObjectKeyPoints = false, bool drawSceneKeyPoints = false);
};

} // end of namespace
    
#endif
