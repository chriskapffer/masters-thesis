//
//  ObjectTrackerLibrary.mm
//  CKObjectTrackerShared
//
//  Created by Christoph Kapffer on 07.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import "ObjectTrackerLibrary.h"
#import "ObjectTrackerDebugger.h"
#import "ObjectTracker.h"

#import "CVImageConverter+PixelBuffer.h"

using namespace ck;
using namespace cv;

@interface ObjectTrackerLibrary ()
{
    Mat _objectImage;
    ObjectTracker* _tracker;

    TrackerOutput _output;
    TrackerDebugInfo _frameDebugInfo;
    vector<TrackerDebugInfoStripped> _videoDebugInfo;
}

@property (nonatomic, assign) dispatch_queue_t stillImageTrackerQueue;

- (void)handleTrackingInVideoResult;
- (void)handleTrackingInImageResult;
- (Homography)homographyWithMatrix:(Mat&)matrix;
- (void)showError:(NSError*)error;

@end

@implementation ObjectTrackerLibrary

#pragma mark - properties

@synthesize delegate = _delegate;
@synthesize stillImageTrackerQueue = _stillImageTrackerQueue;
@synthesize recordDebugInfo = _recordDebugInfo;

#pragma mark - initialization

+ (id)instance
{
    static dispatch_once_t pred = 0;
    __strong static id _sharedObject = nil;
    dispatch_once(&pred, ^{
        _sharedObject = [[self alloc] init];
    });
    return _sharedObject;
}

- (id)init
{
    self = [super init];
    if (self) {
        _objectImage = Mat();
        _tracker = new ObjectTracker();
        _videoDebugInfo = vector<TrackerDebugInfoStripped>();
        _stillImageTrackerQueue = dispatch_queue_create("ck.objecttracker.trackerlibrary.stillimage", DISPATCH_QUEUE_SERIAL);
        _recordDebugInfo = YES;
    }
    return self;
}

- (void)dealloc
{
    delete _tracker;
    _tracker = 0;
    
    dispatch_release(_stillImageTrackerQueue);
}

#pragma mark - tracking methods

- (UIImage*)objectImage
{
    UIImage* image;
    NSError* error = NULL;
    image = [CVImageConverter UIImageFromCVMat:_objectImage error:&error];
    if (error != NULL) {
        [self showError:error];
    }
    return image;
}

- (UIImage*)objectHistogram
{
    UIImage* histogram;
//    NSError* error = NULL;
//    Mat hist = ObjectTrackerDebugger::getHistogramImage(_frameDebugInfo.objectHistogram);
//    histogram = [CVImageConverter UIImageFromCVMat:_objectImage error:&error];
//    if (error != NULL) {
//        [self showError:error];
//    }
    return histogram;
}

- (void)setObjectImageWithImage:(UIImage *)objectImage
{
    NSError* error = NULL;
    UIImage* test = objectImage;
    Mat test2;
    [CVImageConverter CVMat:test2 FromUIImage:test error:&error];
    if (error == NULL) {
        _tracker->setObject(test2);
    } else {
        [self showError:error];
    }
}

- (void)setObjectImageWithBuffer:(CVPixelBufferRef)objectImage
{
    NSError* error = NULL;
    [CVImageConverter CVMat:_objectImage FromCVPixelBuffer:objectImage error:&error];
    if (error == NULL) {
        _tracker->setObject(_objectImage);
    } else {
        [self showError:error];
    }
}

- (Homography)homography
{
    return [self homographyWithMatrix:_output.homography];
}
- (BOOL)foundObject
{
    return _output.isObjectPresent;
}

- (void)trackObjectInImageWithImage:(UIImage*)image
{
    __block UIImage* retainedImage = [image copy];
    dispatch_async(self.stillImageTrackerQueue, ^{
        Mat frame;
        NSError* error = NULL;
        [CVImageConverter CVMat:frame FromUIImage:retainedImage error:&error];
        if (error == NULL) {
            vector<TrackerOutput> output;
            vector<TrackerDebugInfo> debugInfo;
            _tracker->trackObjectInStillImage(frame, output, debugInfo);
            _frameDebugInfo = *(debugInfo.end() - 1);
            _output = *(output.end() - 1);
            [self handleTrackingInImageResult];
        } else {
            [self showError:error];
        }
    });
}

- (void)trackObjectInVideoWithImage:(UIImage*)image
{
    Mat frame;
    NSError* error = NULL;
    [CVImageConverter CVMat:frame FromUIImage:image error:&error];
    if (error == NULL) {
        _tracker->trackObjectInVideo(frame, _output, _frameDebugInfo);
        [self handleTrackingInVideoResult];
    } else {
        [self showError:error];
    }
}

- (void)trackObjectInVideoWithBuffer:(CVPixelBufferRef)buffer
{
    Mat frame;
    NSError* error = NULL;
    [CVImageConverter CVMat:frame FromCVPixelBuffer:buffer error:&error];
    if (error == NULL) {
        _tracker->trackObjectInVideo(frame, _output, _frameDebugInfo);
        [self handleTrackingInVideoResult];
    } else {
        [self showError:error];
    }
}

