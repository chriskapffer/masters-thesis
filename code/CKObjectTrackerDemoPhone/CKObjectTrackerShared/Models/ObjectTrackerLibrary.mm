//
//  ObjectTrackerLibrary.mm
//  CKObjectTrackerShared
//
//  Created by Christoph Kapffer on 07.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import "ObjectTrackerLibrary.h" // Obj-C
#import "ObjectTrackerParameter.h" // Obj-C
#import "ObjectTrackerDebugger.h" // C++
#import "ObjectTracker.h" // C++

#import "CVImageConverter+PixelBuffer.h"

using namespace ck;
using namespace cv;

@interface ObjectTrackerLibrary ()
{
    Mat _objectImage;
    ObjectTracker* _tracker;

    vector<TrackerDebugInfoStripped> _videoDebugInfo;
    
    NSLock* _outputDataLock;
    NSLock* _debugDataLock;
}

@property (nonatomic, assign) dispatch_queue_t trackStillImageQueue;
@property (nonatomic, assign) dispatch_queue_t setObjectImageQueue;
@property (nonatomic, assign) TrackerDebugInfo frameDebugInfo;
@property (nonatomic, assign) TrackerOutput trackerOutput;

- (void)handleTrackingInVideoResult;
- (void)handleTrackingInImageResult;
- (void)showError:(NSError*)error;

@end

@implementation ObjectTrackerLibrary

#pragma mark - properties

@synthesize delegate = _delegate;
@synthesize recordDebugInfo = _recordDebugInfo;
@synthesize trackStillImageQueue = _trackStillImageQueue;
@synthesize setObjectImageQueue = _setObjectImageQueue;
@synthesize frameDebugInfo = _frameDebugInfo;
@synthesize trackerOutput = _trackerOutput;

- (void)setFrameDebugInfo:(ck::TrackerDebugInfo)frameDebugInfo
{
    [_debugDataLock lock];
    _frameDebugInfo = frameDebugInfo;
    [_debugDataLock unlock];
}

- (TrackerDebugInfo)frameDebugInfo
{
    TrackerDebugInfo copy;
    [_debugDataLock lock];
    copy = TrackerDebugInfo(_frameDebugInfo);
    [_debugDataLock unlock];
    return copy;
}

- (void)setTrackerOutput:(ck::TrackerOutput)trackerOutput
{
    [_outputDataLock lock];
    _trackerOutput = trackerOutput;
    [_outputDataLock unlock];
}

- (TrackerOutput)trackerOutput
{
    TrackerOutput copy;
    [_outputDataLock lock];
    copy = TrackerOutput(_trackerOutput);
    [_outputDataLock unlock];
    return copy;
}

- (ObjectTrackerParameterCollection*)parameters
{
    return [self parameterCollectionFromSettings:_tracker->getSettings()];
}

- (BOOL)foundObject
{
    return self.trackerOutput.isObjectPresent;
}

- (CGFloat)objectScale
{
    return self.trackerOutput.objectInfo.scale;
}

- (CGPoint)objectTranslation
{
    Point2f position = self.trackerOutput.objectInfo.translation;
    return CGPointMake(position.x, position.y);
}

- (Matrix3x3)objectRotation
{
    Mat rotation = self.trackerOutput.objectInfo.rotation;
    Matrix3x3 result;
    result.m00 = rotation.at<double>(0,0);
    result.m10 = rotation.at<double>(1,0);
    result.m20 = rotation.at<double>(2,0);
    result.m01 = rotation.at<double>(0,1);
    result.m11 = rotation.at<double>(1,1);
    result.m21 = rotation.at<double>(2,1);
    result.m02 = rotation.at<double>(0,2);
    result.m12 = rotation.at<double>(1,2);
    result.m22 = rotation.at<double>(2,2);
    return result;
}

