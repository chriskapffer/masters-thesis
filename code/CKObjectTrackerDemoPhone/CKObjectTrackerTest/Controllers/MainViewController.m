//
//  MainViewController.m
//  CKObjectTrackerTest
//
//  Created by Christoph Kapffer on 17.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import "MainViewController.h"
#import "ResourceViewController.h"

#import "ObjectTrackerLibrary.h"
#import "VideoReader.h"

#define RESOURCE_FOLDER_NAME @"testdata"

@interface MainViewController () <VideoReaderDelegate, ObjectTrackerLibraryDelegate, ResourceViewControllerDelegate>

@property (nonatomic, strong) ResourceViewController* resourceController;
@property (nonatomic, strong) VideoReader* videoReader;

- (void)startNewVideoSession:(NSString*)videoName;
- (void)setNewObjectImage:(NSString*)imageName;

@end

@implementation MainViewController

#pragma mark - properties

@synthesize resourceController = _resourceController;
@synthesize videoReader = _videoReader;
@synthesize imageView = _imageView;

#pragma mark - view lifecycle

- (void)viewDidLoad
{
    [super viewDidLoad];
    [[UIApplication sharedApplication] setStatusBarHidden:NO withAnimation:NO];
    
    self.imageView.transform = CGAffineTransformMakeRotation(M_PI); // recorded videos and images are flipped
    
    self.resourceController = nil;
    self.videoReader = [[VideoReader alloc] init];
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    
    self.imageView = nil;
    
    if (self.resourceController != nil)
        self.resourceController = nil;
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];

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
    [[ObjectTrackerLibrary instance] trackObjectInVideoWithBuffer:pixelBuffer];
}

#pragma mark - object tracker library delegate

- (void)trackerLibraryDidProcessFrame
{
    dispatch_async(dispatch_get_main_queue(), ^{
        UIImage* debugImage;
        if ([[ObjectTrackerLibrary instance] trackingDebugImage:&debugImage WithObjectRect:YES FilteredPoints:YES AllPoints:NO SearchWindow:NO]) {
            [self.imageView setImage:debugImage];
        }
    });
    //NSLog(@"\n%@", [[ObjectTrackerLibrary instance] frameDebugInfoString]);
}

#pragma mark - helper methods

- (void)startNewVideoSession:(NSString*)videoName
{
    //videoName = [videoName stringByDeletingPathExtension];
    //NSString* fullVideoName = [NSString stringWithFormat:@"%@/%@", RESOURCE_FOLDER_NAME, videoName];
    NSString* fullVideoName = @"testdata/vid_480x360_light_book1";
    NSURL *url = [[NSBundle mainBundle] URLForResource:fullVideoName withExtension:@"mov"];

    [[ObjectTrackerLibrary instance] clearVideoDebugInfo];
    [[UIApplication sharedApplication] setIdleTimerDisabled:YES];
    [self.videoReader readVideoWithURL:url Completion:^{
        NSLog(@"\nDONE\n\n%@", [[ObjectTrackerLibrary instance] videoDebugInfoString]);
        [[UIApplication sharedApplication] setIdleTimerDisabled:NO];
    }];
}

- (void)setNewObjectImage:(NSString*)imageName
{
    //NSString* fullImageName = [NSString stringWithFormat:@"%@/%@", RESOURCE_FOLDER_NAME, imageName];
    NSString* fullImageName = @"testdata/img_408x306_book1.jpg";
    [[ObjectTrackerLibrary instance] setObjectImageWithImage:[UIImage imageNamed:fullImageName]];
    [[ObjectTrackerLibrary instance] clearVideoDebugInfo];
}

@end
