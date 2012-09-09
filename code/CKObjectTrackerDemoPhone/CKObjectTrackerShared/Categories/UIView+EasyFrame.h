//
//  UIView+EasyFrame.h
//  CKObjectTrackerDemoPhone
//
//  Created by Christoph Kapffer on 09.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface UIView (EasyFrame)

- (void)setFrameOrigin:(CGPoint)origin;
- (void)setFrameOriginX:(CGFloat)x;
- (void)setFrameOriginY:(CGFloat)y;

- (void)setFrameSize:(CGSize)size;
- (void)setFrameSizeWidth:(CGFloat)width;
- (void)setFrameSizeHeight:(CGFloat)height;

- (void)addToFrameOrigin:(CGPoint)origin;
- (void)addToFrameOriginX:(CGFloat)x;
- (void)addToFrameOriginY:(CGFloat)y;

- (void)addToFrameSize:(CGSize)size;
- (void)addToFrameSizeWidth:(CGFloat)width;
- (void)addToFrameSizeHeight:(CGFloat)height;

@end
