//
//  ARViewController.h
//  CKObjectTrackerDemoPhone
//
//  Created by Christoph Kapffer on 12.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

@interface ARViewController : GLKViewController

@property (nonatomic, assign) GLKMatrix3 intrinsics;
@property (nonatomic, assign) GLKMatrix3 homography;
@property (nonatomic, assign) CVPixelBufferRef background;
@property (nonatomic, assign) BOOL isObjectPresent;

@end
