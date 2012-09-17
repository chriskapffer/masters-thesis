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

//- (Matrix4x4)modelView
//{
//    return [self modelViewWithHomography:_output.homography];
//}

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
    
    // TODO: do we need this?
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

// 352x288
//
//camera_matrix:
//    [3.2027001927961197e+02, 0., 1.2603930656676752e+02,
//     0., 7.8648529713443349e+02, 1.1084370236165464e+02,
//     0., 0., 1.]
//distortion_coefficients:
//    [-5.9372932410157073e+00,
//     6.3533343369070671e+01,
//     1.5611331766918132e-01,
//     1.2319019895336342e-01,
//     -2.5601747164123583e+02]

// 360x480
//
//camera_matrix:
//    [4.5364281643752662e+02, 0., 1.8255762334310054e+02,
//     0., 4.5678289063460659e+02, 2.2296548937969234e+02,
//     0., 0., 1.]
//distortion_coefficients:
//    [1.7537012227055074e-01,
//     4.3771881528128653e-01,
//     -2.5877934632447315e-02,
//     -8.5821219930251223e-04,
//     -4.9797434723229541e+00]

// 480x640
//
//camera_matrix:
//    [6.0838107806767721e+02, 0., 3.1060831055443424e+02,
//     0., 6.0862299514288236e+02, 2.3708121051002607e+02,
//     0., 0., 1.]
//distortion_coefficients:
//    [2.1889391539941197e-01,
//     -8.7606275768088160e-02,
//     1.5094562330422090e-03,
//     -8.4665365018427967e-03,
//     -2.4701006071163234e+00]

- (GLKMatrix4)projection
{
    // 480x640
    double fx = 604.18678406; // Focal length in x axis
    double fy = 604.43273831; // Focal length in y axis
    double cx = 320.00000000; // Camera primary point x
    double cy = 240.00000000; // Camera primary point y
    double width = 480;
    double height = 640;
    double offsetX = (480 - 320) / 2;//width / 2.;
    double offsetY = (640 - 240) / 2;//height / 2.;
    
    // calibrated camera matrix
    Mat intrinsic = Mat::zeros(3, 3, CV_64FC1);
	intrinsic.at<double>(0,0) = fx;
	intrinsic.at<double>(1,1) = fy;
	intrinsic.at<double>(0,2) = cx;
	intrinsic.at<double>(1,2) = cy;
	intrinsic.at<double>(2,2) = 1.0;
    std::cout << "intrinsics\n" << intrinsic << std::endl;
    
    double left = 0 + offsetX;
    double right = width + offsetX;
    double top = height + offsetY; // flip y
    double bottom = 0 + offsetY;
    double near = .1; // near clipping distance
    double far = 100; // far clipping distance
    
    // orthogonal matrix
    Mat ortho = Mat::zeros(4, 4, CV_64FC1);
    ortho.at<double>(0,0) = 2 / (right - left);
    ortho.at<double>(1,1) = 2 / (top - bottom);
    ortho.at<double>(2,2) = -(2 / (far - near));
    ortho.at<double>(0,3) = -((right + left) / (right - left));
    ortho.at<double>(1,3) = -((top + bottom) / (top - bottom));
    ortho.at<double>(2,3) = -((far + near) / (far - near));
    ortho.at<double>(3,3) = 1;
    std::cout << "ortho\n" << ortho << std::endl;
    
    // camera matrix
    Mat frustum = Mat::zeros(4, 4, CV_64FC1);
	frustum.at<double>(0,0) = fx;
	frustum.at<double>(1,1) = fy;
    frustum.at<double>(0,1) = 0; // skew
	frustum.at<double>(0,2) = -cx; // negative z
	frustum.at<double>(1,2) = -cy; // negative z
	frustum.at<double>(2,2) = near + far;
	frustum.at<double>(2,3) = near * far;
    frustum.at<double>(3,2) = -1; // negative z
    std::cout << "frustum\n" << frustum << std::endl;
    
    // projection matrix
    Mat projection = Mat::zeros(4, 4, CV_64FC1);
    projection = ortho * frustum;
    projection = projection.t();
    std::cout << "projection\n" << projection << std::endl;
    
    GLKMatrix4 result;
    result.m00 = projection.at<double>(0,0);
    result.m10 = projection.at<double>(1,0);
    result.m20 = projection.at<double>(2,0);
    result.m30 = projection.at<double>(3,0);
    result.m01 = projection.at<double>(0,1);
    result.m11 = projection.at<double>(1,1);
    result.m21 = projection.at<double>(2,1);
    result.m31 = projection.at<double>(3,1);
    result.m02 = projection.at<double>(0,2);
    result.m12 = projection.at<double>(1,2);
    result.m22 = projection.at<double>(2,2);
    result.m32 = projection.at<double>(3,2);
    result.m03 = projection.at<double>(0,3);
    result.m13 = projection.at<double>(1,3);
    result.m23 = projection.at<double>(2,3);
    result.m33 = projection.at<double>(3,3);
    
    return result;
}

