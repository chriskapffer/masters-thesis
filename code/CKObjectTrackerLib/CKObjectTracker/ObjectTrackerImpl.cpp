//
//  ObjectTrackerImpl.cpp
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 25.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#include "ObjectTrackerImpl.h"
#include "ObjectTrackerCreator.h"

#if defined(__has_include)
    #if __has_include(<future>) && __has_feature(cxx_lambdas)
        #define __NECESSARY_CPP11_FEATURES_AVAILABLE__
        #include <future>
    #endif

    #if !defined(__NECESSARY_CPP11_FEATURES_PRESENT__) && __has_include(<dispatch/dispatch.h>)
        #define __NECESSARY_APPLE_FEATURES_AVAILABLE__
        #include <dispatch/dispatch.h>
    #endif
#endif

using namespace std;
using namespace cv;

namespace ck {

ObjectTracker::Implementation::Implementation()
    : _settings("Object Tracker Settings")
{    
    Creator::initializeTracker(*this);
}

ObjectTracker::Implementation::~Implementation()
{
    Creator::finalizeTracker(*this);
}

void ObjectTracker::Implementation::initModules(const cv::Mat& objectImage)
{
    double startTime = (double)cv::getTickCount();
    std::map<ModuleType, AbstractModule*>::iterator it;
    for (it = _allModules.begin(); it != _allModules.end(); it++) {
        (*it).second->initWithObjectImage(objectImage);
    }
    double elapsedTime = ((double)cv::getTickCount() - startTime) / cv::getTickFrequency() * 1000;
    cout << "Initialization took " << elapsedTime << " ms." << endl;
    _moduleParams.successor = MODULE_TYPE_DETECTION;
    _currentModule = _allModules[MODULE_TYPE_DETECTION];
}

void ObjectTracker::Implementation::setObject(const Mat& objectImage)
{
    _moduleParams.successor = MODULE_TYPE_EMPTY;
    _currentModule = _allModules[MODULE_TYPE_EMPTY];

#if defined(__NECESSARY_CPP11_FEATURES_AVAILABLE__)
    // use C++11 features for concurrency
    auto function = [] (CKObjectTracker::Impl& tracker, const cv::Mat &objectImage) {
        tracker.initModules(objectImage);
    };
    async(launch::async, function, ref(*this), objectImage);
#elif defined(__NECESSARY_APPLE_FEATURES_AVAILABLE__)
    // use apple features for concurrency
    static dispatch_queue_t queue;
    // if queue is still running wait until it completes
    if (queue) { dispatch_sync(queue, ^{}); }
    // create a new queue
    queue = dispatch_queue_create("com.ckobjecttracker.setobject", DISPATCH_QUEUE_SERIAL);
    if (!queue) return;
    
    dispatch_async(queue, ^{
        this->initModules(objectImage);
        dispatch_async(dispatch_get_main_queue(), ^{
            dispatch_release(queue);
        });
    });
#else
    // no concurrency, no nothing
    this->initModules(objectImage);
#endif
}

void ObjectTracker::Implementation::trackObjectInStillImage(const Mat& image, vector<TrackerOutput>& output, vector<TrackerDebugInfo>& debugInfo)
{
    TrackerOutput outputItem;
    TrackerDebugInfo debugInfoItem;
    while (!outputItem.isObjectPresent && !outputItem.failed) {
        track(image, outputItem, debugInfoItem, false);
        debugInfo.push_back(debugInfoItem);
        output.push_back(outputItem);
    }
}

void ObjectTracker::Implementation::trackObjectInVideo(const Mat& frame, TrackerOutput& output, TrackerDebugInfo& debugInfo)
{
    track(frame, output, debugInfo, true);
}

void ObjectTracker::Implementation::track(const Mat& frame, TrackerOutput& output, TrackerDebugInfo& debugInfo, bool trackInSequence)
{
    _moduleParams.sceneImageCurrent = frame;
    _currentModule->process(_moduleParams, debugInfo);
    _currentModule = _allModules[_moduleParams.successor];

    // set ouput and debug info
    output.homography = _moduleParams.homography;
    output.isObjectPresent = _moduleParams.isObjectPresent;
    output.failed = _currentModule->getType() == MODULE_TYPE_EMPTY;
}

} // end of namespace
