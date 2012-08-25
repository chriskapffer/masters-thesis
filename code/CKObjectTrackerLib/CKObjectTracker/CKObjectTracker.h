//
//  CKObjectTracker.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 17.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTracker_CKObjectTracker_h
#define CKObjectTracker_CKObjectTracker_h

// http://opencv-users.1802565.n2.nabble.com/BRISK-td7452653.html <-- TODO read
// TODO create a namespace

#include "CKObjectTrackerTypeDefinitions.h"

class CKObjectTracker {

public:
    CKObjectTracker();
    ~CKObjectTracker();
    
    void setObject(const cv::Mat& objectImage);
    void trackObjectInVideo(const cv::Mat& frame, TrackerOutput& output, TrackerDebugInfo& debugInfo);
    void trackObjectInStillImage(const cv::Mat& image, std::vector<TrackerOutput>& output, std::vector<TrackerDebugInfo>& debugInfo);
    
private:
    class Impl;
    Impl* mImpl;
};

#endif
