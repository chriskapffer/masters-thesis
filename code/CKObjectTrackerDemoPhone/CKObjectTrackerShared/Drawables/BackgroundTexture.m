//
//  BackgroundTexture.m
//  CKObjectTrackerDemoPhone
//
//  Created by Christoph Kapffer on 13.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import "BackgroundTexture.h"
#import "ShaderUtils.h"

#define VERTEX_COUNT 8

// Uniform index.
enum
{
    UNIFORM_VIDEO_FRAME,
    NUM_UNIFORMS
};

// Attribute index.
enum {
    ATTRIB_VERTEX,
    ATTRIB_TEXCOORD,
    NUM_ATTRIBUTES
};

static const GLfloat gQuadVertices[VERTEX_COUNT] = {
    -1.0f, -1.0f,
    1.0f, -1.0f,
    -1.0f,  1.0f,
    1.0f,  1.0f,
};



@interface BackgroundTexture ()
{
    GLint uniforms[NUM_UNIFORMS];
    GLfloat _textureVertices[VERTEX_COUNT];
    GLuint _program;
    
    GLuint _vertexArray;
    GLuint _positionVBO;
    GLuint _texcoordVBO;
    
    CVOpenGLESTextureRef _backgroundTexture;
    CVOpenGLESTextureCacheRef _videoTextureCache;
}

@end

@implementation BackgroundTexture

#pragma mark - properties

@synthesize viewPortSize = _viewPortSize;
@synthesize textureSize = _textureSize;

- (void)setViewPortSize:(CGSize)viewPortSize
{
    if (viewPortSize.width == _viewPortSize.width && viewPortSize.height == _viewPortSize.height)
        return;
    
    _viewPortSize.width = viewPortSize.width;
    _viewPortSize.height = viewPortSize.height;

    [self updateTextureVertices];
}

- (void)setTextureSize:(CGSize)textureSize
{
    if (textureSize.width == _textureSize.width && textureSize.height == _textureSize.height)
        return;
    
    _textureSize.width = textureSize.width;
    _textureSize.height = textureSize.height;
        
    [self updateTextureVertices];
}

#pragma mark



- (id)initWithContext:(EAGLContext*)context
{
    self = [super init];
    if (self) {
        _textureSize = CGSizeMake(-1, -1);
        
        CVReturn error;
        error = CVOpenGLESTextureCacheCreate(kCFAllocatorDefault, NULL, (__bridge void *)context, NULL, &_videoTextureCache);
        if (error) {
            NSLog(@"Error at CVOpenGLESTextureCacheCreate %d", error);
            return nil;
        }
        
        [self loadShaders];

        glUseProgram(_program);
        glUniform1i(uniforms[UNIFORM_VIDEO_FRAME], 0);
        
        glGenBuffers(1, &_positionVBO);
        glGenBuffers(1, &_texcoordVBO);
        
//        glGenVertexArraysOES(1, &_vertexArray);
//        glBindVertexArrayOES(_vertexArray);
//        
//        glGenBuffers(1, &_positionVBO);
//        glBindBuffer(GL_ARRAY_BUFFER, _positionVBO);
//        glBufferData(GL_ARRAY_BUFFER, sizeof(gQuadVertices), gQuadVertices, GL_DYNAMIC_DRAW);
//
//        glBindVertexArrayOES(0);
    }
    return self;
}

- (void)dealloc
{
    glDeleteBuffers(1, &_positionVBO);
    glDeleteBuffers(1, &_texcoordVBO);
    
    glDeleteVertexArraysOES(1, &_vertexArray);
    
    [self cleanUpTextures];
    
    CFRelease(_videoTextureCache);
    
    if (_program) {
        glDeleteProgram(_program);
        _program = 0;
    }
}

- (void)loadShaders
{
    NSString* vertShaderPath = [[NSBundle mainBundle] pathForResource:@"passThrough" ofType:@"vsh"];
    NSString* fragShaderPath = [[NSBundle mainBundle] pathForResource:@"passThrough" ofType:@"fsh"];
    NSDictionary* attributes = [NSDictionary dictionaryWithObjectsAndKeys:
                                @"position", [NSNumber numberWithUnsignedInt:ATTRIB_VERTEX],
                                @"texCoord", [NSNumber numberWithUnsignedInt:ATTRIB_TEXCOORD], nil];
    if (![ShaderUtils createProgram:&_program WithVertexShaderPath:vertShaderPath FragmentShaderPath:fragShaderPath Attributes:attributes]) {
        NSLog(@"Failed to load shaders.");
        return;
    }
    
    // Get uniform locations.
    uniforms[0] = glGetUniformLocation(_program, "Videoframe");
}

