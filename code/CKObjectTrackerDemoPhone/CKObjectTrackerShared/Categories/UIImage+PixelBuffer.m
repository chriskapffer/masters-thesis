//
//  UIImage+PixelBuffer.m
//  CKObjectTrackerDemoPhone
//
//  Created by Christoph Kapffer on 09.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import "UIImage+PixelBuffer.h"

@implementation UIImage (PixelBuffer)


// from http://developer.apple.com/library/ios/DOCUMENTATION/AudioVideo/Conceptual/AVFoundationPG/Articles/05_MediaRepresentations.html#//apple_ref/doc/uid/TP40010188-CH2-SW4
+ (UIImage*)imageFromPixelBuffer:(CVPixelBufferRef)pixelBuffer
{
    // Lock the base address of the pixel buffer.
    CVPixelBufferLockBaseAddress(pixelBuffer,0);
    
    // Get the number of bytes per row for the pixel buffer.
    size_t bytesPerRow = CVPixelBufferGetBytesPerRow(pixelBuffer);
    // Get the pixel buffer width and height.
    size_t width = CVPixelBufferGetWidth(pixelBuffer);
    size_t height = CVPixelBufferGetHeight(pixelBuffer);
    
    // Create a device-dependent RGB color space.
    static CGColorSpaceRef colorSpace = NULL;
    if (colorSpace == NULL) {
        colorSpace = CGColorSpaceCreateDeviceRGB();
        if (colorSpace == NULL) {
            // Handle the error appropriately.
            return nil;
        }
    }
    
    // Get the base address of the pixel buffer.
    void *baseAddress = CVPixelBufferGetBaseAddress(pixelBuffer);
    // Get the data size for contiguous planes of the pixel buffer.
    size_t bufferSize = CVPixelBufferGetDataSize(pixelBuffer);
    
    // Create a Quartz direct-access data provider that uses data we supply.
    CGDataProviderRef dataProvider =
    CGDataProviderCreateWithData(NULL, baseAddress, bufferSize, NULL);
    // Create a bitmap image from data supplied by the data provider.
    CGImageRef cgImage =
    CGImageCreate(width, height, 8, 32, bytesPerRow,
                  colorSpace, kCGImageAlphaNoneSkipFirst | kCGBitmapByteOrder32Little,
                  dataProvider, NULL, true, kCGRenderingIntentDefault);
    CGDataProviderRelease(dataProvider);
    
    // Create and return an image object to represent the Quartz image.
    UIImage *image = [UIImage imageWithCGImage:cgImage];
    CGImageRelease(cgImage);
    
    CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
    
    return image;
}

@end
