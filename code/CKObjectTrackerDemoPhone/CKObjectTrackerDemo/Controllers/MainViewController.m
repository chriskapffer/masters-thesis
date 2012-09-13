//
//  MainViewController.m
//  CKObjectTrackerDemo
//
//  Created by Christoph Kapffer on 17.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import "MainViewController.h"
#import "SettingsViewController.h"
#import "ARViewController.h"

#import "ObjectTrackerLibrary.h"
#import "CaptureManager.h"

#import "UIImage+Transform.h"
#import "UIImage+PixelBuffer.h"
#import "UIView+EasyFrame.h"

@interface MainViewController () <CaptureManagerDelegate, ObjectTrackerLibraryDelegate, SettingsViewControllerDelegate, UIActionSheetDelegate, UIImagePickerControllerDelegate>

@property (nonatomic, strong) SettingsViewController* settingsController;
@property (nonatomic, strong) ARViewController* arViewController;

@property (nonatomic, strong) UIImageView* debugViewTracking;
@property (nonatomic, strong) UIImageView* debugViewValidation;
@property (nonatomic, strong) UIImageView* debugViewDetection;
@property (nonatomic, strong) UIImageView* debugViewObject;
@property (nonatomic, strong) NSArray* imageViewNames;

@end

@implementation MainViewController

#pragma mark - properties

@synthesize textView = _textView;
@synthesize scrollView = _scrollView;
@synthesize pageControl = _pageControl;
@synthesize navBarItem = _navBarItem;

@synthesize settingsController = _settingsController;
@synthesize arViewController = _arViewController;

@synthesize debugViewTracking = _debugViewTracking;
@synthesize debugViewValidation = _debugViewValidation;
@synthesize debugViewDetection = _debugViewDetection;
@synthesize debugViewObject = _debugViewObject;
@synthesize imageViewNames = _imageViewNames;

#pragma mark - view lifecycle

- (void)viewDidLoad
{
    [super viewDidLoad];
    [[UIApplication sharedApplication] setStatusBarHidden:NO withAnimation:NO];
    
    [[CaptureManager instance] setPixelFormat:kCVPixelFormatType_32BGRA];
    [[CaptureManager instance] setSessionPreset:AVCaptureSessionPreset352x288];
    [[CaptureManager instance] setDelegate:self];
    [[ObjectTrackerLibrary instance] setDelegate:self];
    [[ObjectTrackerLibrary instance] setRecordDebugInfo:NO];
    
    self.arViewController = [[ARViewController alloc] init];
    self.arViewController.view.frame = self.scrollView.frame;
    [self.scrollView addSubview:self.arViewController.view];
    
    self.imageViewNames = [NSArray arrayWithObjects:
                           @"Object Image", @"Statistics", @"Augmented Reality", @"Tracking View", @"Validation View", @"Detection View", nil];
    
    self.debugViewObject = [self registeredImageViewWithIndex:0];
    [self.textView setFrameOrigin:CGPointMake(self.scrollView.bounds.size.width* 1, 0)]; // <-- index 1
    [self.arViewController.view setFrameOrigin:CGPointMake(self.scrollView.bounds.size.width * 2, 0)]; // <-- index 2
    self.debugViewTracking = [self registeredImageViewWithIndex:3];
    self.debugViewValidation = [self registeredImageViewWithIndex:4];
    self.debugViewDetection = [self registeredImageViewWithIndex:5];
    self.scrollView.contentSize = CGSizeMake(self.scrollView.bounds.size.width * 6, self.scrollView.bounds.size.height);
    self.scrollView.contentOffset = CGPointMake(self.scrollView.bounds.size.width * 2, 0);
    self.pageControl.numberOfPages = 6;
    self.pageControl.currentPage = 2;
    self.textView.text = @"";
    
    self.settingsController = nil;

}

- (void)viewDidUnload
{
    [super viewDidUnload];
    
    self.textView = nil;
    self.scrollView = nil;
    self.pageControl = nil;
    self.navBarItem = nil;
    
    if (self.settingsController != nil)
        self.settingsController = nil;
    if (self.arViewController != nil)
        self.arViewController = nil;
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];

    //[[ObjectTrackerLibrary instance] setDelegate:self];
}

