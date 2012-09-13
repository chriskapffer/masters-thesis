//
//  ARViewController.m
//  CKObjectTrackerDemoPhone
//
//  Created by Christoph Kapffer on 12.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import "ARViewController.h"
#import "DrawableObject.h"

#define FIELD_OF_VIEW_DEG 65
#define NEAR_PLANE 1.0f
#define FAR_PLAN 20.0f

static const GLfloat squareVertices[] = {
    -1.0f, -1.0f,
    1.0f, -1.0f,
    -1.0f,  1.0f,
    1.0f,  1.0f,
};

GLfloat textureVertices[8];

// Uniform index.
enum
{
    UNIFORM_VIDEO_FRAME,
    NUM_UNIFORMS
};
GLint uniforms[NUM_UNIFORMS];

// Attribute index.
enum {
    ATTRIB_VERTEX,
    ATTRIB_TEXCOORD,
    NUM_ATTRIBUTES
};

@interface ARViewController ()
{
    GLuint _program;
    
    GLuint _positionVBO;
    GLuint _indexVBO;
    
    size_t _textureWidth;
    size_t _textureHeight;
    
    CVOpenGLESTextureRef _backgroundTexture;
    CVOpenGLESTextureCacheRef _videoTextureCache;
}

@property (nonatomic, strong) EAGLContext* context;
@property (nonatomic, strong) GLKBaseEffect* baseEffect;
@property (nonatomic, assign) GLKMatrix4 projectionMatrix;
@property (nonatomic, assign) CGSize viewPortSize;

@end

@implementation ARViewController

#pragma mark - properties

@synthesize homography = _homography;
@synthesize background = _background;

@synthesize context = _context;
@synthesize baseEffect = _baseEffect;
@synthesize projectionMatrix = _projectionMatrix;

- (void)setBackground:(CVPixelBufferRef)background
{
    [self updateBackground:background];
}

@synthesize viewPortSize = _viewPortSize;

- (void)setViewPortSize:(CGSize)viewPortSize
{
    if (viewPortSize.width == _viewPortSize.width && viewPortSize.height == _viewPortSize.height)
        return;

    glViewport(0, 0, viewPortSize.width, viewPortSize.height);
    [self updateProjectionMatrixWithAspect:fabsf(viewPortSize.width / viewPortSize.height)];
    _viewPortSize = viewPortSize;
}

#pragma mark - view lifecycle

- (void)viewDidLoad
{
    [super viewDidLoad];

    // Create context
    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    if (!self.context) {
        NSLog(@"Failed to create ES context");
        return;
    }
    
    if (![EAGLContext setCurrentContext:self.context])
    {
        NSLog(@"Failed to set current OpenGL context");
        return;
    }
    
    // Initialize view
    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    //view.drawableColorFormat = GLKViewDrawableColorFormatRGBA8888; // or GLKViewDrawableColorFormatRGBA8888
    //view.drawableDepthFormat = GLKViewDrawableDepthFormat16; // or GLKViewDrawableDepthFormat24
    view.drawableMultisample = GLKViewDrawableMultisample4X;
    
    CVReturn err = CVOpenGLESTextureCacheCreate(kCFAllocatorDefault, NULL, (__bridge void *)_context, NULL, &_videoTextureCache);
    if (err)
    {
        NSLog(@"Error at CVOpenGLESTextureCacheCreate %d", err);
        return;
    }
    
    [self setupOpenGL];
    [self setViewPortSize:self.view.bounds.size];
    [self setPreferredFramesPerSecond:30];
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    
    [self tearDownOpenGL];
    
    [self cleanUpTextures];
    
    CFRelease(_videoTextureCache);
    
    if ([EAGLContext currentContext] == self.context)
        [EAGLContext setCurrentContext:nil];
    
    self.context = nil;
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    self.paused = NO;
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
}

- (void)viewWillDisappear:(BOOL)animated
{
    [super viewWillDisappear:animated];
}

- (void)viewDidDisappear:(BOOL)animated
{
    [super viewDidDisappear:animated];
    self.paused = YES;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return YES;
}

#pragma mark - GLKView and GLKViewController delegate methods

#pragma mark glkit view delegate

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    glClear(GL_COLOR_BUFFER_BIT);

    //[self.baseEffect prepareToDraw];
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    //glDrawElements(GL_TRIANGLE_STRIP, sizeof(bgIndices)/sizeof(bgIndices[0]), GL_UNSIGNED_SHORT, 0);
    
}

#pragma mark glkit view controller delegate

