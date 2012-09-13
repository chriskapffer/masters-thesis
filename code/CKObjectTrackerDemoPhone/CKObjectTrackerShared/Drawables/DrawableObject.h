//
//  DrawableObject.h
//  CKObjectTrackerDemoPhone
//
//  Created by Christoph Kapffer on 12.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef struct {
    float Position[3];
    float Normal[3];
    float Color[4];
    float TexCoord[2];
} Vertex;

@interface DrawableObject : NSObject

@property (nonatomic, assign) GLuint textureName;

- (void)draw;

@end