- (void)viewDidDisappear:(BOOL)animated
{
    [super viewDidDisappear:animated];
    
    //[[ObjectTrackerLibrary instance] setDelegate:nil];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

#pragma mark - interface builder actions

- (IBAction)showSettingsButtonClicked:(id)sender
{
    if (self.settingsController == nil) {
        self.settingsController = [[SettingsViewController alloc] init];
        self.settingsController.modalTransitionStyle = UIModalTransitionStyleFlipHorizontal;
        self.settingsController.delegate = self;
    }
    
    [self presentViewController:self.settingsController animated:YES completion:nil];
}

- (IBAction)objectImagePickerButtonClicked:(id)sender
{
    UIActionSheet* actionSheet = [[UIActionSheet alloc] initWithTitle:@"Pick an image of the object to track." delegate:self cancelButtonTitle:@"Cancel" destructiveButtonTitle:nil otherButtonTitles:@"From Camera", @"From Photo Album", nil];
    [actionSheet showInView:self.view];
}

#pragma mark - scroll view delegate

- (void)scrollViewDidScroll:(UIScrollView *)sender {
    // Update the page when more than 50% of the previous/next page is visible
    CGFloat pageWidth = self.scrollView.frame.size.width;
    int page = floor((self.scrollView.contentOffset.x - pageWidth / 2) / pageWidth) + 1;
    if (page != self.pageControl.currentPage) {
        self.pageControl.currentPage = page;
        [self updateNavBarItem];
    }
}

#pragma mark - action sheet delegate

- (void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex
{
    [[CaptureManager instance] stopAndShutDownCaptureSession];
    
    UIImagePickerController* imagePicker = [[UIImagePickerController alloc] init];
    if (buttonIndex == 0) {
        imagePicker.sourceType = UIImagePickerControllerSourceTypeCamera;
        imagePicker.cameraCaptureMode = UIImagePickerControllerCameraCaptureModePhoto;
    } else {
        imagePicker.sourceType = UIImagePickerControllerSourceTypeSavedPhotosAlbum;
    }
    imagePicker.allowsEditing = YES;
    imagePicker.delegate = (id)self;
    imagePicker.modalPresentationStyle = UIModalPresentationFullScreen;
    imagePicker.modalTransitionStyle = UIModalTransitionStyleFlipHorizontal;
    [self presentViewController:imagePicker animated:YES completion:nil];
}

#pragma mark - image picker controller delegate

- (void)imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info
{
    [[CaptureManager instance] setUpAndStartCaptureSession];
    
    CaptureManager* captureManager = [CaptureManager instance];
    CGSize targetSize = [captureManager videoResolutionForSessionPreset:captureManager.sessionPreset];
    NSLog(@"size: %d, %d", (int)targetSize.width, (int)targetSize.height);
    UIImage* image = [[info objectForKey:UIImagePickerControllerEditedImage] rotatedImageWithAngle:-M_PI_2];
    if (!image) { image = [info objectForKey:UIImagePickerControllerOriginalImage]; }
    
    [[ObjectTrackerLibrary instance] setObjectImageWithImage:[image scaledImageWithSize:targetSize]];
    [self.debugViewObject setImage:[image rotatedImageWithAngle:M_PI_2]];
    
    [self dismissViewControllerAnimated:YES completion:nil];
}

- (void)imagePickerControllerDidCancel:(UIImagePickerController *)picker
{
    [self dismissViewControllerAnimated:YES completion:nil];
}

#pragma mark - settings view controller delegate

- (void)settingsControllerFinished
{
    [self dismissViewControllerAnimated:YES completion:nil];
}

- (void)criticalParameterHasChanged
{
    // re-init object image
    ObjectTrackerLibrary* objectTrackerLibrary = [ObjectTrackerLibrary instance];
    [objectTrackerLibrary setObjectImageWithImage:objectTrackerLibrary.objectImage];
}

#pragma mark - capture manager delegate

- (void)didCaptureFrameWithPixelBuffer:(CVPixelBufferRef)pixelBuffer
{
    __block CVPixelBufferRef retainedBuffer = CVPixelBufferRetain(pixelBuffer);
    dispatch_async(dispatch_get_main_queue(), ^{
        self.arViewController.background = retainedBuffer;
        CVPixelBufferRelease(retainedBuffer);
    });
    [[ObjectTrackerLibrary instance] trackObjectInVideoWithBuffer:pixelBuffer];
}

#pragma mark - object tracker library delegate

- (void)trackerLibraryDidProcessFrame
{
    dispatch_async(dispatch_get_main_queue(), ^{
        UIImage* debugImage;
        ObjectTrackerLibrary* trackerLib = [ObjectTrackerLibrary instance];
        if (trackerLib.foundObject) {
            self.arViewController.homography = [self matrixFromHomography:trackerLib.homography];
        }
        if ([trackerLib detectionDebugImage:&debugImage WithSearchWindow:YES]) {
            [self.debugViewDetection setImage:debugImage];
        }
        if ([trackerLib validationDebugImage:&debugImage WithObjectRect:YES ObjectKeyPoints:YES SceneKeyPoints:YES FilteredMatches:YES AllMatches:YES]) {
            [self.debugViewValidation setImage:debugImage];
        }
        if ([trackerLib trackingDebugImage:&debugImage WithObjectRect:YES FilteredPoints:YES AllPoints:YES SearchWindow:NO]) {
            [self.debugViewTracking setImage:debugImage];
        }
        self.textView.text = [trackerLib frameDebugInfoString];
    });
    //NSLog(@"\n%@", [[ObjectTrackerLibrary instance] frameDebugInfoString]);
}

#pragma mark - helper methods

- (UIImageView*)registeredImageViewWithIndex:(int)index
{
    UIImageView* imageView = [[UIImageView alloc] initWithFrame:self.scrollView.bounds];
    [imageView setFrameOriginX:self.scrollView.bounds.size.width * index];
    [imageView setContentMode:UIViewContentModeScaleAspectFit];
    [self.scrollView addSubview:imageView];
    return imageView;
}

- (void)updateNavBarItem
{
    [self.navBarItem setTitle:[self.imageViewNames objectAtIndex:self.pageControl.currentPage]];
}

- (GLKMatrix3)matrixFromHomography:(Homography)homography
{
    GLKMatrix3 result;
    result.m00 = homography.m00;
    result.m01 = homography.m01;
    result.m02 = homography.m02;
    result.m10 = homography.m10;
    result.m11 = homography.m11;
    result.m12 = homography.m12;
    result.m20 = homography.m20;
    result.m21 = homography.m21;
    result.m22 = homography.m22;
    return result;
}

@end
