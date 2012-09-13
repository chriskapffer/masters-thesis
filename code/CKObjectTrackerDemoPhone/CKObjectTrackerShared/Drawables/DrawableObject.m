//
//  DrawableObject.m
//  CKObjectTrackerDemoPhone
//
//  Created by Christoph Kapffer on 12.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import "DrawableObject.h"
#import <GLKit/GLKit.h>

const Vertex objVertices[] = {
    // Front < Position >    < Normal >    < Color >  < Texture >
    {{ 0.5f, -0.5f,  0.5f}, { 0,  0, -1}, {1, 0, 0, 1}, {1, 0}},
    {{ 0.5f,  0.5f,  0.5f}, { 0,  0, -1}, {0, 1, 0, 1}, {1, 1}},
    {{-0.5f,  0.5f,  0.5f}, { 0,  0, -1}, {0, 0, 1, 1}, {0, 1}},
    {{-0.5f, -0.5f,  0.5f}, { 0,  0, -1}, {0, 0, 0, 1}, {0, 0}},
    // Back
    {{ 0.5f,  0.5f, -0.5f}, { 0,  0,  1}, {1, 0, 0, 1}, {0, 1}},
    {{-0.5f, -0.5f, -0.5f}, { 0,  0,  1}, {0, 1, 0, 1}, {1, 0}},
    {{ 0.5f, -0.5f, -0.5f}, { 0,  0,  1}, {0, 0, 1, 1}, {0, 0}},
    {{-0.5f,  0.5f, -0.5f}, { 0,  0,  1}, {0, 0, 0, 1}, {1, 1}},
    // Left
    {{-0.5f, -0.5f,  0.5f}, {-1,  0,  0}, {1, 0, 0, 1}, {1, 0}},
    {{-0.5f,  0.5f,  0.5f}, {-1,  0,  0}, {0, 1, 0, 1}, {1, 1}},
    {{-0.5f,  0.5f, -0.5f}, {-1,  0,  0}, {0, 0, 1, 1}, {0, 1}},
    {{-0.5f, -0.5f, -0.5f}, {-1,  0,  0}, {0, 0, 0, 1}, {0, 0}},
    // Right
    {{ 0.5f, -0.5f, -0.5f}, { 1,  0,  0}, {1, 0, 0, 1}, {1, 0}},
    {{ 0.5f,  0.5f, -0.5f}, { 1,  0,  0}, {0, 1, 0, 1}, {1, 1}},
    {{ 0.5f,  0.5f,  0.5f}, { 1,  0,  0}, {0, 0, 1, 1}, {0, 1}},
    {{ 0.5f, -0.5f,  0.5f}, { 1,  0,  0}, {0, 0, 0, 1}, {0, 0}},
    // Top
    {{ 0.5f,  0.5f,  0.5f}, { 0,  1,  0}, {1, 0, 0, 1}, {1, 0}},
    {{ 0.5f,  0.5f, -0.5f}, { 0,  1,  0}, {0, 1, 0, 1}, {1, 1}},
    {{-0.5f,  0.5f, -0.5f}, { 0,  1,  0}, {0, 0, 1, 1}, {0, 1}},
    {{-0.5f,  0.5f,  0.5f}, { 0,  1,  0}, {0, 0, 0, 1}, {0, 0}},
    // Bottom
    {{ 0.5f, -0.5f, -0.5f}, { 0, -1,  0}, {1, 0, 0, 1}, {1, 0}},
    {{ 0.5f, -0.5f,  0.5f}, { 0, -1,  0}, {0, 1, 0, 1}, {1, 1}},
    {{-0.5f, -0.5f,  0.5f}, { 0, -1,  0}, {0, 0, 1, 1}, {0, 1}},
    {{-0.5f, -0.5f, -0.5f}, { 0, -1,  0}, {0, 0, 0, 1}, {0, 0}}
};

const GLubyte objIndices[] = {
    // Front
    0, 1, 2,
    2, 3, 0,
    // Back
    4, 6, 5,
    4, 5, 7,
    // Left
    8, 9, 10,
    10, 11, 8,
    // Right
    12, 13, 14,
    14, 15, 12,
    // Top
    16, 17, 18,
    18, 19, 16,
    // Bottom
    20, 21, 22,
    22, 23, 20
};

@interface DrawableObject ()
{
    GLuint _vertexBuffer;
    GLuint _indexBuffer;
}

@property (nonatomic, strong) GLKTextureInfo* textureInfo;

@end

@implementation DrawableObject

@synthesize textureInfo = _textureInfo;
@synthesize textureName = _textureName;

- (GLuint)textureName
{
    return self.textureInfo.name;
}

- (id)init
{
    self = [super init];
    if (self) {
        
        NSDictionary * options = [NSDictionary dictionaryWithObjectsAndKeys:
                                  [NSNumber numberWithBool:YES],
                                  GLKTextureLoaderOriginBottomLeft,
                                  nil];
        
        NSError * error;
        NSString *path = [[NSBundle mainBundle] pathForResource:@"tile_floor" ofType:@"png"];
        self.textureInfo = [GLKTextureLoader textureWithContentsOfFile:path options:options error:&error];
        if (self.textureInfo == nil) {
            NSLog(@"Error loading file: %@", [error localizedDescription]);
        }

    }
    return self;
}

- (void)dealloc
{
    // clear buffers
    glDeleteBuffers(1, &_vertexBuffer);
    glDeleteBuffers(1, &_indexBuffer);
}

- (void)draw
{
    // set up buffers
    glGenBuffers(1, &_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(objVertices), objVertices, GL_STATIC_DRAW);
    
    glGenBuffers(1, &_indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(objIndices), objIndices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(GLKVertexAttribPosition);
    glVertexAttribPointer(GLKVertexAttribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *) offsetof(Vertex, Position));
    glEnableVertexAttribArray(GLKVertexAttribNormal);
    glVertexAttribPointer(GLKVertexAttribNormal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *) offsetof(Vertex, Normal));
    glEnableVertexAttribArray(GLKVertexAttribColor);
    glVertexAttribPointer(GLKVertexAttribColor, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *) offsetof(Vertex, Color));
    glEnableVertexAttribArray(GLKVertexAttribTexCoord0);
    glVertexAttribPointer(GLKVertexAttribTexCoord0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *) offsetof(Vertex, TexCoord));
    
    
    glDrawElements(GL_TRIANGLES, sizeof(objIndices)/sizeof(objIndices[0]), GL_UNSIGNED_BYTE, 0);
}


@end
