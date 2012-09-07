//
//  VideoReader.h
//  CKObjectTrackerTest
//
//  Created by Christoph Kapffer on 10.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import <Foundation/Foundation.h>

@protocol VideoReaderDelegate <NSObject>
- (void)didReadFrameWithPixelBuffer:(CVPixelBufferRef)pixelBuffer;
@end

@interface VideoReader : NSObject

@property (nonatomic, strong) id<VideoReaderDelegate> delegate;
@property (nonatomic, assign, readonly) float framesPerSecond;

- (void)readVideoWithURL:(NSURL*)videoURL Completion:(void (^)(void))completion;
- (void)stopReading;

@end
