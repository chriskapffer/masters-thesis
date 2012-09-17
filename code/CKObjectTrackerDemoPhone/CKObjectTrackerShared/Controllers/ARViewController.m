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
#define NEAR_PLANE 0.1f
#define FAR_PLANE 1000.0f

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

@synthesize modelView = _modelView;
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
    self.modelView = GLKMatrix4Identity;
    
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
    
    self.baseEffect.transform.projectionMatrix = _projection;// [self getProjection];
    
    GLKMatrix4 baseModelViewMatrix = GLKMatrix4Identity;
    //GLKMatrix4Translate(baseModelViewMatrix, 240, 360, 0);
    baseModelViewMatrix = GLKMatrix4MakeLookAt(0, 0, 5, // pos
                                               0, 0, 0, // dst
                                               0, 1, 0); // up
    //baseModelViewMatrix = GLKMatrix4Identity;
    //GLKMatrix4Translate(baseModelViewMatrix, 240, 360, 0);
    
    GLKMatrix4 rotation = self.modelView;
    
    //baseModelViewMatrix = GLKMatrix4Rotate(baseModelViewMatrix, 90, 1.0f, 0.0f, 0.0f);
//
//    // Compute the model view matrix for the object rendered with GLKit
//    GLKMatrix4 modelViewMatrix = GLKMatrix4MakeTranslation(0.0f, 0.0f, -1.5f);
//    modelViewMatrix = GLKMatrix4Rotate(modelViewMatrix, 45, 1.0f, 1.0f, 1.0f);
//    modelViewMatrix = GLKMatrix4Multiply(baseModelViewMatrix, modelViewMatrix);
    
    self.baseEffect.transform.modelviewMatrix = GLKMatrix4Multiply(baseModelViewMatrix, self.modelView);
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
    _projectionMatrix = GLKMatrix4MakePerspective(fovRad, aspect, NEAR_PLANE, FAR_PLANE);
}

//// Returns calibratoin data for iPad 2
//// Todo: Add parameters for the rest
//return CameraCalibration(6.24860291e+02 * (640./352.), 6.24860291e+02 * (480./288.), 640 * 0.5f, 480 * 0.5f);

- (GLKMatrix4)getProjection
{
    // Camera parameters
    double fx = 604.18678406; // Focal length in x axis
    double fy = 604.43273831; // Focal length in y axis
    double cx = 319.50000000; // Camera primary point x
    double cy = 239.50000000; // Camera primary point y
    
    double screen_width = 480; // In pixels
    double screen_height = 640; // In pixels
    
    double fovY = 1/(fx/screen_height * 2);
    double aspectRatio = screen_width/screen_height * fy/fx;
    double near = .1;  // Near clipping distance
    double far = 1000;  // Far clipping distance
    double frustum_height = near * fovY;
    double frustum_width = frustum_height * aspectRatio;
    
    double offset_x = (screen_width/2 - cx)/screen_width * frustum_width * 2;
    double offset_y = (screen_height/2 - cy)/screen_height * frustum_height * 2;
    
    double left = -frustum_width - offset_x;
    double right = frustum_width - offset_x;
    double top = -frustum_height - offset_y;
    double bottom = frustum_height - offset_y;
    
    // Build and apply the projection matrix
    GLKMatrix4 proj = {
            (2 * near) / (right - left),                               0,                                0,  0,
                                      0,     (2 * near) / (top - bottom),                                0,  0,
        (right + left) / (right - left), (top + bottom) / (top - bottom),     -(far + near) / (far - near), -1,
                                      0,                               0, -(2 * far * near) / (far - near),  0
    };
    
    return proj;
}

@end

// http://urbanar.blogspot.de/2011/04/from-homography-to-opengl-modelview.html
// http://en.wikipedia.org/wiki/Camera_matrix
// http://en.wikipedia.org/wiki/Camera_resectioning
// http://www.songho.ca/opengl/gl_projectionmatrix.html
// http://www.songho.ca/opengl/gl_transform.html
// 
// http://www.cse.unr.edu/~bebis/CS791E/Notes/CameraParameters.pdf
// http://sightations.wordpress.com/2010/08/03/simulating-calibrated-cameras-in-opengl/
// http://www.opengl.org/discussion_boards/showthread.php/168267-Real-world-camera-parameters?p=1186938#post1186938
// http://stackoverflow.com/questions/3712049/how-to-use-an-opencv-rotation-and-translation-vector-with-opengl-es-in-android
// http://dsp.stackexchange.com/questions/2736/step-by-step-camera-pose-estimation-for-visual-tracking-and-planar-markers

// ( http://vision.ucla.edu//MASKS/MASKS-ch5.pdf )

// http://docs.opencv.org/doc/tutorials/calib3d/camera_calibration/camera_calibration.html#cameracalibrationopencv
// http://stackoverflow.com/questions/3594199/iphone-4-camera-specifications-field-of-view-vertical-horizontal-angle
