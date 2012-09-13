//
//  BackgroundTexture.m
//  CKObjectTrackerDemoPhone
//
//  Created by Christoph Kapffer on 13.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import "BackgroundTexture.h"
#import "ShaderUtils.h"

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

static const GLfloat squareVertices[] = {
    -1.0f, -1.0f,
    1.0f, -1.0f,
    -1.0f,  1.0f,
    1.0f,  1.0f,
};

@interface BackgroundTexture ()
{
    GLint uniforms[NUM_UNIFORMS];
    GLfloat textureVertices[8];
    GLuint _program;
    
    GLuint _positionVBO;
    GLuint _indexVBO;
    
    CVOpenGLESTextureRef _backgroundTexture;
    CVOpenGLESTextureCacheRef _videoTextureCache;
    
    size_t _textureWidth;
    size_t _textureHeight;
}

@end

@implementation BackgroundTexture

#pragma mark - properties

@synthesize viewPortSize = _viewPortSize;


- (void)setTextureSize:(CGSize)textureSize
{
    if (textureSize.width == _textureWidth && textureSize.height == _textureHeight)
        return;
    
    _textureWidth = textureSize.width;
    _textureHeight = textureSize.height;
        
        CGRect textureSamplingRect = [self textureSamplingRectForCroppingTextureWithAspectRatio:textureSize toAspectRatio:self.viewPortSize];
        GLfloat tmp[] = {
            CGRectGetMaxX(textureSamplingRect), CGRectGetMaxY(textureSamplingRect), // 3
            CGRectGetMaxX(textureSamplingRect), CGRectGetMinY(textureSamplingRect), // 1
            CGRectGetMinX(textureSamplingRect), CGRectGetMaxY(textureSamplingRect), // 2
            CGRectGetMinX(textureSamplingRect), CGRectGetMinY(textureSamplingRect), // 0
        };
        memcpy(textureVertices, tmp, sizeof(tmp));
        [self initBuffers];
}

- (void)setContent:(CVPixelBufferRef)content
{

}

#pragma mark

- (id)initWithContext:(EAGLContext*)context
{
    self = [super init];
    if (self) {
        _textureWidth = -1;
        _textureHeight = -1;
        
        CVReturn err = CVOpenGLESTextureCacheCreate(kCFAllocatorDefault, NULL, (__bridge void *)context, NULL, &_videoTextureCache);
        if (err)
        {
            NSLog(@"Error at CVOpenGLESTextureCacheCreate %d", err);
            return nil;
        }
    }
    return self;
}

- (void)dealloc
{
    
    glDeleteBuffers(1, &_positionVBO);
    glDeleteBuffers(1, &_indexVBO);
    
    [self cleanUpTextures];
    CFRelease(_videoTextureCache);
    
    if (_program) {
        glDeleteProgram(_program);
        _program = 0;
    }
}

- (void)initBuffers
{
    glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, 0, 0, squareVertices);
	glEnableVertexAttribArray(ATTRIB_VERTEX);
	glVertexAttribPointer(ATTRIB_TEXCOORD, 2, GL_FLOAT, 0, 0, textureVertices);
	glEnableVertexAttribArray(ATTRIB_TEXCOORD);
}

- (void)cleanUpTextures
{
    if (_backgroundTexture)
    {
        CFRelease(_backgroundTexture);
        _backgroundTexture = NULL;
    }
    // Periodic texture cache flush every frame
    CVOpenGLESTextureCacheFlush(_videoTextureCache, 0);
}


- (void)updateContent:(CVPixelBufferRef)pixelBuffer
{
    CVReturn error;
    size_t width = CVPixelBufferGetWidth(pixelBuffer);
    size_t height = CVPixelBufferGetHeight(pixelBuffer);
    
    if (!_videoTextureCache)
    {
        NSLog(@"No video texture cache");
        return;
    }
    
    self.viewPortSize = self.viewPortSize;
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
    if (error)
    {
        NSLog(@"Error at CVOpenGLESTextureCacheCreateTextureFromImage %d", error);
    }
    
    glBindTexture(CVOpenGLESTextureGetTarget(_backgroundTexture), CVOpenGLESTextureGetName(_backgroundTexture));
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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
    
    
    
    
    glUseProgram(_program);
    
    glUniform1i(uniforms[UNIFORM_VIDEO_FRAME], 0);
}

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
