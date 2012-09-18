//
//  BackgroundTexture.h
//  CKObjectTrackerDemoPhone
//
//  Created by Christoph Kapffer on 13.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface BackgroundTexture : NSObject

@property (nonatomic, assign) CGSize viewPortSize;
@property (nonatomic, assign) CGSize textureSize;

- (id)initWithContext:(EAGLContext*)context;
- (void)updateContent:(CVPixelBufferRef)content;
- (void)draw;

@end