#pragma mark - debug methods

- (NSString*)frameDebugInfoString
{
    string result = ObjectTrackerDebugger::getDebugString(TrackerDebugInfoStripped(_frameDebugInfo));
    return [NSString stringWithUTF8String:result.c_str()];
}
- (NSString*)videoDebugInfoString
{
    string result = ObjectTrackerDebugger::getDebugString(_videoDebugInfo);
    return [NSString stringWithUTF8String:result.c_str()];
}

- (void)clearVideoDebugInfo
{
    _videoDebugInfo.clear();
}

- (BOOL)detectionDebugImage:(UIImage**)image WithSearchWindow:(BOOL)searchWindow
{
    Mat matrix;
    if (ObjectTrackerDebugger::getDetectionModuleDebugImage(matrix, _frameDebugInfo, searchWindow)) {
        NSError* error = NULL;
        *image = [CVImageConverter UIImageFromCVMat:matrix error:&error];
        if (error == NULL) {
            return YES;
        }
        [self showError:error];
    }
    return NO;
}

- (BOOL)validationDebugImage:(UIImage**)image WithObjectRect:(BOOL)objectRect ObjectKeyPoints:(BOOL)objectKeyPoints SceneKeyPoints:(BOOL)sceneKeyPoints FilteredMatches:(BOOL)filteredMatches AllMatches:(BOOL)allmatches
{
    Mat matrix;
    if (ObjectTrackerDebugger::getValidationModuleDebugImage(matrix, _frameDebugInfo, objectRect, objectKeyPoints, sceneKeyPoints, filteredMatches, allmatches)) {
        NSError* error = NULL;
        *image = [CVImageConverter UIImageFromCVMat:matrix error:&error];
        if (error == NULL) {
            return YES;
        }
        [self showError:error];
    }
    return NO;
}

- (BOOL)trackingDebugImage:(UIImage**)image WithObjectRect:(BOOL)objectRect FilteredPoints:(BOOL)filteredPoints AllPoints:(BOOL)allPoints SearchWindow:(BOOL)searchWindow
{
    Mat matrix;
    if (ObjectTrackerDebugger::getTrackingModuleDebugImage(matrix, _frameDebugInfo, objectRect, filteredPoints, allPoints, searchWindow)) {
        NSError* error = NULL;
        *image = [CVImageConverter UIImageFromCVMat:matrix error:&error];
        if (error == NULL) {
            return YES;
        }
        [self showError:error];
    }
    return NO;
}

#pragma mark - helper methods
     
- (void)handleTrackingInVideoResult
{
    if (self.recordDebugInfo) {
        _videoDebugInfo.push_back(TrackerDebugInfoStripped(_frameDebugInfo));
        if (_videoDebugInfo.size() >= MAX_RECORDED_FRAMES) {
            if ([self.delegate respondsToSelector:@selector(reachedDebugInfoRecordingLimit:)])
                [self.delegate reachedDebugInfoRecordingLimit:[self videoDebugInfoString]];
            _videoDebugInfo.clear();
        }
    }
    if (_output.isObjectPresent) {
        if ([self.delegate respondsToSelector:@selector(trackedObjectWithHomography:)]) {
            [self.delegate trackedObjectWithHomography:[self homography]];
        }
    }
    if ([self.delegate respondsToSelector:@selector(didProcessFrame)]) {
        [self.delegate didProcessFrame];
    }
}

- (void)handleTrackingInImageResult
{
    if (_output.isObjectPresent) {
        if ([self.delegate respondsToSelector:@selector(trackedObjectWithHomography:)]) {
            [self.delegate trackedObjectWithHomography:[self homography]];
        }
    } else {
        if ([self.delegate respondsToSelector:@selector(failedToTrackObjectInImage)]) {
            [self.delegate failedToTrackObjectInImage];
        }
    }
    if ([self.delegate respondsToSelector:@selector(didProcessFrame)]) {
        [self.delegate didProcessFrame];
    }
}

- (Homography)homographyWithMatrix:(Mat&)matrix
{
    Homography result;
    result.m11 = matrix.at<double>(0,0);
    result.m12 = matrix.at<double>(0,1);
    result.m13 = matrix.at<double>(0,2);
    result.m21 = matrix.at<double>(1,0);
    result.m22 = matrix.at<double>(1,1);
    result.m23 = matrix.at<double>(1,2);
    result.m31 = matrix.at<double>(2,0);
    result.m32 = matrix.at<double>(2,1);
    result.m33 = matrix.at<double>(2,2);
    return result;
}

- (void)showError:(NSError*)error
{
    UIAlertView* alertView = [[UIAlertView alloc] initWithTitle:error.domain message:error.description delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
    [alertView show];
    NSLog(@"\n%@\n%@", error.domain, error.description);
}

@end
