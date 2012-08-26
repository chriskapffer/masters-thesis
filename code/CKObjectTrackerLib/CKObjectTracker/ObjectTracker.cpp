//
//  CKObjectTracker.cpp
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 17.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#include "CKObjectTrackerImpl.h"
#include "CKObjectTracker.h"

CKObjectTracker::CKObjectTracker()
{
    mImpl = new Impl();
}

CKObjectTracker::~CKObjectTracker()
{
    delete mImpl;
}

void CKObjectTracker::setObject(const cv::Mat& objectImage) {
    mImpl->setObject(objectImage);
}

void CKObjectTracker::trackObjectInVideo(const cv::Mat& frame, TrackerOutput& output, TrackerDebugInfo& debugInfo) {
    mImpl->trackObjectInVideo(frame, output, debugInfo);
}

void CKObjectTracker::trackObjectInStillImage(const cv::Mat& image, std::vector<TrackerOutput>& output, std::vector<TrackerDebugInfo>& debugInfo) {
    mImpl->trackObjectInStillImage(image, output, debugInfo);
}