//
//  MainViewController.m
//  CKObjectTrackerTest
//
//  Created by Christoph Kapffer on 17.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import "MainViewController.h"
#import "SettingsViewController.h"
#import "ResourceViewController.h"

#import "ObjectTrackerLibrary.h"
#import "VideoReader.h"

#import "UIView+EasyFrame.h"
#import "UIImage+Transform.h"
#import "UIImage+PixelBuffer.h"

#define RESOURCE_FOLDER_NAME @"testdata"

@interface MainViewController () <VideoReaderDelegate, ObjectTrackerLibraryDelegate, SettingsViewControllerDelegate, ResourceViewControllerDelegate>

@property (nonatomic, strong) SettingsViewController* settingsController;
@property (nonatomic, strong) ResourceViewController* resourceController;
@property (nonatomic, strong) VideoReader* videoReader;

@property (nonatomic, strong) UIImageView* debugViewRawData;
@property (nonatomic, strong) UIImageView* debugViewTracking;
@property (nonatomic, strong) UIImageView* debugViewValidation;
@property (nonatomic, strong) UIImageView* debugViewDetection;
@property (nonatomic, strong) UIImageView* debugViewObject;
@property (nonatomic, strong) NSArray* imageViewNames;

- (void)startNewVideoSession:(NSString*)videoName;
- (void)setNewObjectImage:(NSString*)imageName;

@end

@implementation MainViewController

#pragma mark - properties

@synthesize textView = _textView;
@synthesize scrollView = _scrollView;
@synthesize pageControl = _pageControl;
@synthesize navBarItem = _navBarItem;

@synthesize settingsController = _settingsController;
@synthesize resourceController = _resourceController;
@synthesize videoReader = _videoReader;

@synthesize debugViewRawData = _debugViewRawData;
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
    
    self.imageViewNames = [NSArray arrayWithObjects:
                           @"Object Image", @"Statistics", @"Raw Data", @"Tracking View", @"Validation View", @"Detection View", nil];
    
    self.debugViewObject = [self registeredImageViewWithIndex:0];
    [self.textView setFrameOrigin:CGPointMake(self.scrollView.bounds.size.width, 0)]; // <-- index 1
    self.debugViewRawData = [self registeredImageViewWithIndex:2];
    self.debugViewTracking = [self registeredImageViewWithIndex:3];
    self.debugViewValidation = [self registeredImageViewWithIndex:4];
    self.debugViewDetection = [self registeredImageViewWithIndex:5];
    self.scrollView.contentSize = CGSizeMake(self.scrollView.bounds.size.width * 6, self.scrollView.bounds.size.height);
    self.scrollView.contentOffset = CGPointMake(self.scrollView.bounds.size.width * 2, 0);
    self.pageControl.numberOfPages = 6;
    self.pageControl.currentPage = 2;
    self.textView.text = @"";
    
    self.settingsController = nil;
    self.resourceController = nil;
    self.videoReader = [[VideoReader alloc] init];
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
    
    if (self.resourceController != nil)
        self.resourceController = nil;
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    
    [self updateNavBarItem];
    
    [[ObjectTrackerLibrary instance] setDelegate:self];
    self.videoReader.delegate = self;
    self.videoReader.paused = NO;
}

