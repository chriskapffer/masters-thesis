//
//  CaptureManager.h
//  CKObjectTrackerDemo
//
//  Created by Christoph Kapffer on 31.05.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

@protocol CaptureManagerDelegate <NSObject>
- (void)didCaptureFrameWithPixelBuffer:(CVPixelBufferRef)pixelBuffer;
@optional
- (void)didChangeCameraPosition:(AVCaptureDevicePosition)cameraPosition;
- (void)adjustingFocusPropertyChanged:(BOOL)isAdjustingFocus;
- (void)adjustingExposurePropertyChanged:(BOOL)isAdjustingExposure;
- (void)adjustingWhiteBalancePropertyChanged:(BOOL)isAdjustingWhiteBalance;
@end

@interface CaptureManager : NSObject 

@property (nonatomic, assign) id <CaptureManagerDelegate> delegate;
@property (nonatomic, assign, readonly) float framesPerSecond;

@property (nonatomic, assign) AVCaptureTorchMode torchMode;
@property (nonatomic, assign) AVCaptureFocusMode focusMode;
@property (nonatomic, assign) AVCaptureExposureMode exposureMode;
@property (nonatomic, assign) AVCaptureWhiteBalanceMode whiteBalanceMode;
@property (nonatomic, assign) AVCaptureDevicePosition currentCameraPosition;
@property (nonatomic, strong, readonly) NSArray* availableSessionPresets;
@property (nonatomic, strong) NSString* sessionPreset;
@property (nonatomic, assign) OSType pixelFormat;

+ (id)instance;

- (BOOL)hasTorch;
- (BOOL)hasFocus;
- (BOOL)hasExposure;
- (BOOL)hasWhiteBalance;
- (BOOL)hasFrontCamera;
- (BOOL)isOrientationSupported;

- (BOOL)canSetSessionPreset:(NSString*)preset;
- (BOOL)canSetPixelFormat:(OSType)format;

- (void)focusAtPoint:(CGPoint)point;
- (void)exposureAtPoint:(CGPoint)point;

- (BOOL)canSetCameraPosition:(AVCaptureDevicePosition)position ForSessionPreset:(NSString*)preset;
- (void)setCameraPosition:(AVCaptureDevicePosition)position WithSessionPreset:(NSString*)preset;
- (void)rotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation;
- (CGSize)videoResolutionForSessionPreset:(NSString*)preset;

- (BOOL)setUpAndStartCaptureSession;
- (void)stopAndShutDownCaptureSession;

- (void)addPreview:(UIView*)view;
- (void)removePreview;

@end
