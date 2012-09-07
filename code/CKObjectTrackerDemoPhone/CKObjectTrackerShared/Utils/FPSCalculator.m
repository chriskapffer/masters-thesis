//
//  FPSCalculator.m
//  CKObjectTrackerShared
//
//  Created by Christoph Kapffer on 14.06.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import "FPSCalculator.h"

@implementation FPSCalculator

@synthesize framesPerSecond;

- (void)calculateFramerateAtTimestamp:(CMTime)timestamp
{
    // implementation in subclass
}

@end
