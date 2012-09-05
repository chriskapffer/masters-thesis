//
//  ObjectTrackerImpl.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 25.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_ObjectTrackerImpl_h
#define CKObjectTrackerLib_ObjectTrackerImpl_h

#include "ObjectTracker.h"
#include "AbstractModule.h"

namespace ck {
    
class ObjectTracker::Implementation {

private:
    friend class Initializer;
    
    ModuleParams _moduleParams;
    AbstractModule* _currentModule;

    std::map<ModuleType, AbstractModule*> _allModules;
    Settings* _settings;
    
    void initModules(const cv::Mat& objectImage);
    void track(const cv::Mat& frame, TrackerOutput& output, TrackerDebugInfo& debugInfo, bool trackInSequence);
public:
    Implementation();
    ~Implementation();
    
    const inline Settings getSettings() const {
        return *_settings;
    }
    
    void setObject(const cv::Mat& objectImage);
    void trackObjectInVideo(const cv::Mat& frame, TrackerOutput& output, TrackerDebugInfo& debugInfo);
    void trackObjectInStillImage(const cv::Mat& image, std::vector<TrackerOutput>& output, std::vector<TrackerDebugInfo>& debugInfo);
};

} // end of namespace
    
#endif
