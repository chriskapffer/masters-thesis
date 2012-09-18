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
#define FAR_PLANE 1000.0f
#define CAM_DIST 3.0f

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
@property (nonatomic, assign) GLKMatrix3 invIntrinsics;
@property (nonatomic, assign) CGSize viewPortSize;

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

@synthesize intrinsics = _intrinsics;
@synthesize invIntrinsics = _invIntrinsics;
@synthesize homography = _homography;
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

- (void)setIntrinsics:(GLKMatrix3)intrinsics
{
    _intrinsics = intrinsics;
    bool isInvertible;
    _invIntrinsics = GLKMatrix3Invert(intrinsics, &isInvertible);
    if (!isInvertible) {
        NSLog(@"Failed to invert camera matrix!");
    }
}

- (void)setHomography:(GLKMatrix3)homography
{
    [self updateModelMatrixWithHomography:homography];
    _homography = homography;
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
    //view.drawableDepthFormat = GLKViewDrawableDepthFormat16; // or GLKViewDrawableDepthFormat24
    view.drawableMultisample = GLKViewDrawableMultisample4X;
    
    [self setupOpenGL];
    
    // init objects
    self.backgroundTexture = [[BackgroundTexture alloc] initWithContext:self.context];
    self.drawableObject = [[DrawableObject alloc] init];
    
    self.modelMatrix = GLKMatrix4Identity;
    self.viewMatrix = GLKMatrix4MakeLookAt( 0, 0, -CAM_DIST, // pos
                                            0, 0,  0, // dst
                                           -1, 0,  0); // up
    
    double fx = 604.43273831; // Focal length in x axis
    double fy = 604.18678406; // Focal length in y axis
    double cx = 239.50000000; // Camera primary point x
    double cy = 319.50000000; // Camera primary point y
    self.intrinsics = GLKMatrix3MakeAndTranspose(fx,  0, cx,
                                                  0, fy, cy,
                                                  0,  0,  1); // openGL is column major --> transpose!
    
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
    
    [self.backgroundTexture draw];
    
    [self.baseEffect prepareToDraw];
    if (self.drawObject) {
        [self.drawableObject draw];
    }
}

#pragma mark glkit view controller delegate

- (void)update
{
    [self setViewPortSize:self.view.bounds.size];
    self.backgroundTexture.viewPortSize = self.viewPortSize;
    
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
    self.projectionMatrix = GLKMatrix4MakePerspective(fovRad, aspect, NEAR_PLANE, FAR_PLANE);
}

- (void)updateModelMatrixWithHomography:(GLKMatrix3)homography
{
    // Column vectors of homography
    GLKVector3 h1 = GLKMatrix3GetColumn(homography, 0);
    GLKVector3 h2 = GLKMatrix3GetColumn(homography, 1);
    
    // inverse
    GLKVector3 invH1 = GLKMatrix3MultiplyVector3(self.invIntrinsics, h1);
    GLKVector3 invH2 = GLKMatrix3MultiplyVector3(self.invIntrinsics, h2);
    
    // Column vectors of rotation matrix
    GLKVector3 r1 = GLKVector3Normalize(invH1);
    GLKVector3 r2 = GLKVector3Normalize(invH2);
    GLKVector3 r3 = GLKVector3CrossProduct(r1, r2);
    GLKMatrix4 rotationMatrix = GLKMatrix4MakeAndTranspose(r1.x, r2.x, r3.x, 0,
                                                           r1.y, r2.y, r3.y, 0,
                                                           r1.z, r2.z, r3.z, 0,
                                                              0,    0,    0, 1); // openGL is column major --> transpose!
    
    float scaleFactor = (homography.m00 + homography.m11) / 2.0f;
    GLKMatrix4 scaleMatrix = GLKMatrix4MakeScale(scaleFactor, scaleFactor, scaleFactor);

    float sceneWidth = self.backgroundTexture.textureSize.width;
    float sceneHeight = self.backgroundTexture.textureSize.height;
    float objectSize = MIN(sceneWidth, sceneHeight) * scaleFactor; // TODO: get real object width and height
    float offsetX = (sceneWidth - objectSize) / 2.0f;
    float offsetY = (sceneHeight - objectSize) / 2.0f;
    GLKVector3 t = GLKVector3Make(homography.m20 - offsetX, homography.m21 - offsetY, 0); // translation from center in px
    t.x = t.x / (sceneWidth * 0.5f); // normalized
    t.y = t.y / (sceneHeight * 0.5f); // normalized
    
    float aspect = fabsf(self.viewPortSize.width / self.viewPortSize.height);
    float fovY = GLKMathDegreesToRadians(FIELD_OF_VIEW_DEG);
    float fovX = fovY * aspect;
    float halfWidthProjPlane = tanf(fovX / 2.0f) * CAM_DIST;
    float halfHeightProjPlane = tanf(fovY / 2.0f) * CAM_DIST;

    t.x *= halfWidthProjPlane;
    t.y *= halfHeightProjPlane;
    GLKMatrix4 translationMatrix = GLKMatrix4MakeTranslation(t.x, t.y, 0);
    
    self.modelMatrix = GLKMatrix4Multiply(GLKMatrix4Multiply(translationMatrix, rotationMatrix), scaleMatrix);
}
// http://stackoverflow.com/questions/5342330/how-to-augment-cube-onto-a-specific-position-using-3x3-homography

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