- (void)viewDidDisappear:(BOOL)animated
{
    [super viewDidDisappear:animated];

    self.videoReader.paused = YES;
    self.videoReader.delegate = nil;
    [[ObjectTrackerLibrary instance] setDelegate:nil];
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

- (IBAction)resourcePickerButtonClicked:(id)sender
{
    if (self.resourceController == nil) {
        self.resourceController = [[ResourceViewController alloc] init];
        self.resourceController.modalTransitionStyle = UIModalTransitionStyleFlipHorizontal;
        self.resourceController.resourceFolderPath = RESOURCE_FOLDER_NAME;
        self.resourceController.delegate = self;
    }
    
    [self presentViewController:self.resourceController animated:YES completion:nil];
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

#pragma mark - settings view controller delegate

- (void)settingsControllerfinished
{
    [self dismissViewControllerAnimated:YES completion:nil];
}

#pragma mark - resource view controller delegate

- (void)resourceControllerCanceled:(ResourceViewController*)resourceController
{
    [self dismissViewControllerAnimated:YES completion:nil];
}

- (void)resourceController:(ResourceViewController*)resourceController SelectedImage:(NSString*)imageName Video:(NSString*)videoName
{
    [self dismissViewControllerAnimated:YES completion:nil];
    [self.videoReader stopReading];
    [self setNewObjectImage:imageName];
    [self startNewVideoSession:videoName];
}

#pragma mark - video reader delegate

- (void)didReadFrameWithPixelBuffer:(CVPixelBufferRef)pixelBuffer
{
    __block CVPixelBufferRef retainedBuffer = CVPixelBufferRetain(pixelBuffer);
    dispatch_async(dispatch_get_main_queue(), ^{
        UIImage* image = [UIImage imageFromPixelBuffer:retainedBuffer];
        CVPixelBufferRelease(retainedBuffer);
        [self.debugViewRawData setImage:[image rotatedImageWithAngle:M_PI_2]];
        //[self.debugViewRawData setImage:[self.videoReader imageFromPixelBuffer:pixelBuffer]];
    });
    [[ObjectTrackerLibrary instance] trackObjectInVideoWithBuffer:pixelBuffer];
}

#pragma mark - object tracker library delegate

- (void)trackerLibraryDidProcessFrame
{
    dispatch_async(dispatch_get_main_queue(), ^{
        UIImage* debugImage;
        if ([[ObjectTrackerLibrary instance] detectionDebugImage:&debugImage WithSearchWindow:YES]) {
            [self.debugViewDetection setImage:debugImage];
        }
        if ([[ObjectTrackerLibrary instance] validationDebugImage:&debugImage WithObjectRect:YES ObjectKeyPoints:YES SceneKeyPoints:YES FilteredMatches:YES AllMatches:YES]) {
            [self.debugViewValidation setImage:debugImage];
        }
        if ([[ObjectTrackerLibrary instance] trackingDebugImage:&debugImage WithObjectRect:YES FilteredPoints:YES AllPoints:YES SearchWindow:NO]) {
            [self.debugViewTracking setImage:debugImage];
        }
        self.textView.text = [[ObjectTrackerLibrary instance] frameDebugInfoString];
    });
    //NSLog(@"\n%@", [[ObjectTrackerLibrary instance] frameDebugInfoString]);
}

#pragma mark - helper methods

- (UIImageView*)registeredImageViewWithIndex:(int)index
{
    UIImageView* imageView = [[UIImageView alloc] initWithFrame:self.scrollView.bounds];
    [imageView setFrameOriginX:self.scrollView.bounds.size.width * index];
    [imageView setTransform:CGAffineTransformMakeRotation(M_PI)];
    [imageView setContentMode:UIViewContentModeScaleAspectFit];
    [self.scrollView addSubview:imageView];
    return imageView;
}

- (void)updateNavBarItem
{
    [self.navBarItem setTitle:[self.imageViewNames objectAtIndex:self.pageControl.currentPage]];
}

- (void)startNewVideoSession:(NSString*)videoName
{
    videoName = [videoName stringByDeletingPathExtension];
    NSString* fullVideoName = [NSString stringWithFormat:@"%@/%@", RESOURCE_FOLDER_NAME, videoName];
    NSURL *url = [[NSBundle mainBundle] URLForResource:fullVideoName withExtension:@"mov"];

    [[ObjectTrackerLibrary instance] clearVideoDebugInfo];
    [[UIApplication sharedApplication] setIdleTimerDisabled:YES];
    [self.videoReader readVideoWithURL:url Completion:^{
        NSLog(@"\nDONE\n\n%@", [[ObjectTrackerLibrary instance] videoDebugInfoString]);
        self.textView.text = [[ObjectTrackerLibrary instance] videoDebugInfoString];
        [[UIApplication sharedApplication] setIdleTimerDisabled:NO];
    }];
}

- (void)setNewObjectImage:(NSString*)imageName
{
    NSString* fullImageName = [NSString stringWithFormat:@"%@/%@", RESOURCE_FOLDER_NAME, imageName];
    UIImage* objectImage = [UIImage imageNamed:fullImageName];
    [[ObjectTrackerLibrary instance] setObjectImageWithImage:objectImage];
    [[ObjectTrackerLibrary instance] clearVideoDebugInfo];
    
    [self.debugViewObject setImage:[objectImage rotatedImageWithAngle:M_PI_2]];
}

@end