- (NSString*)frameDebugInfoString
{
    string result = ObjectTrackerDebugger::getDebugString(TrackerDebugInfoStripped(self.frameDebugInfo));
    return [NSString stringWithUTF8String:result.c_str()];
}
- (NSString*)videoDebugInfoString
{
    string result = ObjectTrackerDebugger::getDebugString(_videoDebugInfo);
    return [NSString stringWithUTF8String:result.c_str()];
}

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
        _trackStillImageQueue = dispatch_queue_create("ck.objecttracker.trackerlibrary.trackstillimage", DISPATCH_QUEUE_SERIAL);
        _setObjectImageQueue = dispatch_queue_create("ck.objecttracker.trackerlibrary.setobjectimage", DISPATCH_QUEUE_SERIAL);
        _outputDataLock = [[NSLock alloc] init];
        _debugDataLock = [[NSLock alloc] init];
        _recordDebugInfo = YES;
    }
    return self;
}

- (void)dealloc
{
    delete _tracker;
    _tracker = 0;
    
    dispatch_release(_trackStillImageQueue);
    dispatch_release(_setObjectImageQueue);
    _outputDataLock = nil;
    _debugDataLock = nil;
}

- (void)setFocalLength:(CGPoint)focalLength {
    _tracker->setFocalLength(Point2f(focalLength.x, focalLength.y));
}

- (void)setPrincipalPoint:(CGPoint)principalPoint {
    _tracker->setPrincipalPoint(Point2f(principalPoint.x, principalPoint.y));
}

#pragma mark - object related methods

- (UIImage*)objectImage
{
    if (_objectImage.empty()) { return nil; }
    
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
//    Mat hist = ObjectTrackerDebugger::getHistogramImage(self.frameDebugInfo.objectHistogram);
//    histogram = [CVImageConverter UIImageFromCVMat:_objectImage error:&error];
//    if (error != NULL) {
//        [self showError:error];
//    }
    return histogram;
}

- (void)setObjectImageAsync
{
    if (_objectImage.empty()) { return; }
    
    static int queueCnt = 0;
    queueCnt++;
    dispatch_async(self.setObjectImageQueue, ^{
        if (queueCnt > 1) {
            queueCnt--;
            return;
        }
        _tracker->setObject(_objectImage);
        queueCnt--;
        dispatch_async(dispatch_get_main_queue(), ^{
            if ([self.delegate respondsToSelector:@selector(finishedObjectImageInitialization)]) {
                [self.delegate finishedObjectImageInitialization];
            }
        });
    });
}

- (void)setObjectImageWithImage:(UIImage *)objectImage
{
    if (objectImage == nil) { return; }
    
    Mat tmpImage;
    NSError* error = NULL;
    [CVImageConverter CVMat:tmpImage FromUIImage:objectImage error:&error];
    if (error == NULL) {
        tmpImage.copyTo(_objectImage);
        [self setObjectImageAsync];
    } else {
        [self showError:error];
    }
}

- (void)setObjectImageWithBuffer:(CVPixelBufferRef)objectImage
{
    Mat tmpImage;
    NSError* error = NULL;
    [CVImageConverter CVMat:tmpImage FromCVPixelBuffer:objectImage error:&error];
    if (error == NULL) {
        tmpImage.copyTo(_objectImage);
        [self setObjectImageAsync];
    } else {
        [self showError:error];
    }
}

- (void)reInitializeObjectImage
{
    [self setObjectImageAsync];
}

#pragma mark - parameter related methods

- (void)setBoolParameterWithName:(NSString*)name Value:(BOOL)value
{
    _tracker->getSettings().setBoolValue([name UTF8String], value);
}

- (void)setintParameterWithName:(NSString*)name Value:(int)value
{
    _tracker->getSettings().setIntValue([name UTF8String], value);
}

- (void)setFloatParameterWithName:(NSString*)name Value:(float)value
{
    _tracker->getSettings().setFloatValue([name UTF8String], value);
}

- (void)setStringParameterWithName:(NSString*)name Value:(NSString*)value
{
    _tracker->getSettings().setStringValue([name UTF8String], [value UTF8String]);
}

#pragma mark - tracking related methods

