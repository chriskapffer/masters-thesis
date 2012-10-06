//
//  ARViewController.m
//  CKObjectTrackerDemoPhone
//
//  Created by Christoph Kapffer on 12.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import "ARViewController.h"
#import "BackgroundTexture.h"
#import "DrawableObject.h"

#define FIELD_OF_VIEW_DEG 60
#define NEAR_PLANE 0.1f
#define FAR_PLANE 100.0f
#define CAM_DIST 3.0f
#define SCALE_CUT_OFF 0.5f

#define PREFERRED_FRAMES_PER_SECOND 30
#define MAX_FRAMES_WITHOUT_OBJECT (PREFERRED_FRAMES_PER_SECOND * 1.0f)

@interface ARViewController ()
{
    int _framesWithOutObject;
}

@property (nonatomic, strong) EAGLContext* context;
@property (nonatomic, strong) GLKBaseEffect* baseEffect;
@property (nonatomic, assign) GLKMatrix4 projectionMatrix;
@property (nonatomic, assign) GLKMatrix4 modelMatrix;
@property (nonatomic, assign) GLKMatrix4 viewMatrix;
@property (nonatomic, assign) CGSize viewPortSize;

@property (nonatomic, assign) GLKMatrix4 objectRotationMatrix;
@property (nonatomic, assign) GLKMatrix4 objectTranslationMatrix;
@property (nonatomic, assign) GLKMatrix4 objectScaleMatrix;

@property (nonatomic, strong) BackgroundTexture* backgroundTexture;
@property (nonatomic, strong) DrawableObject* drawableObject;
@property (nonatomic, assign) BOOL drawObject;

@end

@implementation ARViewController

#pragma mark - properties

@synthesize context = _context;
@synthesize baseEffect = _baseEffect;
@synthesize viewPortSize = _viewPortSize;
@synthesize projectionMatrix = _projectionMatrix;
@synthesize modelMatrix = _modelMatrix;
@synthesize viewMatrix = _viewMatrix;

@synthesize backgroundTexture = _backgroundTexture;
@synthesize drawableObject = _drawableObject;

@synthesize objectRotationMatrix = _objectRotationMatrix;
@synthesize objectTranslationMatrix = _objectTranslationMatrix;
@synthesize objectScaleMatrix = _objectScaleMatrix;

@synthesize background = _background;
@synthesize drawObject = _drawObject;
@synthesize isObjectPresent = _isObjectPresent;

- (void)setViewPortSize:(CGSize)viewPortSize
{
    if (viewPortSize.width == _viewPortSize.width && viewPortSize.height == _viewPortSize.height)
        return;
    
    glViewport(0, 0, viewPortSize.width, viewPortSize.height);
    [self updateProjectionMatrixWithAspect:fabsf(viewPortSize.width / viewPortSize.height)];
    _viewPortSize = viewPortSize;
}

- (void)setObjectRotation:(Matrix3x3)rotation
{
    _objectRotationMatrix = GLKMatrix4MakeAndTranspose(
                                rotation.m00, rotation.m01, rotation.m02, 0,
                                rotation.m10, rotation.m11, rotation.m12, 0,
                                rotation.m20, rotation.m21, rotation.m22, 0,
                                           0,            0,            0, 1); // openGL is column major --> transpose!
}

- (void)setObjectTranslation:(CGPoint)translation
{
    translation.x -= 0.5f; // move to center
    translation.y -= 0.5f; // move to center
    
    float aspect = fabsf(self.viewPortSize.width / self.viewPortSize.height);
    float fovY = GLKMathDegreesToRadians(FIELD_OF_VIEW_DEG);
    float fovX = fovY * aspect;
    float halfHeightProjPlane = tanf(fovY / 2.0f) * CAM_DIST;
    float halfWidthProjPlane = tanf(fovX / 2.0f) * CAM_DIST;
    translation.x *= halfHeightProjPlane * 2.0f;
    translation.y *= halfWidthProjPlane * 2.0f;

    self.objectTranslationMatrix = GLKMatrix4MakeTranslation(translation.x, translation.y, 0);
}

