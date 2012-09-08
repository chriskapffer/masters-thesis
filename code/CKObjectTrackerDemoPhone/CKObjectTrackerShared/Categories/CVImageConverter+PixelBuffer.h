//
//  CVImageConverter+PixelBuffer.h
//  CKObjectTrackerDemoPhone
//
//  Created by Christoph Kapffer on 07.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import "CVImageConverter.h"

@interface CVImageConverter (PixelBuffer)

+ (void) CVMat:(cv::Mat&)cv_image FromCVPixelBuffer:(CVPixelBufferRef)cv_pxBuffer error:(NSError **)outError;
+ (void) CVMat:(cv::Mat&)cv_image FromCVPixelBuffer:(CVPixelBufferRef)cv_pxBuffer error:(NSError **)outError stripAlpha:(BOOL)stripAlpha;

@end
