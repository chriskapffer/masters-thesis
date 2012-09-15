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

static inline double atD(const Mat& m, int row, int col)
{
    return m.at<double>(row, col);
}

@interface ObjectTrackerLibrary ()
{
    Mat _objectImage;
    ObjectTracker* _tracker;

    TrackerOutput _output;
    TrackerDebugInfo _frameDebugInfo;
    vector<TrackerDebugInfoStripped> _videoDebugInfo;
}

@property (nonatomic, assign) dispatch_queue_t trackStillImageQueue;
@property (nonatomic, assign) dispatch_queue_t setObjectImageQueue;

- (void)handleTrackingInVideoResult;
- (void)handleTrackingInImageResult;
- (Matrix3x3)homographyWithMatrix:(const Mat&)matrix;
- (Matrix4x4)modelViewWithHomography:(const Mat&)homography;
- (void)showError:(NSError*)error;

@end

@implementation ObjectTrackerLibrary

#pragma mark - properties

@synthesize delegate = _delegate;
@synthesize recordDebugInfo = _recordDebugInfo;
@synthesize trackStillImageQueue = _trackStillImageQueue;
@synthesize setObjectImageQueue = _setObjectImageQueue;

- (ObjectTrackerParameterCollection*) parameters
{
    return [self parameterCollectionFromSettings:_tracker->getSettings()];
}

- (Matrix3x3)homography
{
    return [self homographyWithMatrix:_output.homography];
}

- (Matrix4x4)modelView
{
    return [self modelViewWithHomography:_output.homography];
}

- (BOOL)foundObject
{
    return _output.isObjectPresent;
}

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
//    Mat hist = ObjectTrackerDebugger::getHistogramImage(_frameDebugInfo.objectHistogram);
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
        TrackerOutput output = _output;
        TrackerDebugInfo debugInfo = _frameDebugInfo;
        _tracker->trackObjectInVideo(frame, output, debugInfo);
        _frameDebugInfo = debugInfo;
        _output = output;
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
        TrackerOutput output = _output;
        TrackerDebugInfo debugInfo = _frameDebugInfo;
        _tracker->trackObjectInVideo(frame, output, debugInfo);
        _frameDebugInfo = debugInfo;
        _output = output;
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
    if ([self.delegate respondsToSelector:@selector(trackerLibraryDidProcessFrame)]) {
        [self.delegate trackerLibraryDidProcessFrame];
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
    if ([self.delegate respondsToSelector:@selector(trackerLibraryDidProcessFrame)]) {
        [self.delegate trackerLibraryDidProcessFrame];
    }
}

- (Matrix3x3)homographyWithMatrix:(const Mat&)matrix
{
    Matrix3x3 result;
    result.m00 = atD(matrix, 0, 0);
    result.m01 = atD(matrix, 0, 1);
    result.m02 = atD(matrix, 0, 2);
    result.m10 = atD(matrix, 1, 0);
    result.m11 = atD(matrix, 1, 1);
    result.m12 = atD(matrix, 1, 2);
    result.m20 = atD(matrix, 2, 0);
    result.m21 = atD(matrix, 2, 1);
    result.m22 = atD(matrix, 2, 2);
    return result;
}

