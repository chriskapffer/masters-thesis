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

@property (nonatomic, assign) CVPixelBufferRef background;
@property (nonatomic, assign) BOOL isObjectPresent;

- (void)setObjectRotation:(Matrix3x3)rotation;
- (void)setObjectTranslation:(CGPoint)translation;
- (void)setObjectScale:(CGFloat)scale;

@end
