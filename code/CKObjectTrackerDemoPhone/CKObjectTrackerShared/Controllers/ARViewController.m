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

@interface ARViewController ()
{
	size_t videoframeWidth;
	size_t videoframeHeight;
}

@property (nonatomic, strong) EAGLContext* renderContext;
@property (nonatomic, strong) GLKBaseEffect* shaderEffect;
@property (nonatomic, assign) GLKMatrix4 projectionMatrix;
@property (nonatomic, strong) DrawableObject* object;
@property (nonatomic, assign) CGSize viewSize;


@end

@implementation ARViewController

#pragma mark - properties

@synthesize homography = _homography;
@synthesize cameraImage = _cameraImage;

@synthesize renderContext = _renderContext;
@synthesize shaderEffect = _shaderEffect;
@synthesize projectionMatrix = _projectionMatrix;

@synthesize viewSize = _viewSize;

- (void)setCameraImage:(CVPixelBufferRef)cameraImage
{
}

- (void)setViewSize:(CGSize)viewSize
{
    if (viewSize.width == _viewSize.width && viewSize.height == _viewSize.height)
        return;

    [self updateProjectionMatrixWithAspect:fabsf(viewSize.width / viewSize.height)];
    _viewSize = viewSize;
}

#pragma mark - view lifecycle

- (void)viewDidLoad
{
    [super viewDidLoad];

    self.renderContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    [self setPreferredFramesPerSecond:30];
    
    // init gl kit view
    GLKView* glkView = (GLKView*)self.view;
    glkView.context = self.renderContext;
    //glkView.drawableColorFormat = GLKViewDrawableColorFormatRGBA8888; // or GLKViewDrawableColorFormatRGBA8888
    //glkView.drawableDepthFormat = GLKViewDrawableDepthFormat16; // or GLKViewDrawableDepthFormat24
    glkView.drawableMultisample = GLKViewDrawableMultisample4X;
    //glkView.delegate = self;
    
    [self setupOpenGL];
    
    self.object = [[DrawableObject alloc] init];
    
    [self updateProjectionMatrixWithAspect:fabsf(self.view.bounds.size.width / self.view.bounds.size.height)];
    
    
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    
    self.object = nil;
    
    [self tearDownOpenGL];
    
    if ([EAGLContext currentContext] == self.renderContext)
        [EAGLContext setCurrentContext:nil];
    
    self.renderContext = nil;
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

#pragma mark glkit view delegate

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    glClear(GL_COLOR_BUFFER_BIT);

    [self.shaderEffect prepareToDraw];
    
    [self.object draw];
}

#pragma mark glkit view controller delegate

- (void)update
{
    self.viewSize = self.view.bounds.size;
    self.shaderEffect.transform.projectionMatrix = self.projectionMatrix;
    
    self.shaderEffect.texture2d0.name = self.object.textureName;
    self.shaderEffect.texture2d0.enabled = GL_TRUE;
    
    self.shaderEffect.colorMaterialEnabled = GL_TRUE;
    
    self.shaderEffect.light0.enabled = GL_TRUE;
    self.shaderEffect.light0.diffuseColor = GLKVector4Make(1.0f, 0.8f, 0.6f, 1.0f);
    self.shaderEffect.light0.ambientColor = GLKVector4Make(0.8f, 0.6f, 0.4f, 1.0f);
    self.shaderEffect.light0.position = GLKVector4Make(3, 7, -7, 0);
    self.shaderEffect.light0.specularColor = GLKVector4Make(1.0f, 0.8f, 0.6f, 1.0f);
    self.shaderEffect.lightingType = GLKLightingTypePerPixel;
    
    GLKMatrix4 modelViewMatrix = GLKMatrix4MakeTranslation(0.0f, -1.0f, -6.0f);
    //modelViewMatrix = GLKMatrix4Rotate(modelViewMatrix, GLKMathDegreesToRadians(25), 1, 0, 0);
    modelViewMatrix = GLKMatrix4Rotate(modelViewMatrix, GLKMathDegreesToRadians(45), 0, 1, 0);
    self.shaderEffect.transform.modelviewMatrix = modelViewMatrix;
    
}



#pragma mark - helper methods

- (void)setupOpenGL
{
    [EAGLContext setCurrentContext:self.renderContext];
    
    
    // init shader
    self.shaderEffect = [[GLKBaseEffect alloc] init];
    


    
    
    glClearColor(1.0f, 0.5f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

- (void)tearDownOpenGL
{
    // make our context current if neccessary
    if ([EAGLContext currentContext] != self.renderContext)
        [EAGLContext setCurrentContext:self.renderContext];
        
    // clear shader
    self.shaderEffect = nil;
}

- (void)updateProjectionMatrixWithAspect:(float)aspect
{
    float fovRad = GLKMathDegreesToRadians(FIELD_OF_VIEW_DEG);
    _projectionMatrix = GLKMatrix4MakePerspective(fovRad, aspect, NEAR_PLANE, FAR_PLAN);
}

@end