- (void)setObjectScale:(CGFloat)scale
{
    scale = powf(scale, SCALE_CUT_OFF);
    self.objectScaleMatrix = GLKMatrix4MakeScale(scale, scale, scale);
}

- (void)setBackground:(CVPixelBufferRef)background
{
    [self updateBackground:background];
}

- (void)setIsObjectPresent:(BOOL)isObjectPresent
{
    if (isObjectPresent == _isObjectPresent) { return; }
    
    _isObjectPresent = isObjectPresent;
    if (isObjectPresent) {
        _drawObject = YES;
    } else {
        _framesWithOutObject = 0;
    }
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
    view.drawableColorFormat = GLKViewDrawableColorFormatRGB565; // or GLKViewDrawableColorFormatRGBA8888
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    view.drawableMultisample = GLKViewDrawableMultisample4X;
    
    [self setupOpenGL];
    
    // init objects
    self.backgroundTexture = [[BackgroundTexture alloc] initWithContext:self.context];
    self.drawableObject = [[DrawableObject alloc] initTeapot];
    
    self.modelMatrix = GLKMatrix4Identity;
    self.viewMatrix = GLKMatrix4MakeLookAt( 0, 0, -CAM_DIST, // pos
                                            0, 0,  0, // dst
                                           -1, 0,  0); // up
        
    [self setViewPortSize:self.view.bounds.size];
    [self setPreferredFramesPerSecond:PREFERRED_FRAMES_PER_SECOND];
    [self setIsObjectPresent:NO];
    [self setDrawObject:NO];
}

- (void)viewDidUnload
{
    [super viewDidUnload];
 
    self.backgroundTexture = nil;
    self.drawableObject = nil;
    
    [self tearDownOpenGL];
    
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glDisable(GL_DEPTH_TEST);
    [self.backgroundTexture draw];
    
    glEnable(GL_DEPTH_TEST);
    if (self.drawObject) {
        [self.drawableObject drawWithEffect:self.baseEffect];
    }
}

#pragma mark glkit view controller delegate

- (void)update
{
    [self setViewPortSize:self.view.bounds.size];
    self.backgroundTexture.viewPortSize = self.viewPortSize;
    
    self.modelMatrix = GLKMatrix4Multiply(GLKMatrix4Multiply(self.objectTranslationMatrix, self.objectRotationMatrix), self.objectScaleMatrix);
    
    self.baseEffect.transform.projectionMatrix = self.projectionMatrix;
    self.baseEffect.transform.modelviewMatrix = GLKMatrix4Multiply(self.viewMatrix, self.modelMatrix);
    
    if (!self.isObjectPresent && self.drawObject) {
        _framesWithOutObject++;
        if (_framesWithOutObject >= MAX_FRAMES_WITHOUT_OBJECT) {
            self.drawObject = NO;
        }
    }
}

#pragma mark - helper methods

- (void)setupOpenGL
{
    [EAGLContext setCurrentContext:self.context];
    
    // init base effect (mimics fixed function pipeline)
    self.baseEffect = [[GLKBaseEffect alloc] init];
    self.baseEffect.light0.diffuseColor = GLKVector4Make(1.0f, 0.4f, 0.4f, 1.0f);
    self.baseEffect.light0.enabled = GL_TRUE;
    
    // set clear color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

- (void)tearDownOpenGL
{
    // make our context current if neccessary
    if ([EAGLContext currentContext] != self.context)
        [EAGLContext setCurrentContext:self.context];

    // clear base effect
    self.baseEffect = nil;
}

- (void)updateBackground:(CVPixelBufferRef)pixelBuffer
{
    [self.backgroundTexture updateContent:pixelBuffer];
}

- (void)updateProjectionMatrixWithAspect:(float)aspect
{
    float fovRad = GLKMathDegreesToRadians(FIELD_OF_VIEW_DEG);
    self.projectionMatrix = GLKMatrix4MakePerspective(fovRad, aspect, NEAR_PLANE, FAR_PLANE);
}

@end
