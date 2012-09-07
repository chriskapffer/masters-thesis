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

- (Homography)homographyWithMatrix:(Mat&)matrix;
- (void)showError:(NSError*)error;

@end

@implementation ObjectTrackerLibrary

#pragma mark - properties

@synthesize delegate = _delegate;
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
        _recordDebugInfo = YES;
    }
    return self;
}

- (void)dealloc
{
    delete _tracker;
    _tracker = 0;
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
    [CVImageConverter CVMat:_objectImage FromUIImage:objectImage error:&error];
    if (error == NULL) {
        _tracker->setObject(_objectImage);
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
    Mat frame;
    NSError* error = NULL;
    [CVImageConverter CVMat:frame FromUIImage:image error:&error];
    if (error == NULL) {
        vector<TrackerOutput> output;
        vector<TrackerDebugInfo> debugInfo;
        _tracker->trackObjectInStillImage(frame, output, debugInfo);
        _frameDebugInfo = *(debugInfo.end() - 1);
        _output = *(output.end() - 1);
    } else {
        [self showError:error];
    }
}

- (void)trackObjectInVideoWithImage:(UIImage*)image
{
    Mat frame;
    NSError* error = NULL;
    [CVImageConverter CVMat:frame FromUIImage:image error:&error];
    if (error == NULL) {
        _tracker->trackObjectInVideo(frame, _output, _frameDebugInfo);
        if (self.recordDebugInfo) {
            _videoDebugInfo.push_back(TrackerDebugInfoStripped(_frameDebugInfo));
        }
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
        if (self.recordDebugInfo) {
            _videoDebugInfo.push_back(TrackerDebugInfoStripped(_frameDebugInfo));
            // TODO: if size > MAX_REC_FRAMES 
        }
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

- (UIImage*)detectionDebugImageWithSearchWindow:(BOOL)searchWindow
{
    Mat matrix;
    UIImage* image;
    NSError* error = NULL;
    ObjectTrackerDebugger::getDetectionModuleDebugImage(matrix, _frameDebugInfo, searchWindow);
    image = [CVImageConverter UIImageFromCVMat:matrix error:&error];
    if (error != NULL) {
        [self showError:error];
    }
    return image;
}

- (UIImage*)validationDebugImageWithObjectRect:(BOOL)objectRect ObjectKeyPoints:(BOOL)objectKeyPoints SceneKeyPoints:(BOOL)sceneKeyPoints FilteredMatches:(BOOL)filteredMatches AllMatches:(BOOL)allmatches
{
    Mat matrix;
    UIImage* image;
    NSError* error = NULL;
    ObjectTrackerDebugger::getValidationModuleDebugImage(matrix, _frameDebugInfo, objectRect, objectKeyPoints, sceneKeyPoints, filteredMatches, allmatches);
    image = [CVImageConverter UIImageFromCVMat:matrix error:&error];
    if (error != NULL) {
        [self showError:error];
    }
    return image;
}

- (UIImage*)trackingDebugImageWithObjectRect:(BOOL)objectRect FilteredPoints:(BOOL)filteredPoints AllPoints:(BOOL)allPoints SearchWindow:(BOOL)searchWindow
{
    Mat matrix;
    UIImage* image;
    NSError* error = NULL;
    ObjectTrackerDebugger::getTrackingModuleDebugImage(matrix, _frameDebugInfo, objectRect, filteredPoints, allPoints, searchWindow);
    image = [CVImageConverter UIImageFromCVMat:matrix error:&error];
    if (error != NULL) {
        [self showError:error];
    }
    return image;
}

#pragma mark - helper methods
     
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
