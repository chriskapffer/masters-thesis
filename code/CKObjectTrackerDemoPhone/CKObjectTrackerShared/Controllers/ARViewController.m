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

#define FIELD_OF_VIEW_DEG 65
#define NEAR_PLANE 1.0f
#define FAR_PLAN 20.0f

@interface ARViewController ()

@property (nonatomic, strong) EAGLContext* context;
@property (nonatomic, strong) GLKBaseEffect* baseEffect;
@property (nonatomic, assign) GLKMatrix4 projectionMatrix;
@property (nonatomic, strong) BackgroundTexture* backgroundTexture;
@property (nonatomic, strong) DrawableObject* drawableObject;
@property (nonatomic, assign) CGSize viewPortSize;

@end

@implementation ARViewController

#pragma mark - properties

@synthesize homography = _homography;
@synthesize background = _background;

@synthesize context = _context;
@synthesize baseEffect = _baseEffect;
@synthesize projectionMatrix = _projectionMatrix;
@synthesize backgroundTexture = _backgroundTexture;
@synthesize drawableObject = _drawableObject;
@synthesize viewPortSize = _viewPortSize;

- (void)setBackground:(CVPixelBufferRef)background
{
    [self updateBackground:background];
}

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
    view.drawableColorFormat = GLKViewDrawableColorFormatRGB565; // or GLKViewDrawableColorFormatRGBA8888
    //view.drawableDepthFormat = GLKViewDrawableDepthFormat16; // or GLKViewDrawableDepthFormat24
    view.drawableMultisample = GLKViewDrawableMultisample4X;
    
    [self setupOpenGL];
    
    // init objects
    self.backgroundTexture = [[BackgroundTexture alloc] initWithContext:self.context];
    self.drawableObject = [[DrawableObject alloc] init];
    
    [self setViewPortSize:self.view.bounds.size];
    [self setPreferredFramesPerSecond:30];
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
    
    [self.backgroundTexture draw];
    
    [self.baseEffect prepareToDraw];
    
    [self.drawableObject draw];
}

#pragma mark glkit view controller delegate

- (void)update
{
    [self setViewPortSize:self.view.bounds.size];
    self.backgroundTexture.viewPortSize = self.viewPortSize;
    
    self.baseEffect.transform.projectionMatrix = _projectionMatrix;
    
    GLKMatrix4 baseModelViewMatrix = GLKMatrix4MakeTranslation(0.0f, 0.0f, -4.0f);
    baseModelViewMatrix = GLKMatrix4Rotate(baseModelViewMatrix, 0, 0.0f, 1.0f, 0.0f);
    
    // Compute the model view matrix for the object rendered with GLKit
    GLKMatrix4 modelViewMatrix = GLKMatrix4MakeTranslation(0.0f, 0.0f, -1.5f);
    modelViewMatrix = GLKMatrix4Rotate(modelViewMatrix, 45, 1.0f, 1.0f, 1.0f);
    modelViewMatrix = GLKMatrix4Multiply(baseModelViewMatrix, modelViewMatrix);
    
    self.baseEffect.transform.modelviewMatrix = modelViewMatrix;
}

#pragma mark - helper methods

- (void)setupOpenGL
{
    [EAGLContext setCurrentContext:self.context];
    // set clear color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    // enable culling and depth test
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // init base effect (mimics fixed function pipeline)
    self.baseEffect = [[GLKBaseEffect alloc] init];
    self.baseEffect.light0.diffuseColor = GLKVector4Make(1.0f, 0.4f, 0.4f, 1.0f);
    self.baseEffect.light0.enabled = GL_TRUE;
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
    _projectionMatrix = GLKMatrix4MakePerspective(fovRad, aspect, NEAR_PLANE, FAR_PLAN);
}

@end
// http://urbanar.blogspot.de/2011/04/from-homography-to-opengl-modelview.html
// http://urbanar.blogspot.de/2011/04/offline-camera-calibration-for.html

// http://computer-vision-talks.com/2011/11/pose-estimation-problem/