- (void)update
{
    [self setViewPortSize:self.view.bounds.size];
    
    
    
    
//    glBufferData(GL_ARRAY_BUFFER, [_ripple getVertexSize], [_ripple getTexCoords], GL_DYNAMIC_DRAW);
    
    self.baseEffect.transform.projectionMatrix = self.projectionMatrix;
    
    self.baseEffect.colorMaterialEnabled = GL_TRUE;
    
    self.baseEffect.light0.enabled = GL_TRUE;
    self.baseEffect.light0.diffuseColor = GLKVector4Make(1.0f, 0.8f, 0.6f, 1.0f);
    self.baseEffect.light0.ambientColor = GLKVector4Make(0.8f, 0.6f, 0.4f, 1.0f);
    self.baseEffect.light0.position = GLKVector4Make(3, 7, -7, 0);
    self.baseEffect.light0.specularColor = GLKVector4Make(1.0f, 0.8f, 0.6f, 1.0f);
    self.baseEffect.lightingType = GLKLightingTypePerPixel;
    
    GLKMatrix4 modelViewMatrix = GLKMatrix4MakeTranslation(0.0f, -1.0f, -6.0f);
    //modelViewMatrix = GLKMatrix4Rotate(modelViewMatrix, GLKMathDegreesToRadians(25), 1, 0, 0);
    modelViewMatrix = GLKMatrix4Rotate(modelViewMatrix, GLKMathDegreesToRadians(45), 0, 1, 0);
    self.baseEffect.transform.modelviewMatrix = modelViewMatrix;
    
}

#pragma mark - helper methods

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

- (void)initBuffers
{
//    glGenBuffers(1, &_positionVBO);
//    glBindBuffer(GL_ARRAY_BUFFER, _positionVBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(bgVertices), bgVertices, GL_STATIC_DRAW);
//    
//    glGenBuffers(1, &_indexVBO);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexVBO);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(bgIndices), bgIndices, GL_STATIC_DRAW);
//    
//    glEnableVertexAttribArray(GLKVertexAttribPosition);
//    glVertexAttribPointer(GLKVertexAttribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPositionTexture), (const GLvoid *) offsetof(VertexPositionTexture, Position));
//    
//    glEnableVertexAttribArray(GLKVertexAttribTexCoord0);
//    glVertexAttribPointer(GLKVertexAttribTexCoord0, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPositionTexture), (const GLvoid *) offsetof(VertexPositionTexture, TexCoord));
    
    
    glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, 0, 0, squareVertices);
	glEnableVertexAttribArray(ATTRIB_VERTEX);
	glVertexAttribPointer(ATTRIB_TEXCOORD, 2, GL_FLOAT, 0, 0, textureVertices);
	glEnableVertexAttribArray(ATTRIB_TEXCOORD);
}

- (void)setupOpenGL
{
    [EAGLContext setCurrentContext:self.context];
    // set clear color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    // enable culling and depth test
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // init shader
    self.baseEffect = [[GLKBaseEffect alloc] init];
    
    [self loadShaders];
    
    glUseProgram(_program);
    
    glUniform1i(uniforms[UNIFORM_VIDEO_FRAME], 0);
}

- (void)tearDownOpenGL
{
    // make our context current if neccessary
    if ([EAGLContext currentContext] != self.context)
        [EAGLContext setCurrentContext:self.context];
        
    glDeleteBuffers(1, &_positionVBO);
    glDeleteBuffers(1, &_indexVBO);
    
    if (_program) {
        glDeleteProgram(_program);
        _program = 0;
    }
    
    // clear shader
    self.baseEffect = nil;
}