- (void)trackObjectInImageWithImage:(UIImage*)image
{
    __block UIImage* retainedImage = [image copy];
    dispatch_async(self.trackStillImageQueue, ^{
        Mat frame;
        NSError* error = NULL;
        [CVImageConverter CVMat:frame FromUIImage:retainedImage error:&error];
        if (error == NULL) {
            vector<TrackerOutput> output;
            vector<TrackerDebugInfo> debugInfo;
            _tracker->trackObjectInStillImage(frame, output, debugInfo);
            self.frameDebugInfo = *(debugInfo.end() - 1);
            self.trackerOutput = *(output.end() - 1);
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
        TrackerOutput output = self.trackerOutput;
        TrackerDebugInfo debugInfo = self.frameDebugInfo;
        _tracker->trackObjectInVideo(frame, output, debugInfo);
        self.frameDebugInfo = debugInfo;
        self.trackerOutput = output;
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
        TrackerOutput output = self.trackerOutput;
        TrackerDebugInfo debugInfo = self.frameDebugInfo;
        _tracker->trackObjectInVideo(frame, output, debugInfo);
        self.frameDebugInfo = debugInfo;
        self.trackerOutput = output;
        [self handleTrackingInVideoResult];
    } else {
        [self showError:error];
    }
}

#pragma mark - debug methods

- (void)clearVideoDebugInfo
{
    _videoDebugInfo.clear();
}

- (BOOL)detectionDebugImage:(UIImage**)image WithSearchWindow:(BOOL)searchWindow
{
    Mat matrix;
    if (ObjectTrackerDebugger::getDetectionModuleDebugImage(matrix, self.frameDebugInfo, searchWindow)) {
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
    if (ObjectTrackerDebugger::getValidationModuleDebugImage(matrix, self.frameDebugInfo, objectRect, objectKeyPoints, sceneKeyPoints, filteredMatches, allmatches)) {
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
    if (ObjectTrackerDebugger::getTrackingModuleDebugImage(matrix, self.frameDebugInfo, objectRect, filteredPoints, allPoints, searchWindow)) {
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
        _videoDebugInfo.push_back(TrackerDebugInfoStripped(self.frameDebugInfo));
        if (_videoDebugInfo.size() >= MAX_RECORDED_FRAMES) {
            if ([self.delegate respondsToSelector:@selector(reachedDebugInfoRecordingLimit:)])
                [self.delegate reachedDebugInfoRecordingLimit:[self videoDebugInfoString]];
            _videoDebugInfo.clear();
        }
    }
    if (self.trackerOutput.isObjectPresent) {
        if ([self.delegate respondsToSelector:@selector(trackedObjectSuccessfully)]) {
            [self.delegate trackedObjectSuccessfully];
        }
    }
    if ([self.delegate respondsToSelector:@selector(trackerLibraryDidProcessFrame)]) {
        [self.delegate trackerLibraryDidProcessFrame];
    }
}

- (void)handleTrackingInImageResult
{
    if (self.trackerOutput.isObjectPresent) {
        if ([self.delegate respondsToSelector:@selector(trackedObjectSuccessfully)]) {
            [self.delegate trackedObjectSuccessfully];
        }
    } else {
        if ([self.delegate respondsToSelector:@selector(failedToTrackObjectInImage)]) {
            [self.delegate failedToTrackObjectInImage];
        }
    }
    if ([self.delegate respondsToSelector:@selector(trackerLibraryDidProcessFrame)]) {
        [self.delegate trackerLibraryDidProcessFrame];
    }
}

- (void)showError:(NSError*)error
{
    UIAlertView* alertView = [[UIAlertView alloc] initWithTitle:error.domain message:error.description delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
    [alertView show];
    NSLog(@"\n%@\n%@", error.domain, error.description);
}

- (ObjectTrackerParameter*)boolParameterWithName:(string)name FromSettings:(Settings)settings
{
    bool critical;
    bool value;
    settings.getBoolValue(name, value);
    settings.getBoolInfo(name, critical);

    ObjectTrackerParameter* parameter = [[ObjectTrackerParameter alloc] init];
    [parameter setName:[NSString stringWithUTF8String:name.c_str()]];
    [parameter setType:ObjectTrackerParameterTypeBool];
    [parameter setBoolValue:value];
    [parameter setCritical:critical];
    return parameter;
}

- (ObjectTrackerParameter*)intParameterWithName:(string)name FromSettings:(Settings)settings
{
    bool critical;
    int value, min, max;
    vector<int> values;
    settings.getIntValue(name, value);
    settings.getIntInfo(name, min, max, values, critical);
    NSMutableArray* intValues = [NSMutableArray arrayWithCapacity:values.size()];
    for (int i = 0; i < values.size(); i++) {
        [intValues addObject:[NSNumber numberWithInt:values[i]]];
    }
    
    ObjectTrackerParameter* parameter = [[ObjectTrackerParameter alloc] init];
    [parameter setName:[NSString stringWithUTF8String:name.c_str()]];
    [parameter setType:ObjectTrackerParameterTypeInt];
    [parameter setIntValue:value];
    [parameter setIntMax:max];
    [parameter setIntMin:min];
    [parameter setIntValues:intValues];
    [parameter setCritical:critical];
    return parameter;
}

- (ObjectTrackerParameter*)floatParameterWithName:(string)name FromSettings:(Settings)settings
{
    bool critical;
    float value, min, max;
    settings.getFloatValue(name, value);
    settings.getFloatInfo(name, min, max, critical);

    ObjectTrackerParameter* parameter = [[ObjectTrackerParameter alloc] init];
    [parameter setName:[NSString stringWithUTF8String:name.c_str()]];
    [parameter setType:ObjectTrackerParameterTypeFloat];
    [parameter setFloatValue:value];
    [parameter setFloatMax:max];
    [parameter setFloatMin:min];
    [parameter setCritical:critical];
    return parameter;
}

- (ObjectTrackerParameter*)stringParameterWithName:(string)name FromSettings:(Settings)settings
{
    bool critical;
    string value;
    vector<string> values;
    settings.getStringValue(name, value);
    settings.getStringInfo(name, values, critical);
    NSMutableArray* stringValues = [NSMutableArray arrayWithCapacity:values.size()];
    for (int i = 0; i < values.size(); i++) {
        [stringValues addObject:[NSString stringWithUTF8String:values[i].c_str()]];
    }
    
    ObjectTrackerParameter* parameter = [[ObjectTrackerParameter alloc] init];
    [parameter setName:[NSString stringWithUTF8String:name.c_str()]];
    [parameter setType:ObjectTrackerParameterTypeString];
    [parameter setStringValue:[NSString stringWithUTF8String:value.c_str()]];
    [parameter setStringValues:stringValues];
    [parameter setCritical:critical];
    return parameter;
}

- (ObjectTrackerParameter*)parameterWithName:(string)name FromSettings:(Settings)settings
{
    ObjectTrackerParameter* parameter = nil;
    Type type; settings.getParameterType(name, type);
    switch (type) {
        case ck::CK_TYPE_BOOL:
            parameter = [self boolParameterWithName:name FromSettings:settings];
            break;
        case ck::CK_TYPE_INT:
            parameter = [self intParameterWithName:name FromSettings:settings];
            break;
        case ck::CK_TYPE_FLOAT:
            parameter = [self floatParameterWithName:name FromSettings:settings];
            break;
        case ck::CK_TYPE_STRING:
            parameter = [self stringParameterWithName:name FromSettings:settings];
            break;
    }
    return parameter;
}

- (ObjectTrackerParameterCollection*) parameterCollectionFromSettings:(Settings)settings
{
    vector<string> parameterNames = settings.getParameterNames();
    NSMutableArray* parameters = [NSMutableArray arrayWithCapacity:parameterNames.size()];
    for (int i = 0; i < parameterNames.size(); i++) {
        [parameters addObject:[self parameterWithName:parameterNames[i] FromSettings:settings]];
    }
    
    vector<Settings> subCategories = settings.getSubCategories();
    NSMutableArray* subCollections = [NSMutableArray arrayWithCapacity:subCategories.size()];
    for (int i = 0; i < subCategories.size(); i++) {
        [subCollections addObject:[self parameterCollectionFromSettings:subCategories[i]]];
    }
    
    ObjectTrackerParameterCollection* collection = [[ObjectTrackerParameterCollection alloc] init];
    [collection setName:[NSString stringWithUTF8String:settings.getName().c_str()]];
    [collection setSubCollections:subCollections];
    [collection setParameters:parameters];
    return collection;
}

@end
