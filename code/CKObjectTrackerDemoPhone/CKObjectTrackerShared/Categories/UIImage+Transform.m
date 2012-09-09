//
//  UIImage+Scaling.m
//  CKObjectTrackerDemoPhone
//
//  Created by Christoph Kapffer on 08.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import "UIImage+Scaling.h"

@implementation UIImage (Scaling)

- (UIImage*)scaledImageWithSize:(CGSize)size
{
    return [self scaledImageWithSize:size KeepRatio:YES];
}

- (UIImage*)scaledImageWithSize:(CGSize)size KeepRatio:(BOOL)keepRatio
{
    if (keepRatio) {
        CGFloat scalingFactor = MIN(size.width / self.size.width, size.height / self.size.height);
        size = CGSizeMake(self.size.width * scalingFactor, self.size.height * scalingFactor);
    }
    // Create a bitmap graphics context
    // This will also set it as the current context
    UIGraphicsBeginImageContext(size);
    
    // Draw the scaled image in the current context
    [self drawInRect:CGRectMake(0, 0, size.width, size.height)];
    
    // Create a new image from current context
    UIImage* scaledImage = UIGraphicsGetImageFromCurrentImageContext();
    
    // Pop the current context from the stack
    UIGraphicsEndImageContext();
    
    // Return our new scaled image
    return scaledImage;
}

@end
