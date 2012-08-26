//
//  ObjectTracker.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 17.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTracker_ObjectTracker_h
#define CKObjectTracker_ObjectTracker_h

// http://opencv-users.1802565.n2.nabble.com/BRISK-td7452653.html <-- TODO read
// TODO create a namespace

#include "ObjectTrackerTypeDefinitions.h"

namespace ck {

class ObjectTracker {

public:
    ObjectTracker();
    ~ObjectTracker();
    
    void setObject(const cv::Mat& objectImage);
    void trackObjectInVideo(const cv::Mat& frame, TrackerOutput& output, TrackerDebugInfo& debugInfo);
    void trackObjectInStillImage(const cv::Mat& image, std::vector<TrackerOutput>& output, std::vector<TrackerDebugInfo>& debugInfo);
    
private:
    class Impl;
    Impl* mImpl;
};

} // end of namespace

#endif