- (GLKMatrix4)modelView
{
    // Decompose the Homography into translation and rotation vectors
    // Based on: https://gist.github.com/740979
    
    double fx = 604.18678406; // Focal length in x axis
    double fy = 604.43273831; // Focal length in y axis
    double cx = 320.00000000; // Camera primary point x
    double cy = 240.00000000; // Camera primary point y
    
    Mat cameraMatrix = Mat::zeros(3, 3, CV_64FC1);
	cameraMatrix.at<double>(0,0) = fx;
	cameraMatrix.at<double>(1,1) = fy;
	cameraMatrix.at<double>(0,2) = cx;
	cameraMatrix.at<double>(1,2) = cy;
	cameraMatrix.at<double>(2,2) = 1.0;

    Mat inverseCamera = Mat::zeros(3, 3, CV_64FC1);
    inverseCamera.at<double>(0,0) = 1 / fx;
    inverseCamera.at<double>(0,2) = -cx / fx;
    inverseCamera.at<double>(1,1) = 1 / fy;
    inverseCamera.at<double>(1,2) = -cy / fy;
    inverseCamera.at<double>(2,2) = 1;
    
    Mat H_matrix = _output.homography;
    
    // Column vectors of homography
    Mat h1 = H_matrix.col(0);
    Mat h2 = H_matrix.col(1);
    Mat h3 = H_matrix.col(2);
    
    Mat inverseH1 = inverseCamera * h1;
    // Calculate a length, for normalizing
    double lambda = sqrt(inverseH1.at<double>(0,0)*inverseH1.at<double>(0,0) +
                         inverseH1.at<double>(1,0)*inverseH1.at<double>(1,0) +
                         inverseH1.at<double>(2,0)*inverseH1.at<double>(2,0));

    if(lambda != 0) {
        lambda = 1/lambda;
        // Normalize inverseCameraMatrix
        inverseCamera *= lambda;
    }
    else {
        printf("Lambda was 0...\n");
    }
    
    // Column vectors of rotation matrix
    Mat r1 = inverseCamera * h1;
    Mat r2 = inverseCamera * h2;
    Mat r3 = r1.cross(r2);    // Orthogonal to r1 and r2
    
    // Put rotation columns into rotation matrix... with some unexplained sign changes
    Mat rotationMatrix = (Mat_<double>(3,3) <<
                          r1.at<double>(0,0), r2.at<double>(0,0), r3.at<double>(0,0),
                          r1.at<double>(1,0), r2.at<double>(1,0), r3.at<double>(1,0),
                          r1.at<double>(2,0), r2.at<double>(2,0), r3.at<double>(2,0));
    
    //SVD decomposed(rotationMatrix); // smooths stuff, but disables scaling // TODO: understand this
    //rotationMatrix = decomposed.u * decomposed.vt;
    
    // Translation vector T
    Mat translationVector = inverseCamera * h3;
//    translationVector.at<double>(0,0) *=  1;
//    translationVector.at<double>(1,0) *= -1;
//    translationVector.at<double>(2,0) *= -1;
    
    Mat modelView = (Mat_<double>(4,4) <<
                       rotationMatrix.at<double>(0,0), rotationMatrix.at<double>(0,1), rotationMatrix.at<double>(0,2), 0,
                       rotationMatrix.at<double>(1,0), rotationMatrix.at<double>(1,1), rotationMatrix.at<double>(1,2), 0,
                       rotationMatrix.at<double>(2,0), rotationMatrix.at<double>(2,1), rotationMatrix.at<double>(2,2), 0,
                       -translationVector.at<double>(1,0) / 240, -translationVector.at<double>(0,0) / 320, 0, 1);
    //modelView = modelView.t();
    
    
//    double fx = 604.18678406; // Focal length in x axis
//    double fy = 604.43273831; // Focal length in y axis
//    double cx = 320.00000000; // Camera primary point x
//    double cy = 240.00000000; // Camera primary point y
//    
//    Mat cameraMatrix = (Mat_<double>(3,3) <<
//                        fx,  0, cx,
//                        0, fy, cy,
//                        0,  0,  1);
//    
//    Mat inverseCameraMatrix = (Mat_<double>(3,3) <<
//                               1/cameraMatrix.at<double>(0,0) , 0 , -cameraMatrix.at<double>(0,2)/cameraMatrix.at<double>(0,0) ,
//                               0 , 1/cameraMatrix.at<double>(1,1) , -cameraMatrix.at<double>(1,2)/cameraMatrix.at<double>(1,1) ,
//                               0 , 0 , 1);
//    
//    Mat H_matrix = _output.homography;
//    
//    // Column vectors of homography
//    Mat h1 = H_matrix.col(0);
//    Mat h2 = H_matrix.col(1);
//    Mat h3 = H_matrix.col(2);
//    
//    Mat inverseH1 = inverseCameraMatrix * h1;
//    // Calculate a length, for normalizing
//    double lambda = sqrt(inverseH1.at<double>(0,0)*inverseH1.at<double>(0,0) +
//                         inverseH1.at<double>(1,0)*inverseH1.at<double>(1,0) +
//                         inverseH1.at<double>(2,0)*inverseH1.at<double>(2,0));
//    
//    if(lambda != 0) {
//        lambda = 1/lambda;
//        // Normalize inverseCameraMatrix
//        inverseCameraMatrix.at<double>(0,0) *= lambda;
//        inverseCameraMatrix.at<double>(1,0) *= lambda;
//        inverseCameraMatrix.at<double>(2,0) *= lambda;
//        inverseCameraMatrix.at<double>(0,1) *= lambda;
//        inverseCameraMatrix.at<double>(1,1) *= lambda;
//        inverseCameraMatrix.at<double>(2,1) *= lambda;
//        inverseCameraMatrix.at<double>(0,2) *= lambda;
//        inverseCameraMatrix.at<double>(1,2) *= lambda;
//        inverseCameraMatrix.at<double>(2,2) *= lambda;
//    }
//    else {
//        printf("Lambda was 0...\n");
//    }
//    
//    // Column vectors of rotation matrix
//    Mat r1 = h1;
//    Mat r2 = h2;
//    Mat r3 = r1.cross(r2);    // Orthogonal to r1 and r2
//    
//    // Put rotation columns into rotation matrix... with some unexplained sign changes
//    Mat rotationMatrix = (Mat_<double>(3,3) <<
//                          r1.at<double>(0,0), r2.at<double>(0,0), r3.at<double>(0,0),
//                          r1.at<double>(1,0), r2.at<double>(1,0), r3.at<double>(1,0),
//                          r1.at<double>(2,0), r2.at<double>(2,0), r3.at<double>(2,0));
//    
//    //SVD decomposed(rotationMatrix); // smooths stuff, but disables scaling // TODO: understand this
//    //rotationMatrix = decomposed.u * decomposed.vt;
//    
//    // Translation vector T
//    Mat translationVector = h3;//inverseCameraMatrix * h3;
//    translationVector.at<double>(0,0) *=  1;
//    translationVector.at<double>(1,0) *= -1;
//    translationVector.at<double>(2,0) *= -1;
//    
//    Mat modelView = (Mat_<double>(4,4) <<
//                     rotationMatrix.at<double>(0,0), rotationMatrix.at<double>(0,1), rotationMatrix.at<double>(0,2), 0,//translationVector.at<double>(0,0),
//                     rotationMatrix.at<double>(1,0), rotationMatrix.at<double>(1,1), rotationMatrix.at<double>(1,2), 0,//translationVector.at<double>(1,0),
//                     rotationMatrix.at<double>(2,0), rotationMatrix.at<double>(2,1), rotationMatrix.at<double>(2,2), 1,//translationVector.at<double>(2,0),
//                     0,0,0,1);
//    //modelView = modelView.t();
//
    GLKMatrix4 result;
    result.m00 = modelView.at<double>(0,0);
    result.m10 = modelView.at<double>(1,0);
    result.m20 = modelView.at<double>(2,0);
    result.m30 = modelView.at<double>(3,0);
    result.m01 = modelView.at<double>(0,1);
    result.m11 = modelView.at<double>(1,1);
    result.m21 = modelView.at<double>(2,1);
    result.m31 = modelView.at<double>(3,1);
    result.m02 = modelView.at<double>(0,2);
    result.m12 = modelView.at<double>(1,2);
    result.m22 = modelView.at<double>(2,2);
    result.m32 = modelView.at<double>(3,2);
    result.m03 = modelView.at<double>(0,3);
    result.m13 = modelView.at<double>(1,3);
    result.m23 = modelView.at<double>(2,3);
    result.m33 = modelView.at<double>(3,3);
    return result;
}