- (void)initBuffers
{    
    glBindBuffer(GL_ARRAY_BUFFER, _positionVBO);
    glBufferData(GL_ARRAY_BUFFER, VERTEX_COUNT * sizeof(GLfloat), gQuadVertices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(ATTRIB_VERTEX);
    glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);

    glBindBuffer(GL_ARRAY_BUFFER, _texcoordVBO);
    glBufferData(GL_ARRAY_BUFFER, VERTEX_COUNT * sizeof(GLfloat), _textureVertices, GL_DYNAMIC_DRAW);
    
    glEnableVertexAttribArray(ATTRIB_TEXCOORD);
    glVertexAttribPointer(ATTRIB_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
}

- (void)cleanUpTextures
{
    if (_backgroundTexture)
    {
        CFRelease(_backgroundTexture);
        _backgroundTexture = NULL;
    }
    // Periodic texture cache flush every time content changes
    CVOpenGLESTextureCacheFlush(_videoTextureCache, 0);
}

- (void)draw
{
//    glBindVertexArrayOES(_vertexArray);
//
//    glUseProgram(_program);
//    glUniform1i(uniforms[UNIFORM_VIDEO_FRAME], 0);
//    
    //[self initBuffers];

    
    

    
    glUseProgram(_program);
    glUniform1i(uniforms[UNIFORM_VIDEO_FRAME], 0);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
//    glBindVertexArrayOES(0);
}

- (void)updateContent:(CVPixelBufferRef)pixelBuffer
{
    if (!_videoTextureCache) {
        NSLog(@"No video texture cache");
        return;
    }
    
    CVReturn error;
    size_t width = CVPixelBufferGetWidth(pixelBuffer);
    size_t height = CVPixelBufferGetHeight(pixelBuffer);
    
    self.textureSize = CGSizeMake(width, height);
    
    [self cleanUpTextures];
    glActiveTexture(GL_TEXTURE0);
    error = CVOpenGLESTextureCacheCreateTextureFromImage(kCFAllocatorDefault,
                                                         _videoTextureCache,
                                                         pixelBuffer,
                                                         NULL,
                                                         GL_TEXTURE_2D,
                                                         GL_RGBA,
                                                         width,
                                                         height,
                                                         GL_BGRA,
                                                         GL_UNSIGNED_BYTE,
                                                         0,
                                                         &_backgroundTexture);
    if (error) {
        NSLog(@"Error at CVOpenGLESTextureCacheCreateTextureFromImage %d", error);
    }
    
    glBindTexture(CVOpenGLESTextureGetTarget(_backgroundTexture), CVOpenGLESTextureGetName(_backgroundTexture));
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

- (void)updateTextureVertices
{
    CGSize textureSize = self.textureSize;
    CGRect textureSamplingRect = [self textureSamplingRectForCroppingTextureWithAspectRatio:textureSize
                                                                              toAspectRatio:self.viewPortSize];
    GLfloat tmp[VERTEX_COUNT] = {
        CGRectGetMaxX(textureSamplingRect), CGRectGetMaxY(textureSamplingRect), // 3
        CGRectGetMaxX(textureSamplingRect), CGRectGetMinY(textureSamplingRect), // 1
        CGRectGetMinX(textureSamplingRect), CGRectGetMaxY(textureSamplingRect), // 2
        CGRectGetMinX(textureSamplingRect), CGRectGetMinY(textureSamplingRect), // 0
    };
    memcpy(_textureVertices, tmp, sizeof(tmp));
    [self initBuffers];
}

// code from: https://developer.apple.com/library/ios/samplecode/RosyWriter/Listings/Classes_RosyWriterPreviewView_m.html#//apple_ref/doc/uid/DTS40011110-Classes_RosyWriterPreviewView_m-DontLinkElementID_6
- (CGRect)textureSamplingRectForCroppingTextureWithAspectRatio:(CGSize)textureAspectRatio toAspectRatio:(CGSize)croppingAspectRatio
{
	CGRect normalizedSamplingRect = CGRectZero;
	CGSize cropScaleAmount = CGSizeMake(croppingAspectRatio.width / textureAspectRatio.width, croppingAspectRatio.height / textureAspectRatio.height);
	CGFloat minScale = fmin(cropScaleAmount.width, cropScaleAmount.height);
	CGSize scaledTextureSize = CGSizeMake(textureAspectRatio.width * minScale, textureAspectRatio.height * minScale);
	
	if ( cropScaleAmount.height > cropScaleAmount.width ) {
		normalizedSamplingRect.size.width = croppingAspectRatio.width / scaledTextureSize.width;
		normalizedSamplingRect.size.height = 1.0;
	}
	else {
		normalizedSamplingRect.size.height = croppingAspectRatio.height / scaledTextureSize.height;
		normalizedSamplingRect.size.width = 1.0;
	}
	// Center crop
	normalizedSamplingRect.origin.x = (1.0 - normalizedSamplingRect.size.width)/2.0;
	normalizedSamplingRect.origin.y = (1.0 - normalizedSamplingRect.size.height)/2.0;
	
	return normalizedSamplingRect;
}

@end
