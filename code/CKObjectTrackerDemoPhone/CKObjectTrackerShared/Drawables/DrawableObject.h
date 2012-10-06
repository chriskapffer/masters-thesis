//
//  DrawableObject.h
//  CKObjectTrackerDemoPhone
//
//  Created by Christoph Kapffer on 12.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <GLKit/GLKit.h>

@interface DrawableObject : NSObject

- (id)initCube;
- (id)initTeapot;

- (void)drawWithEffect:(GLKBaseEffect*)effect;

@end