- (Matrix4x4)modelViewWithHomography:(const Mat&)homography
{
    Mat modelView = [self modelviewMatrixFromHomography:homography];
    Matrix4x4 result;
    result.m00 = atD(modelView, 0, 0);
    result.m01 = atD(modelView, 0, 1);
    result.m02 = atD(modelView, 0, 2);
    result.m03 = atD(modelView, 0, 3);
    result.m10 = atD(modelView, 1, 0);
    result.m11 = atD(modelView, 1, 1);
    result.m12 = atD(modelView, 1, 2);
    result.m13 = atD(modelView, 1, 3);
    result.m20 = atD(modelView, 2, 0);
    result.m21 = atD(modelView, 2, 1);
    result.m22 = atD(modelView, 2, 2);
    result.m23 = atD(modelView, 2, 3);
    result.m30 = atD(modelView, 3, 0);
    result.m31 = atD(modelView, 3, 1);
    result.m32 = atD(modelView, 3, 2);
    result.m33 = atD(modelView, 3, 3);
    return result;
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

void setIntrinsicParams(double fx, double fy, double cx, double cy, Mat& intrinsic, Mat& inverse)
{
    intrinsic = Mat::zeros(3, 3, CV_64FC1);
    inverse   = Mat::zeros(3, 3, CV_64FC1);
    
	intrinsic.at<double>(0,0) = fx;
	intrinsic.at<double>(1,1) = fy;
	intrinsic.at<double>(0,2) = cx;
	intrinsic.at<double>(1,2) = cy;
	intrinsic.at<double>(2,2) = 1.0;
    
	// Create inverse calibration matrix:
	double tau = fx / fy;
    inverse.at<double>(0,0) = 1.0 / (tau * fy);
    inverse.at<double>(0,1) = 0.0;
    inverse.at<double>(0,2) = -cx / (tau * fy);
    inverse.at<double>(1,0) = 0.0;
    inverse.at<double>(1,1) = 1.0 / fy;
    inverse.at<double>(1,2) = -cy / fy;
    inverse.at<double>(2,0) = 0.0;
    inverse.at<double>(2,1) = 0.0;
    inverse.at<double>(2,2) = 1.0;
}

- (Mat)modelviewMatrixFromHomography:(const Mat&)homography
{
    // Camera parameters
    double fx = 786.42938232; // Focal length in x axis
    double fy = 786.42938232; // Focal length in y axis (usually the same?)
    double cx = 217.01358032; // Camera primary point x
    double cy = 311.25384521; // Camera primary point y
    
    // Decompose the Homography into translation and rotation vectors
    // Based on: https://gist.github.com/740979/97f54a63eb5f61f8f2eb578d60eb44839556ff3f
    Mat intrinsics, invIntrinsics;
    setIntrinsicParams(fx, fy, cx, cy, intrinsics, invIntrinsics);
    
    // Column vectors of homography
    Mat h1 = (Mat_<double>(3,1) << atD(homography, 0,0), atD(homography, 1,0), atD(homography, 2,0));
    Mat h2 = (Mat_<double>(3,1) << atD(homography, 0,1), atD(homography, 1,1), atD(homography, 2,1));
    Mat h3 = (Mat_<double>(3,1) << atD(homography, 0,2), atD(homography, 1,2), atD(homography, 2,2));
    
    Mat inverseH1 = invIntrinsics * h1;
    // Calculate a length, for normalizing
    double lambda = sqrt(atD(h1,0,0) * atD(h1,0,0) +
                         atD(h1,1,0) * atD(h1,1,0) +
                         atD(h1,2,0) * atD(h1,2,0));
    
    Mat rotationMatrix;
    
    if(lambda != 0) {
        lambda = 1/lambda;
        // Normalize inverseCameraMatrix
        invIntrinsics.at<double>(0,0) *= lambda;
        invIntrinsics.at<double>(1,0) *= lambda;
        invIntrinsics.at<double>(2,0) *= lambda;
        invIntrinsics.at<double>(0,1) *= lambda;
        invIntrinsics.at<double>(1,1) *= lambda;
        invIntrinsics.at<double>(2,1) *= lambda;
        invIntrinsics.at<double>(0,2) *= lambda;
        invIntrinsics.at<double>(1,2) *= lambda;
        invIntrinsics.at<double>(2,2) *= lambda;
        
        // Column vectors of rotation matrix
        Mat r1 = invIntrinsics * h1;
        Mat r2 = invIntrinsics * h2;
        Mat r3 = r1.cross(r2);    // Orthogonal to r1 and r2
        
        // Put rotation columns into rotation matrix... with some unexplained sign changes
        rotationMatrix = (Mat_<double>(3,3) <<
                           atD(r1,0,0), -atD(r2,0,0), -atD(r3,0,0),
                          -atD(r1,1,0),  atD(r2,1,0),  atD(r3,1,0),
                          -atD(r1,2,0),  atD(r2,2,0),  atD(r3,2,0));
        
        // Translation vector T
        Mat translationVector;
        translationVector = invIntrinsics * h3;
        translationVector.at<double>(0,0) *= 1;
        translationVector.at<double>(1,0) *= -1;
        translationVector.at<double>(2,0) *= -1;
        
        SVD decomposed(rotationMatrix); // I don't really know what this does. But it works.
        rotationMatrix = decomposed.u * decomposed.vt;
        
        Mat modelviewMatrix = (Mat_<double>(4,4) <<
            atD(rotationMatrix,0,0), atD(rotationMatrix,0,1), atD(rotationMatrix,0,2), atD(translationVector,0,0),
            atD(rotationMatrix,1,0), atD(rotationMatrix,1,1), atD(rotationMatrix,1,2), atD(translationVector,1,0),
            atD(rotationMatrix,2,0), atD(rotationMatrix,2,1), atD(rotationMatrix,2,2), atD(translationVector,2,0),
                                  0,                       0,                       0,                         1);
        return modelviewMatrix;
    }
    printf("Lambda was 0...\n");
    return Mat::eye(4, 4, CV_64FC1);
}

@end