- (Mat)modelviewMatrixFromHomography:(const Mat&)homography
{
    // Camera parameters
    
    // from http://urbanar.blogspot.de/2011/04/from-homography-to-opengl-modelview.html
//    double fx = 786.42938232; // Focal length in x axis
//    double fy = 786.42938232; // Focal length in y axis
//    double cx = 217.01358032; // Camera primary point x
//    double cy = 311.25384521; // Camera primary point y
    
//    // 352x288
//    double fx = 320.27001927; // Focal length in x axis
//    double fy = 320.27001927; // Focal length in y axis
//    double cx = 352 * 0.5f; // Camera primary point x
//    double cy = 288 * 0.5f; // Camera primary point y
//
//    // 360x480
//    double fx = 453.64281643; // Focal length in x axis
//    double fy = 456.78289063; // Focal length in y axis
//    double cx = 182.55762334; // Camera primary point x
//    double cy = 222.96548937; // Camera primary point y
    
    // 480x640
    double fx = 604.18678406 * 0.5; // Focal length in x axis
    double fy = 604.43273831 * 0.5; // Focal length in y axis
    double cx = 319.50000000 * 0.5; // Camera primary point x
    double cy = 239.50000000 * 0.5; // Camera primary point y
    double width = 480 * 0.5;
    double height = 640 * 0.5;
    
    // calibrated camera matrix
    Mat intrinsic = Mat::zeros(3, 3, CV_64FC1);
	intrinsic.at<double>(0,0) = fx;
	intrinsic.at<double>(1,1) = fy;
	intrinsic.at<double>(0,2) = cx;
	intrinsic.at<double>(1,2) = cy;
	intrinsic.at<double>(2,2) = 1.0;
    std::cout << "intrinsics\n" << intrinsic << std::endl;
    
    double left = 0;
    double right = width;
    double top = height; // flip y
    double bottom = 0;
    double near = .1; // near clipping distance
    double far = 100; // far clipping distance
    
    // orthogonal matrix
    Mat ortho = Mat::zeros(4, 4, CV_64FC1);
    ortho.at<double>(0,0) = 2 / (right - left);
    ortho.at<double>(1,1) = 2 / (top - bottom);
    ortho.at<double>(2,2) = -(2 / (far - near));
    ortho.at<double>(0,3) = -((right + left) / (right - left));
    ortho.at<double>(1,3) = -((top + bottom) / (top - bottom));
    ortho.at<double>(2,3) = -((far + near) / (far - near));
    ortho.at<double>(3,3) = 1;
    std::cout << "ortho\n" << ortho << std::endl;
    
    // camera matrix
    Mat frustum = Mat::zeros(4, 4, CV_64FC1);
	frustum.at<double>(0,0) = fx;
	frustum.at<double>(1,1) = fy;
    frustum.at<double>(0,1) = 0; // skew
	frustum.at<double>(0,2) = -cx; // negative z
	frustum.at<double>(1,2) = -cy; // negative z
	frustum.at<double>(2,2) = near + far;
	frustum.at<double>(2,3) = near * far;
    frustum.at<double>(3,2) = -1; // negative z
    std::cout << "frustum\n" << frustum << std::endl;
    
    // projection matrix
    Mat projection = Mat::zeros(4, 4, CV_64FC1);
    projection = ortho * frustum;
    std::cout << "projection\n" << projection << std::endl;
    
    // homography columns
    std::cout << "homography\n" << homography << std::endl;
    Mat h1 = homography.col(0);
    Mat h2 = homography.col(1);
    Mat h3 = homography.col(2);
    
    // rotation matrix columns
    Mat r1 = h1;
    Mat r2 = h2;
    Mat r3 = h1.cross(h2);
    
    // translation vector
    Mat t = h3;
    
    // model view matrix
    Mat modelView = Mat::zeros(4, 4, CV_64FC1);
    modelView.at<double>(0,0) = r1.at<double>(0,0);
    modelView.at<double>(1,0) = r1.at<double>(1,0);
    modelView.at<double>(2,0) = r1.at<double>(2,0);
    modelView.at<double>(0,1) = r2.at<double>(0,0);
    modelView.at<double>(1,1) = r2.at<double>(1,0);
    modelView.at<double>(2,1) = r2.at<double>(2,0);
    modelView.at<double>(0,2) = r3.at<double>(0,0);
    modelView.at<double>(1,2) = r3.at<double>(1,0);
    modelView.at<double>(2,2) = r3.at<double>(2,0);
    modelView.at<double>(0,3) = t.at<double>(0,0);
    modelView.at<double>(1,3) = t.at<double>(1,0);
    modelView.at<double>(2,3) = t.at<double>(2,0);
    modelView.at<double>(3,3) = 1; // homogenous coordinate;
    std::cout << "modelView\n" << modelView << std::endl;
    
    return Mat::eye(4, 4, CV_64FC1);
}

@end
