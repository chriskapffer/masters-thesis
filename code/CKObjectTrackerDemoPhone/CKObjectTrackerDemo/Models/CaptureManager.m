//
//  CaptureManager.m
//  CKObjectTrackerDemo
//
//  Created by Christoph Kapffer on 31.05.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import "CaptureManager.h"

#import "FPSCalcApple.h"
#import "FPSCalcSummerhill.h"
#import "Utils.h"

@interface CaptureManager () <AVCaptureVideoDataOutputSampleBufferDelegate>

@property (nonatomic, strong) AVCaptureSession* captureSession;
@property (nonatomic, strong) AVCaptureDeviceInput* videoInput;
@property (nonatomic, strong) AVCaptureVideoDataOutput* videoOutput;
@property (nonatomic, strong) AVCaptureVideoPreviewLayer* previewLayer;
@property (nonatomic, strong) dispatch_queue_t videoCaptureQueue;

@property (nonatomic, strong) FPSCalculator* fpsCalculator;

@end

@implementation CaptureManager

#pragma mark - Properties

@synthesize captureSession = _captureSession;
@synthesize videoInput = _videoInput;
@synthesize videoOutput = _videoOutput;
@synthesize previewLayer = _previewLayer;
@synthesize videoCaptureQueue = _videoCaptureQueue;
@synthesize sessionPreset = _sessionPreset;

@synthesize fpsCalculator = _fpsCalculator;
@synthesize delegate = _delegate;

#pragma mark Getters

- (AVCaptureTorchMode)torchMode
{
    return [[self.videoInput device] torchMode];
}

- (AVCaptureFocusMode)focusMode
{
    return [[self.videoInput device] focusMode];
}

- (AVCaptureExposureMode)exposureMode
{
    return [[self.videoInput device] exposureMode];
}

- (AVCaptureWhiteBalanceMode)whiteBalanceMode
{
    return [[self.videoInput device] whiteBalanceMode];
}

- (AVCaptureDevicePosition)currentCameraPosition
{
    return [[self.videoInput device] position];
}

- (NSArray*)availableSessionPresets
{
    static NSArray* presets = nil;
    if (presets == nil) {
        presets = [NSArray arrayWithObjects:
                   AVCaptureSessionPreset1920x1080,
                   AVCaptureSessionPreset1280x720,
                   AVCaptureSessionPreset640x480,
                   AVCaptureSessionPresetMedium, // 480x360
                   AVCaptureSessionPreset352x288,
                   AVCaptureSessionPresetLow, // 192x144
                   nil];
    }
    
    NSMutableArray* availablePresets = [NSMutableArray arrayWithArray:presets];
    for (int i = availablePresets.count - 1; i >= 0; i--) {
        if (![self.captureSession canSetSessionPreset:[availablePresets objectAtIndex:i]]) {
            [availablePresets removeObjectAtIndex:i];
        }
    }
    return availablePresets;
}

- (NSString*)sessionPreset
{
    return _sessionPreset;
}

- (OSType)pixelFormat
{
    return [[[self.videoOutput videoSettings] objectForKey:(id)kCVPixelBufferPixelFormatTypeKey] unsignedIntValue];
}

- (float)framesPerSecond
{
    return [self.fpsCalculator framesPerSecond];
}

#pragma mark Setters

- (void)setTorchMode:(AVCaptureTorchMode)torchMode
{
    AVCaptureDevice* device = [self.videoInput device];
    if (![device isTorchModeSupported:torchMode] || [device torchMode] == torchMode)
        return;
    
    NSError* error;
    if (![device lockForConfiguration:&error])
    {
        [self showError:error];
        return;
    }
    
    [device setTorchMode:torchMode];
    [device unlockForConfiguration];
}

- (void)setFocusMode:(AVCaptureFocusMode)focusMode
{
    AVCaptureDevice* device = [self.videoInput device];
    if (![device isFocusModeSupported:focusMode] || [device focusMode] == focusMode)
        return;
    
    NSError* error;
    if (![device lockForConfiguration:&error])
    {
        [self showError:error];
        return;
    }
    
    [device setFocusMode:focusMode];
    [device unlockForConfiguration];
}

- (void)setExposureMode:(AVCaptureExposureMode)exposureMode
{
    AVCaptureDevice* device = [self.videoInput device];
    if (![device isExposureModeSupported:exposureMode] || [device exposureMode] == exposureMode)
        return;
    
    NSError* error;
    if (![device lockForConfiguration:&error])
    {
        [self showError:error];
        return;
    }
    
    [device setExposureMode:exposureMode];
    [device unlockForConfiguration];
}