// code from https://gist.github.com/740979/97f54a63eb5f61f8f2eb578d60eb44839556ff3f
//var intrinsic:Vector.<Number> = new Vector.<Number>(9, true);
//var intrinsicInverse:Vector.<Number> = new Vector.<Number>(9, true);
//
//var R:Vector.<Number> = new Vector.<Number>( 9, true );
//var t:Vector.<Number> = new Vector.<Number>( 3, true );
//
//// SVD routine
//var svd:SVD = new SVD();
//
//// input homography[9] - 3x3 Matrix
//// please note that homography should be computed
//// using centered object/reference points coordinates
//// for example coords from [0, 0], [320, 0], [320, 240], [0, 240]
//// should be converted to [-160, -120], [160, -120], [160, 120], [-160, 120]
//function computePose(homography:Vector.<Number>):Boolean
//{
//	var h1:Vector.<Number> = Vector.<Number>([homography[0], homography[3], homography[6]]);
//	var h2:Vector.<Number> = Vector.<Number>([homography[1], homography[4], homography[7]]);
//	var h3:Vector.<Number> = Vector.<Number>([homography[2], homography[5], homography[8]]);
//	var invH1:Vector.<Number> = new Vector.<Number>(3, true);
//	var invC:Vector.<Number>;
//	var r1:Vector.<Number> = new Vector.<Number>(3, true);
//	var r2:Vector.<Number> = new Vector.<Number>(3, true);
//	var r3:Vector.<Number> = new Vector.<Number>(3, true);
//	var vT:Vector.<Number> = new Vector.<Number>(9, true);
//	//
//	invC = intrinsicInverse.concat();
//	// matrix multiplication [src1, src2, dst]
//	multMat(invC, h1, invH1);
//    
//	var v0:Number = invH1[0];
//	var v1:Number = invH1[1];
//	var v2:Number = invH1[2];
//	var lambda:Number = Math.sqrt( v0 * v0 + v1 * v1 + v2 * v2 );
//    
//	if (lambda == 0) return false;
//    
//	lambda = 1.0 / lambda;
//	invC[0] *= lambda;
//	invC[1] *= lambda;
//	invC[2] *= lambda;
//	invC[3] *= lambda;
//	invC[4] *= lambda;
//	invC[5] *= lambda;
//	invC[6] *= lambda;
//	invC[7] *= lambda;
//	invC[8] *= lambda;
//    
//	// Create normalized R1 & R2:
//	multMat(invC, h1, r1);
//	multMat(invC, h2, r2);
//    
//	// Get R3 orthonormal to R1 and R2:
//	r3[0] = r1[1] * r2[2] - r1[2] * r2[1];
//	r3[1] = r1[2] * r2[0] - r1[0] * r2[2];
//	r3[2] = r1[0] * r2[1] - r1[1] * r2[0];
//    
//	// Put the rotation column vectors in the rotation matrix:
//	// u can play with flip sign of rows here depending on how u apply 3D matrix
//	R[0] = r1[0];
//	R[1] = r2[0];
//	R[2] = r3[0];
//	R[3] = r1[1];
//	R[4] = r2[1];
//	R[5] = r3[1];
//	R[6] = r1[2];
//	R[7] = r2[2];
//	R[8] = r3[2];
//    
//	// Calculate Translation Vector T:
//	multMat(invC, h3, t);
//    
//	// Transformation of R into - in Frobenius sense - next orthonormal matrix:
//	svd.decompose( R, 3, 3 );
//	transposeMat( svd.V, vT );
//	multMat( svd.U, vT, R );
//    
//	return true;
//}
//
//function setIntrinsicParams(fx:Number, fy:Number, cx:Number, cy:Number):void
//{
//	intrinsic[0] = fx;
//	intrinsic[4] = fy;
//	intrinsic[2] = cx;
//	intrinsic[5] = cy;
//	intrinsic[8] = 1.0;
//	//
//	// Create inverse calibration matrix:
//	var tau:Number = fx / fy;
//	intrinsicInverse[0] = 1.0 / (tau*fy);
//	intrinsicInverse[1] = 0.0;
//	intrinsicInverse[2] = -cx / (tau*fy);
//	intrinsicInverse[3] = 0.0;
//	intrinsicInverse[4] = 1.0 / fy;
//	intrinsicInverse[5] = -cy / fy;
//	intrinsicInverse[6] = 0.0;
//	intrinsicInverse[7] = 0.0;
//	intrinsicInverse[8] = 1.0;
//}
