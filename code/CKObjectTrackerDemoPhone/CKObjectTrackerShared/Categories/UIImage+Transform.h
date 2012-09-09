//
//  UIImage+Scaling.h
//  CKObjectTrackerDemoPhone
//
//  Created by Christoph Kapffer on 08.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface UIImage (Scaling)

- (UIImage*)scaledImageWithSize:(CGSize)size;
- (UIImage*)scaledImageWithSize:(CGSize)size KeepRatio:(BOOL)keepRatio;

@end