- (void)setWhiteBalanceMode:(AVCaptureWhiteBalanceMode)whiteBalanceMode
{
    AVCaptureDevice* device = [self.videoInput device];
    if (![device isWhiteBalanceModeSupported:whiteBalanceMode] || [device whiteBalanceMode] == whiteBalanceMode)
        return;
    
    NSError* error;
    if (![device lockForConfiguration:&error])
    {
        [self showError:error];
        return;
    }
    
    [device setWhiteBalanceMode:whiteBalanceMode];
    [device unlockForConfiguration];
}

- (void)setCurrentCameraPosition:(AVCaptureDevicePosition)newCameraPosition
{
    AVCaptureDevice* oldDevice = [self.videoInput device];
    AVCaptureDevice* newDevice = [self cameraWithPosition:newCameraPosition];
    
    if (!newDevice || newDevice == oldDevice)
        return;
    
    NSError *error;
    AVCaptureDeviceInput* oldVideoInput = [self videoInput];
    AVCaptureDeviceInput* newVideoInput = [[AVCaptureDeviceInput alloc] initWithDevice:newDevice error:&error];
    if (error)
    {
        [self showError:error];
        return;
    }
    
    [self.captureSession beginConfiguration];
    
    // remove old input first, otherwise nothing can be added
    [self.captureSession removeInput:oldVideoInput];
    if ([self.captureSession canAddInput:newVideoInput])
    {
        [self.captureSession addInput:newVideoInput];
        [self setVideoInput:newVideoInput];
        if ([self.delegate respondsToSelector:@selector(didChangeCameraPosition:)])
            [self.delegate didChangeCameraPosition:newCameraPosition];
    }
    else
    {
        // re-insert old input if new input couldn't be added
        [self.captureSession addInput:oldVideoInput];
    }
    
    [self.captureSession commitConfiguration];
}

- (void)setPixelFormat:(OSType)pixelFormat
{
    if (pixelFormat == [self pixelFormat] || ![self canSetPixelFormat:pixelFormat])
        return;
    
    NSMutableDictionary* settings = [NSMutableDictionary dictionaryWithDictionary:[self.videoOutput videoSettings]];
    [settings setObject:[NSNumber numberWithUnsignedInt:pixelFormat] forKey:(id)kCVPixelBufferPixelFormatTypeKey];
    [self.videoOutput setVideoSettings:settings];
}

- (void)setSessionPreset:(NSString *)sessionPreset
{
    if ([_sessionPreset isEqualToString:sessionPreset] || ![self canSetSessionPreset:sessionPreset])
        return;
    _sessionPreset = sessionPreset;
    
    [self.captureSession beginConfiguration];
    [self.captureSession setSessionPreset:sessionPreset];
    [self.captureSession commitConfiguration];
}

#pragma mark - Initialization methods

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
    if(self)
    {
        //fpsCalculator = [[FPSCalcSummerhill alloc] init];
        _sessionPreset = AVCaptureSessionPresetMedium;
        _fpsCalculator = [[FPSCalcApple alloc] init];
    }
    return self;
}

- (void)dealloc
{
    if (self.captureSession != nil)
        [self stopAndShutDownCaptureSession];
}

