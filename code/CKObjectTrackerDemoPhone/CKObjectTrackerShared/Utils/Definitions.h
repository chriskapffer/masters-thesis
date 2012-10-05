//
//  Definitions.h
//  CKObjectTrackerDemoPhone
//
//  Created by Christoph Kapffer on 08.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#define trace() NSLog(@"%s", __PRETTY_FUNCTION__)

typedef struct {
    CGFloat m00, m01, m02;
    CGFloat m10, m11, m12;
    CGFloat m20, m21, m22;
} Matrix3x3;