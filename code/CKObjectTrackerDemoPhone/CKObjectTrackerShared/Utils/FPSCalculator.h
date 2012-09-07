//
//  FPSCalculator.h
//  CKObjectTrackerShared
//
//  Created by Christoph Kapffer on 14.06.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

@interface FPSCalculator : NSObject

@property (nonatomic, assign) float framesPerSecond;

- (void)calculateFramerateAtTimestamp:(CMTime)timestamp;

@end
