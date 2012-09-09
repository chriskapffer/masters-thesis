//
//  UIImage+Transform.m
//  CKObjectTrackerDemoPhone
//
//  Created by Christoph Kapffer on 08.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import "UIImage+Transform.h"

@implementation UIImage (Transform)

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

// from: http://stackoverflow.com/a/5017568/782862
- (UIImage*)rotatedImageWithAngle:(float)radians
{
    UIView* rotatedViewBox = [[UIView alloc] initWithFrame: CGRectMake(0, 0, self.size.width, self.size.height)];
    rotatedViewBox.transform = CGAffineTransformMakeRotation(radians);
    CGSize rotatedSize = rotatedViewBox.frame.size;
    
    UIGraphicsBeginImageContext(rotatedSize);
    CGContextRef bitmap = UIGraphicsGetCurrentContext();
    CGContextTranslateCTM(bitmap, rotatedSize.width/2, rotatedSize.height/2);
    CGContextRotateCTM(bitmap, radians);
    
    CGRect targetRect = CGRectMake(-self.size.width / 2, -self.size.height / 2, self.size.width, self.size.height);
    CGContextScaleCTM(bitmap, 1.0, -1.0);
    CGContextDrawImage(bitmap, targetRect, [self CGImage]);
    
    UIImage* rotatedImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    
    return rotatedImage;
}

@end
