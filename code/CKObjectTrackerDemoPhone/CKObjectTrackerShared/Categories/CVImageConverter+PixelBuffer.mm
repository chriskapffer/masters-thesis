//
//  CVImageConverter+PixelBuffer.m
//  CKObjectTrackerDemoPhone
//
//  Created by Christoph Kapffer on 07.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import "CVImageConverter+PixelBuffer.h"

@implementation CVImageConverter (PixelBuffer)

+ (void) CVMat:(cv::Mat&)cv_image FromCVPixelBuffer:(CVPixelBufferRef)cv_pxBuffer error:(NSError **)outError
{
    size_t width = CVPixelBufferGetWidth(cv_pxBuffer);
    size_t height = CVPixelBufferGetHeight(cv_pxBuffer);
    OSType format = CVPixelBufferGetPixelFormatType(cv_pxBuffer);
    void *baseAddress;
    int type;
    
    CVPixelBufferLockBaseAddress(cv_pxBuffer, 0);
    if (format == kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange || format == kCVPixelFormatType_422YpCbCr8) {
        baseAddress = CVPixelBufferGetBaseAddressOfPlane(cv_pxBuffer, 0);
        type = CV_8UC1;
    } else if (format == kCVPixelFormatType_32BGRA) {
        baseAddress = CVPixelBufferGetBaseAddress(cv_pxBuffer);
        type = CV_8UC4;
    } else {
        CVPixelBufferUnlockBaseAddress(cv_pxBuffer, 0);
        NSString* formatString = [NSString stringWithUTF8String:(char*)format];
        NSString* description = [NSString stringWithFormat:@"Unsupported format: %@", formatString];
        NSDictionary *userInfo = @{ NSLocalizedDescriptionKey : description };
        *outError = [[NSError alloc] initWithDomain:@"CVImageConversionError" code:1 userInfo:userInfo];
        return;
    }
    
    cv_image = cv::Mat(height, width, type, baseAddress);
    CVPixelBufferUnlockBaseAddress(cv_pxBuffer, 0);
}

@end
