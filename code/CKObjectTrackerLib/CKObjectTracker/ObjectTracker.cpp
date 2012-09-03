//
//  ObjectTracker.cpp
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 17.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#include "ObjectTrackerImpl.h"
#include "ObjectTracker.h"
#include "Profiler.h"

namespace ck {

ObjectTracker::ObjectTracker()
{
    mImpl = new Implementation();
}

ObjectTracker::~ObjectTracker()
{
    Profiler::Finish();
    delete mImpl;
}

Settings ObjectTracker::settings() const {
    return mImpl->getSettings();
}
    
void ObjectTracker::setObject(const cv::Mat& objectImage) {
    mImpl->setObject(objectImage);
}

void ObjectTracker::trackObjectInVideo(const cv::Mat& frame, TrackerOutput& output, TrackerDebugInfo& debugInfo) {
    mImpl->trackObjectInVideo(frame, output, debugInfo);
}

void ObjectTracker::trackObjectInStillImage(const cv::Mat& image, std::vector<TrackerOutput>& output, std::vector<TrackerDebugInfo>& debugInfo) {
    mImpl->trackObjectInStillImage(image, output, debugInfo);
}

} // end of namespace
