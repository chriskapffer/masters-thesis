//
//  FPSCalcApple.m
//  CKObjectTrackerShared
//
//  Created by Christoph Kapffer on 14.06.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import "FPSCalcApple.h"

@interface FPSCalcApple ()

@property (nonatomic, strong) NSMutableArray* previousTimestamps;

@end

@implementation FPSCalcApple

@synthesize previousTimestamps;

- (id)init
{
    self = [super init];
    if (self) {
        previousTimestamps = [[NSMutableArray alloc] init];
    }
    return self;
}

// this code is taken from http://developer.apple.com/library/ios/#samplecode/RosyWriter/Listings/Classes_RosyWriterVideoProcessor_m.html#//apple_ref/doc/uid/DTS40011110-Classes_RosyWriterVideoProcessor_m-DontLinkElementID_8
- (void) calculateFramerateAtTimestamp:(CMTime) timestamp
{
	[previousTimestamps addObject:[NSValue valueWithCMTime:timestamp]];
    
	CMTime oneSecond = CMTimeMake(1, 1);
	CMTime oneSecondAgo = CMTimeSubtract(timestamp, oneSecond);
    
	while (CMTIME_COMPARE_INLINE([[previousTimestamps objectAtIndex:0] CMTimeValue], <, oneSecondAgo))
		[previousTimestamps removeObjectAtIndex:0];
    
	float newRate = [previousTimestamps count];
	[self setFramesPerSecond:(self.framesPerSecond + newRate) / 2.0];
}

@end
