//
//  VideoReader.m
//  CKObjectTrackerTest
//
//  Created by Christoph Kapffer on 10.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import "VideoReader.h"

#import <AVFoundation/AVFoundation.h>

#import "FPSCalcApple.h"
#import "FPSCalcSummerhill.h"

@interface VideoReader ()

@property (nonatomic, strong) AVAssetReader* assetReader;
@property (nonatomic, strong) FPSCalculator* fpsCalculator;
@property (nonatomic, assign) dispatch_queue_t readerQueue;
@property (nonatomic, assign) BOOL stopRequested;

@end

@implementation VideoReader

#pragma mark - properties

@synthesize delegate = _delegate;
@synthesize assetReader = _assetReader;
@synthesize fpsCalculator = _fpsCalculator;
@synthesize readerQueue = _readerQueue;
@synthesize stopRequested = _stopRequested;

- (float)framesPerSecond
{
    return [self.fpsCalculator framesPerSecond];
}

#pragma mark - initialization

- (id)init
{
    self = [super init];
    if(self) {
        _readerQueue = dispatch_queue_create("ck.objecttracker.videoreader.processframes", DISPATCH_QUEUE_SERIAL);
        _fpsCalculator = [[FPSCalcApple alloc] init];
        _stopRequested = NO;
    }
    return self;
}

- (void)dealloc
{
    dispatch_release(_readerQueue);
}

#pragma mark - video reading

- (void)readVideoWithURL:(NSURL*)videoURL Completion:(void (^)(void))completion
{
    [self stopReading];
    
    NSString *tracksKey = @"tracks";
    AVURLAsset *asset = [AVURLAsset URLAssetWithURL:videoURL options:nil];
    [asset loadValuesAsynchronouslyForKeys:[NSArray arrayWithObject:tracksKey] completionHandler:
     ^{
         NSString* formatKey = (NSString*)kCVPixelBufferPixelFormatTypeKey;
         NSNumber* formatValue = [NSNumber numberWithUnsignedInt:kCVPixelFormatType_32BGRA];
         NSDictionary* videoSettings = [NSDictionary dictionaryWithObject:formatValue forKey:formatKey];
         
         NSError *error;
         AVKeyValueStatus status = [asset statusOfValueForKey:tracksKey error:&error];
         if (status != AVKeyValueStatusLoaded) {
             NSLog(@"The asset's tracks were not loaded:\n%@", [error localizedDescription]);
             return;
         }
         
         NSArray * tracks = [asset tracksWithMediaType:AVMediaTypeVideo];
         if (tracks.count < 1) {
             NSLog(@"There is no video track in file \"%@\"", [videoURL lastPathComponent]);
             return;
         }
         
         self.assetReader = [AVAssetReader assetReaderWithAsset:asset error:&error];
         if (error) {
             NSLog(@"Could not initialize asset reader:\n%@", error.localizedDescription);
             return;
         }
         
         AVAssetTrack* videoTrack = [tracks objectAtIndex:0];
         AVAssetReaderTrackOutput* trackOutput = [AVAssetReaderTrackOutput assetReaderTrackOutputWithTrack:videoTrack outputSettings:videoSettings];
         
         if (![self.assetReader canAddOutput:trackOutput]) {
             NSLog(@"Could not add track output to asset reader.");
             return;
         }
         
         [self.assetReader addOutput:trackOutput];
         [self.assetReader startReading];
         [self startProcessingWithCompletion:completion];
     }];
}

- (void)stopReading
{
    self.stopRequested = YES;
    dispatch_sync(self.readerQueue, ^{ NSLog(@"stop"); });
    self.stopRequested = NO;
}

- (void)startProcessingWithCompletion:(void (^)(void))completion
{
    dispatch_async(self.readerQueue, ^{
        while (!self.stopRequested && [self readNextFrame]) { }
        dispatch_sync(dispatch_get_main_queue(), ^{
            NSLog(@"finish");
            completion();
        });
    });
}

- (BOOL)readNextFrame
{
    if (self.assetReader.status != AVAssetReaderStatusReading)
        return NO;
    
    AVAssetReaderOutput* output = [self.assetReader.outputs objectAtIndex:0];
    CMSampleBufferRef sampleBuffer = [output copyNextSampleBuffer];
    if (!sampleBuffer)
        return NO;
    
    [self.fpsCalculator calculateFramerateAtTimestamp:CMSampleBufferGetPresentationTimeStamp(sampleBuffer)];
    CVPixelBufferRef pixelBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);

    OSType format = CVPixelBufferGetPixelFormatType(pixelBuffer);
    if (format == kCVPixelFormatType_32BGRA || format == kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange || format == kCVPixelFormatType_422YpCbCr8) {
        if ([self.delegate respondsToSelector:@selector(didReadFrameWithPixelBuffer:)]) {
                [self.delegate didReadFrameWithPixelBuffer:pixelBuffer];
        }
    } else {
        NSLog(@"Unsupported pixel format.");
    }
    
    CFRelease(sampleBuffer);
    return YES;
}

@end