- (BOOL)setUpAndStartCaptureSession
{
	NSError* error = nil;
    
    // create new capture session instance
    AVCaptureSession *newCaptureSession = [[AVCaptureSession alloc] init];
    
    // Init the device inputs
    AVCaptureDeviceInput* newVideoInput = [[AVCaptureDeviceInput alloc] initWithDevice:[self cameraWithPosition:AVCaptureDevicePositionBack] error:&error];
    
    if (error)
    {
        [self showError:error];
		return NO;
    }
    
    // Setup the video output
    NSDictionary* videoOutputSettings = [NSDictionary dictionaryWithObjectsAndKeys:
        [NSNumber numberWithUnsignedInt:kCVPixelFormatType_32BGRA], (id)kCVPixelBufferPixelFormatTypeKey,
        nil];

    self.videoCaptureQueue = dispatch_queue_create("ck.video.capturemanager.capture", DISPATCH_QUEUE_SERIAL);

    AVCaptureVideoDataOutput *newVideoOutput = [[AVCaptureVideoDataOutput alloc] init];    
	[newVideoOutput setSampleBufferDelegate:self queue:self.videoCaptureQueue];
	[newVideoOutput setAlwaysDiscardsLateVideoFrames:YES];
    [newVideoOutput setVideoSettings:videoOutputSettings];
    
    // Add inputs and output to the capture session
    if ([newCaptureSession canAddInput:newVideoInput]) {
        [newCaptureSession addInput:newVideoInput];
    }
    
    if ([newCaptureSession canAddOutput:newVideoOutput]) {
        [newCaptureSession addOutput:newVideoOutput];
    }
    
//    // try to set session preset, start with highest
//    if ([newCaptureSession canSetSessionPreset:AVCaptureSessionPreset1920x1080]) {
//        newCaptureSession.sessionPreset = AVCaptureSessionPreset1920x1080;
//    } else if ([newCaptureSession canSetSessionPreset:AVCaptureSessionPreset1280x720]) {
//        newCaptureSession.sessionPreset = AVCaptureSessionPreset1280x720;
//    } else if ([newCaptureSession canSetSessionPreset:AVCaptureSessionPreset640x480]) {
//        newCaptureSession.sessionPreset = AVCaptureSessionPreset640x480;
//    } else {
//        newCaptureSession.sessionPreset = AVCaptureSessionPresetMedium;
//    }
    newCaptureSession.sessionPreset = self.sessionPreset;
    
    [self setVideoInput:newVideoInput];
    [self setVideoOutput:newVideoOutput];
    [self setCaptureSession:newCaptureSession];
    
    // start the capture session running, note this is an async operation
    // status is provided via notifications such as AVCaptureSessionDidStartRunningNotification/AVCaptureSessionDidStopRunningNotification
    [self.captureSession startRunning];
    
    [self addObserver:self forKeyPath:@"videoInput.device.adjustingFocus" options:NSKeyValueObservingOptionNew context:NULL];
    [self addObserver:self forKeyPath:@"videoInput.device.adjustingExposure" options:NSKeyValueObservingOptionNew context:NULL];
    [self addObserver:self forKeyPath:@"videoInput.device.adjustingWhiteBalance" options:NSKeyValueObservingOptionNew context:NULL];
    
	return YES;
}

- (void)stopAndShutDownCaptureSession
{
    // TODO: remove preview layer
    [self.captureSession stopRunning];
    
    dispatch_sync(self.videoCaptureQueue, ^{ });
    //dispatch_release(self.videoCaptureQueue);
    
    [self removeObserver:self forKeyPath:@"videoInput.device.adjustingFocus"];
    [self removeObserver:self forKeyPath:@"videoInput.device.adjustingExposure"];
    [self removeObserver:self forKeyPath:@"videoInput.device.adjustingWhiteBalance"];
    
    self.captureSession = nil;
    self.videoOutput = nil;
    self.videoInput = nil;
}

- (void)addPreview:(UIView*)view
{
    if(!self.previewLayer)
    {
        // create a preview layer so we can see the visual output of an AVCaptureSession
        self.previewLayer = [[AVCaptureVideoPreviewLayer alloc] initWithSession:[self captureSession]];
        [self.previewLayer setVideoGravity:AVLayerVideoGravityResizeAspectFill];
        [self.previewLayer setFrame:[view bounds]];
    }
	
    // add the preview layer to the hierarchy
	[[view layer] addSublayer:self.previewLayer];
}

-(void)removePreview
{
    if(self.previewLayer)
    {
        [self.previewLayer removeFromSuperlayer];
        self.previewLayer = nil;
    }
}

#pragma mark - Predicate methods

- (BOOL)hasTorch
{
    return [[self.videoInput device] hasTorch];
}

- (BOOL)hasFocus
{
    AVCaptureDevice* device = [self.videoInput device];
    return [device isFocusModeSupported:AVCaptureFocusModeContinuousAutoFocus]
        && [device isFocusModeSupported:AVCaptureFocusModeLocked];
}

- (BOOL)hasExposure
{
    AVCaptureDevice* device = [self.videoInput device];
    return [device isExposureModeSupported:AVCaptureExposureModeContinuousAutoExposure]
        && [device isExposureModeSupported:AVCaptureExposureModeLocked];
}

- (BOOL)hasWhiteBalance
{
    AVCaptureDevice* device = [self.videoInput device];
    return [device isWhiteBalanceModeSupported:AVCaptureWhiteBalanceModeContinuousAutoWhiteBalance]
        && [device isWhiteBalanceModeSupported:AVCaptureWhiteBalanceModeLocked];
}

- (BOOL)hasFrontCamera
{
    return [self cameraWithPosition:AVCaptureDevicePositionFront] != nil;
}

- (BOOL)isOrientationSupported
{
    return [[self previewLayer] isOrientationSupported];
}