- (CGRect)textureSamplingRectForCroppingTextureWithAspectRatio:(CGSize)textureAspectRatio toAspectRatio:(CGSize)croppingAspectRatio
{
	CGRect normalizedSamplingRect = CGRectZero;
	CGSize cropScaleAmount = CGSizeMake(croppingAspectRatio.width / textureAspectRatio.width, croppingAspectRatio.height / textureAspectRatio.height);
	CGFloat maxScale = fmax(cropScaleAmount.width, cropScaleAmount.height);
	CGSize scaledTextureSize = CGSizeMake(textureAspectRatio.width * maxScale, textureAspectRatio.height * maxScale);
	
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

- (void)updateBackground:(CVPixelBufferRef)pixelBuffer
{
    CVReturn error;
    size_t width = CVPixelBufferGetWidth(pixelBuffer);
    size_t height = CVPixelBufferGetHeight(pixelBuffer);
    
    if (!_videoTextureCache)
    {
        NSLog(@"No video texture cache");
        return;
    }
    
    if (!textureVertices || width != _textureWidth || height != _textureHeight)
    {
        _textureWidth = width;
        _textureHeight = height;
        
        CGRect textureSamplingRect = [self textureSamplingRectForCroppingTextureWithAspectRatio:CGSizeMake(_textureWidth, _textureHeight) toAspectRatio:self.view.bounds.size];
        GLfloat tmp[] = {
            CGRectGetMinX(textureSamplingRect), CGRectGetMaxY(textureSamplingRect),
            CGRectGetMaxX(textureSamplingRect), CGRectGetMaxY(textureSamplingRect),
            CGRectGetMinX(textureSamplingRect), CGRectGetMinY(textureSamplingRect),
            CGRectGetMaxX(textureSamplingRect), CGRectGetMinY(textureSamplingRect),
        };
        *textureVertices = *tmp;
        
        [self initBuffers];
    }
    
    [self cleanUpTextures];
    
    glActiveTexture(GL_TEXTURE0);
    error = CVOpenGLESTextureCacheCreateTextureFromImage(kCFAllocatorDefault,
                                                         _videoTextureCache,
                                                         pixelBuffer,
                                                         NULL,
                                                         GL_TEXTURE_2D,
                                                         GL_RGBA,
                                                         _textureWidth,
                                                         _textureHeight,
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

- (void)updateProjectionMatrixWithAspect:(float)aspect
{
    float fovRad = GLKMathDegreesToRadians(FIELD_OF_VIEW_DEG);
    _projectionMatrix = GLKMatrix4MakePerspective(fovRad, aspect, NEAR_PLANE, FAR_PLAN);
}

#pragma mark - OpenGL ES 2 shader compilation

- (BOOL)loadShaders
{
    GLuint vertShader, fragShader;
    NSString *vertShaderPathname, *fragShaderPathname;
    
    // Create shader program.
    _program = glCreateProgram();
    
    // Create and compile vertex shader.
    vertShaderPathname = [[NSBundle mainBundle] pathForResource:@"passThrough" ofType:@"vsh"];
    if (![self compileShader:&vertShader type:GL_VERTEX_SHADER file:vertShaderPathname]) {
        NSLog(@"Failed to compile vertex shader");
        return NO;
    }
    
    // Create and compile fragment shader.
    fragShaderPathname = [[NSBundle mainBundle] pathForResource:@"passThrough" ofType:@"fsh"];
    if (![self compileShader:&fragShader type:GL_FRAGMENT_SHADER file:fragShaderPathname]) {
        NSLog(@"Failed to compile fragment shader");
        return NO;
    }
    
    // Attach vertex shader to program.
    glAttachShader(_program, vertShader);
    
    // Attach fragment shader to program.
    glAttachShader(_program, fragShader);
        
    // Bind attribute locations.
    // This needs to be done prior to linking.
    glBindAttribLocation(_program, ATTRIB_VERTEX, "position");
    glBindAttribLocation(_program, ATTRIB_TEXCOORD, "texCoord");
    
    // Link program.
    if (![self linkProgram:_program]) {
        NSLog(@"Failed to link program: %d", _program);
        
        if (vertShader) {
            glDeleteShader(vertShader);
            vertShader = 0;
        }
        if (fragShader) {
            glDeleteShader(fragShader);
            fragShader = 0;
        }
        if (_program) {
            glDeleteProgram(_program);
            _program = 0;
        }
        
        return NO;
    }
    
    // Get uniform locations.
    uniforms[UNIFORM_VIDEO_FRAME] = glGetUniformLocation(_program, "Videoframe");
    
    // Release vertex and fragment shaders.
    if (vertShader) {
        glDetachShader(_program, vertShader);
        glDeleteShader(vertShader);
    }
    if (fragShader) {
        glDetachShader(_program, fragShader);
        glDeleteShader(fragShader);
    }
    
    return YES;
}

- (BOOL)compileShader:(GLuint *)shader type:(GLenum)type file:(NSString *)file
{
    GLint status;
    const GLchar *source;
    
    source = (GLchar *)[[NSString stringWithContentsOfFile:file encoding:NSUTF8StringEncoding error:nil] UTF8String];
    if (!source) {
        NSLog(@"Failed to load vertex shader");
        return NO;
    }
    
    *shader = glCreateShader(type);
    glShaderSource(*shader, 1, &source, NULL);
    glCompileShader(*shader);
    
#if defined(DEBUG)
    GLint logLength;
    glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetShaderInfoLog(*shader, logLength, &logLength, log);
        NSLog(@"Shader compile log:\n%s", log);
        free(log);
    }
#endif
    
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
    if (status == 0) {
        glDeleteShader(*shader);
        return NO;
    }
    
    return YES;
}

- (BOOL)linkProgram:(GLuint)prog
{
    GLint status;
    glLinkProgram(prog);
    
#if defined(DEBUG)
    GLint logLength;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetProgramInfoLog(prog, logLength, &logLength, log);
        NSLog(@"Program link log:\n%s", log);
        free(log);
    }
#endif
    
    glGetProgramiv(prog, GL_LINK_STATUS, &status);
    if (status == 0) {
        return NO;
    }
    
    return YES;
}

@end
