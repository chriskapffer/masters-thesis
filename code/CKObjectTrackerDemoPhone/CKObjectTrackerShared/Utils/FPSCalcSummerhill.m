//
//  FPSCalcSummerhill.m
//  CKObjectTrackerShared
//
//  Created by Christoph Kapffer on 14.06.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import "FPSCalcSummerhill.h"

// Number of frames to average for FPS calculation
const int kFrameTimeBufferSize = 5;

@interface FPSCalcSummerhill ()
{
    // Fps calculation
    CMTimeValue _lastFrameTimestamp;
    float *_frameTimes;
    int _frameTimesIndex;
    int _framesToAverage;
    float _captureQueueFps;
}

@end

@implementation FPSCalcSummerhill

- (id)init
{
    self = [super init];
    if (self) {
        // Create frame time circular buffer for calculating averaged fps
        _frameTimes = (float*)malloc(sizeof(float) * kFrameTimeBufferSize);
    }
    return self;
}

- (void)dealloc
{
    if (_frameTimes) {
        free(_frameTimes);
    }
}

// this code is taken from https://github.com/aptogo/FaceTracker
- (void)calculateFramerateAtTimestamp:(CMTime)timestamp
{
    if (_lastFrameTimestamp == 0) {
        _lastFrameTimestamp = timestamp.value;
        _framesToAverage = 1;
    }
    else {
        float frameTime = (float)(timestamp.value - _lastFrameTimestamp) / timestamp.timescale;
        _lastFrameTimestamp = timestamp.value;
        
        _frameTimes[_frameTimesIndex++] = frameTime;
        
        if (_frameTimesIndex >= kFrameTimeBufferSize) {
            _frameTimesIndex = 0;
        }
        
        float totalFrameTime = 0.0f;
        for (int i = 0; i < _framesToAverage; i++) {
            totalFrameTime += _frameTimes[i];
        }
        
        float averageFrameTime = totalFrameTime / _framesToAverage;
        float fps = 1.0f / averageFrameTime;
        
        if (fabsf(fps - _captureQueueFps) > 0.1f) {
            _captureQueueFps = fps;
            dispatch_sync(dispatch_get_main_queue(), ^{
                [self setFramesPerSecond:fps];
            });    
        }
        
        _framesToAverage++;
        if (_framesToAverage > kFrameTimeBufferSize) {
            _framesToAverage = kFrameTimeBufferSize;
        }
    }
}

@end