- (BOOL)canSetSessionPreset:(NSString*)preset
{
    return [self.captureSession canSetSessionPreset:preset];
}

- (BOOL)canSetPixelFormat:(OSType)format
{
    return [self.videoOutput.availableVideoCVPixelFormatTypes containsObject:[NSNumber numberWithUnsignedInt:format]];
}

#pragma mark - Configuration methods

- (void) focusAtPoint:(CGPoint)point
{
    AVCaptureDevice* device = [self.videoInput device];
    if ([device isFocusPointOfInterestSupported])
    {
        NSError* error;
        if (![device lockForConfiguration:&error])
        {
            [self showError:error];
            return;
        }
        
        [device setFocusPointOfInterest:point];
        [device unlockForConfiguration];     
    }
}

- (void)exposureAtPoint:(CGPoint)point
{
    AVCaptureDevice* device = [self.videoInput device];
    if ([device isExposurePointOfInterestSupported])
    {
        NSError* error;
        if (![device lockForConfiguration:&error])
        {
            [self showError:error];
            return;
        }
        
        [device setExposurePointOfInterest:point];
        [device unlockForConfiguration];     
    }
}

#pragma mark - Other public methods

- (BOOL)canSetCameraPosition:(AVCaptureDevicePosition)position ForSessionPreset:(NSString*)preset
{
    return !(position == AVCaptureDevicePositionFront
             && ([preset isEqualToString:AVCaptureSessionPreset1920x1080]
                 || [preset isEqualToString:AVCaptureSessionPreset1280x720]
                 || [preset isEqualToString:AVCaptureSessionPresetiFrame1280x720]
                 || [preset isEqualToString:AVCaptureSessionPresetiFrame960x540]));
}

- (void)setCameraPosition:(AVCaptureDevicePosition)position WithSessionPreset:(NSString*)preset
{
    if (![self canSetCameraPosition:position ForSessionPreset:preset] || ![self canSetSessionPreset:preset])
        return;
    
    NSError *error;
    AVCaptureDevice* device = [self cameraWithPosition:position];
    AVCaptureDeviceInput* input = [[AVCaptureDeviceInput alloc] initWithDevice:device error:&error];
    if (error) {
        [self showError:error];
        return;
    }
    
    // do both operations in one configuration block to keep things smooth
    [self.captureSession beginConfiguration];
    // change preset before adding new input
    [self.captureSession setSessionPreset:preset];
    // remove old input first, otherwise nothing can be added
    [self.captureSession removeInput:[self videoInput]];
    [self.captureSession addInput:input];
    [self.captureSession commitConfiguration];
    
    [self setVideoInput:input];
}

- (void)rotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    if ([self.previewLayer isOrientationSupported])
        [self.previewLayer setOrientation:[self videoOrientationFromInterfaceOrientation:interfaceOrientation]];
    
    [self.previewLayer setFrame:self.previewLayer.superlayer.bounds];
}

- (CGSize)videoResolutionForSessionPreset:(NSString*)preset
{
    if ([preset isEqualToString:AVCaptureSessionPresetPhoto] || [preset isEqualToString:AVCaptureSessionPresetHigh]) {
        NSString* camPosition = (self.videoInput.device.position == AVCaptureDevicePositionFront ? @"Front" : @"Back");
        preset = [NSString stringWithFormat:@"%@%@%@", preset, deviceModelName(), [self hasFrontCamera] ? camPosition : @""];
    }

    static NSDictionary* presetTable = nil;
    if (presetTable == nil) {
        presetTable = [NSDictionary dictionaryWithObjectsAndKeys:
                       [NSValue valueWithCGSize:CGSizeMake(512, 384)], @"AVCaptureSessionPresetPhotoiPhone3GS",
                       [NSValue valueWithCGSize:CGSizeMake(640, 480)], @"AVCaptureSessionPresetPhotoiPhone4Front",
                       [NSValue valueWithCGSize:CGSizeMake(852, 640)], @"AVCaptureSessionPresetPhotoiPhone4Back",
                       [NSValue valueWithCGSize:CGSizeMake(640, 480)], @"AVCaptureSessionPresetPhotoiPhone4SFront",
                       [NSValue valueWithCGSize:CGSizeMake(852, 640)], @"AVCaptureSessionPresetPhotoiPhone4SBack",
                       [NSValue valueWithCGSize:CGSizeMake(640, 480)], @"AVCaptureSessionPresetHighiPhone3GS",
                       [NSValue valueWithCGSize:CGSizeMake(640, 480)], @"AVCaptureSessionPresetHighiPhone4Front",
                       [NSValue valueWithCGSize:CGSizeMake(1280, 720)], @"AVCaptureSessionPresetHighiPhone4Back",
                       [NSValue valueWithCGSize:CGSizeMake(640, 480)], @"AVCaptureSessionPresetHighiPhone4SFront",
                       [NSValue valueWithCGSize:CGSizeMake(1920, 1080)], @"AVCaptureSessionPresetHighiPhone4SBack",
                       [NSValue valueWithCGSize:CGSizeMake(480, 360)], AVCaptureSessionPresetMedium,
                       [NSValue valueWithCGSize:CGSizeMake(192, 144)], AVCaptureSessionPresetLow,
                       [NSValue valueWithCGSize:CGSizeMake(1920, 1080)], AVCaptureSessionPreset1920x1080,
                       [NSValue valueWithCGSize:CGSizeMake(1280, 720)], AVCaptureSessionPreset1280x720,
                       [NSValue valueWithCGSize:CGSizeMake(640, 480)], AVCaptureSessionPreset640x480,
                       [NSValue valueWithCGSize:CGSizeMake(352, 288)], AVCaptureSessionPreset352x288,
                       nil];
    }
    return [[presetTable objectForKey:preset] CGSizeValue];
}

