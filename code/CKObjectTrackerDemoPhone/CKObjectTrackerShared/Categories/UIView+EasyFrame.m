//
//  UIView+EasyFrame.m
//  CKObjectTrackerDemoPhone
//
//  Created by Christoph Kapffer on 09.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import "UIView+EasyFrame.h"

@implementation UIView (EasyFrame)

- (void)setFrameOrigin:(CGPoint)origin
{
    CGRect tmp = self.frame;
    tmp.origin = origin;
    self.frame = tmp;
}

- (void)setFrameOriginX:(CGFloat)x
{
    CGRect tmp = self.frame;
    tmp.origin.x = x;
    self.frame = tmp;
}

- (void)setFrameOriginY:(CGFloat)y
{
    CGRect tmp = self.frame;
    tmp.origin.y = y;
    self.frame = tmp;
}

- (void)setFrameSize:(CGSize)size
{
    CGRect tmp = self.frame;
    tmp.size = size;
    self.frame = tmp;
}

- (void)setFrameSizeWidth:(CGFloat)width
{
    CGRect tmp = self.frame;
    tmp.size.width = width;
    self.frame = tmp;
}

- (void)setFrameSizeHeight:(CGFloat)height
{
    CGRect tmp = self.frame;
    tmp.size.height = height;
    self.frame = tmp;
}

- (void)addToFrameOrigin:(CGPoint)origin
{
    CGRect tmp = self.frame;
    tmp.origin = CGPointMake(tmp.origin.x + origin.x, tmp.origin.y + origin.y);
    self.frame = tmp;
}

- (void)addToFrameOriginX:(CGFloat)x
{
    CGRect tmp = self.frame;
    tmp.origin.x += x;
    self.frame = tmp;
}

- (void)addToFrameOriginY:(CGFloat)y
{
    CGRect tmp = self.frame;
    tmp.origin.y += y;
    self.frame = tmp;
}

- (void)addToFrameSize:(CGSize)size
{
    CGRect tmp = self.frame;
    tmp.size = CGSizeMake(tmp.size.width + size.width, tmp.size.height + size.height);
    self.frame = tmp;
}

- (void)addToFrameSizeWidth:(CGFloat)width
{
    CGRect tmp = self.frame;
    tmp.size.width += width;
    self.frame = tmp;
}

- (void)addToFrameSizeHeight:(CGFloat)height
{
    CGRect tmp = self.frame;
    tmp.size.height += height;
    self.frame = tmp;
}

@end
