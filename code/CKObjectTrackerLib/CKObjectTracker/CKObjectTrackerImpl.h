//
//  CKObjectTrackerImpl.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 25.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_CKObjectTrackerImpl_h
#define CKObjectTrackerLib_CKObjectTrackerImpl_h

#include "CKObjectTracker.h"

#include "AbstractModule.h"

class CKObjectTracker::Impl {

private:
    ModuleInOutParams _moduleParams;
    AbstractModule* _currentModule;

    std::map<ModuleType, AbstractModule*> _allModules;

    void initModules(const cv::Mat& objectImage);
    void track(const cv::Mat& frame, TrackerOutput& output, TrackerDebugInfo& debugInfo, bool trackInSequence);
    
public:
    Impl();
    ~Impl();
    
    void setObject(const cv::Mat& objectImage);
    void trackObjectInVideo(const cv::Mat& frame, TrackerOutput& output, TrackerDebugInfo& debugInfo);
    void trackObjectInStillImage(const cv::Mat& image, std::vector<TrackerOutput>& output, std::vector<TrackerDebugInfo>& debugInfo);
};

#endif