#pragma mark - Sample buffer delegate

- (void)captureOutput:(AVCaptureOutput *)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection {
    [self.fpsCalculator calculateFramerateAtTimestamp:CMSampleBufferGetPresentationTimeStamp(sampleBuffer)];
    
    CVPixelBufferRef pixelBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);

    OSType format = CVPixelBufferGetPixelFormatType(pixelBuffer);
    if (format == kCVPixelFormatType_32BGRA || format == kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange || format == kCVPixelFormatType_422YpCbCr8) {
        if ([self.delegate respondsToSelector:@selector(didCaptureFrameWithPixelBuffer:)]) {
            [self.delegate didCaptureFrameWithPixelBuffer:pixelBuffer];
        }
    }
    else {
        NSLog(@"Unsupported pixel format.");
    }
}

#pragma mark - Key value observer

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{
    BOOL value = [[change objectForKey:NSKeyValueChangeNewKey] boolValue];
    
    if ([keyPath isEqual:@"videoInput.device.adjustingFocus"]) {
        if ([self.delegate respondsToSelector:@selector(adjustingFocusPropertyChanged:)])
            [self.delegate adjustingFocusPropertyChanged:value];
    }
    else if ([keyPath isEqual:@"videoInput.device.adjustingExposure"])  {
        if ([self.delegate respondsToSelector:@selector(adjustingExposurePropertyChanged:)])
            [self.delegate adjustingExposurePropertyChanged:value];
    }
    else if ([keyPath isEqual:@"videoInput.device.adjustingWhiteBalance"])  {
        if ([self.delegate respondsToSelector:@selector(adjustingWhiteBalancePropertyChanged:)])
            [self.delegate adjustingWhiteBalancePropertyChanged:value];
    }
    else {
        
    }
}

#pragma mark - Helper Methods

- (AVCaptureVideoOrientation)videoOrientationFromInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    AVCaptureVideoOrientation orientation;
    switch (interfaceOrientation) {
        case UIInterfaceOrientationPortrait:
            orientation = AVCaptureVideoOrientationPortrait;
            break;
        case UIInterfaceOrientationPortraitUpsideDown:
            orientation = AVCaptureVideoOrientationPortraitUpsideDown;
            break;
        case UIInterfaceOrientationLandscapeLeft:
            orientation = AVCaptureVideoOrientationLandscapeLeft;
            break;
        case UIInterfaceOrientationLandscapeRight:
            orientation = AVCaptureVideoOrientationLandscapeRight;
            break;
        default:
            break;
    }
    return orientation;
}

- (AVCaptureDevice*)cameraWithPosition:(AVCaptureDevicePosition)position
{
    NSArray* devices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
    for (AVCaptureDevice* device in devices) {
        if ([device position] == position) {
            return device;
        }
    }
    return nil;
}

// Displays an error message in an alert view
- (void)showError:(NSError*)error
{
    CFRunLoopPerformBlock(CFRunLoopGetMain(), kCFRunLoopCommonModes, ^(void) {
        UIAlertView* alertView = [[UIAlertView alloc] initWithTitle:[error localizedDescription]
                                                            message:[error localizedFailureReason]
                                                           delegate:nil
                                                  cancelButtonTitle:@"OK"
                                                  otherButtonTitles:nil];
        [alertView show];
    });
}

@end
