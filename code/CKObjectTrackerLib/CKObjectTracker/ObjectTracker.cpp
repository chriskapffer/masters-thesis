//
//  ObjectTracker.cpp
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 17.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#include "ObjectTracker.h"
#include "ObjectTrackerImpl.h"

namespace ck {

ObjectTracker::ObjectTracker()
{
    mImpl = new Implementation();
}

ObjectTracker::~ObjectTracker()
{
    delete mImpl;
}

const Settings ObjectTracker::getSettings() const {
    return mImpl->getSettings();
}

void ObjectTracker::setFocalLength(const cv::Point2f& focalLength) {
    mImpl->setFocalLength(focalLength);
}

cv::Point2f ObjectTracker::getFocalLength() const {
    return mImpl->getFocalLength();
}

void ObjectTracker::setPrincipalPoint(const cv::Point2f& principalPoint) {
    mImpl->setPrincipalPoint(principalPoint);
}

cv::Point2f ObjectTracker::getPrincipalPoint() const {
    return mImpl->getPrincipalPoint();
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
