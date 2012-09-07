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

@interface MainViewController () <ResourceViewControllerDelegate, VideoReaderDelegate>

@property (nonatomic, strong) ResourceViewController* resourceController;
@property (nonatomic, strong) VideoReader* videoReader;

@end

@implementation MainViewController

#pragma mark - properties

@synthesize resourceController = _resourceController;
@synthesize videoReader = _videoReader;

#pragma mark - view lifecycle

- (void)viewDidLoad
{
    [super viewDidLoad];

    self.resourceController = nil;
    self.videoReader = [[VideoReader alloc] init];
    self.videoReader.delegate = self;
}

- (void)viewDidUnload
{
    [super viewDidUnload];

    if (self.resourceController != nil)
        self.resourceController = nil;
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
    
    [self setNewReferenceImage:imageName];
    [self startNewVideoSession:videoName];
}

#pragma mark - video reader delegate

- (void)didReadFrameWithPixelBuffer:(CVPixelBufferRef)pixelBuffer
{
    [[ObjectTrackerLibrary instance] trackObjectInVideoWithBuffer:pixelBuffer];
}

#pragma mark - helper methods

- (void)startNewVideoSession:(NSString*)videoName
{
    [[ObjectTrackerLibrary instance] clearVideoDebugInfo];
    NSURL *url = [[NSBundle mainBundle] URLForResource:videoName withExtension:@"mov"];

    [[UIApplication sharedApplication] setIdleTimerDisabled:YES];
    [self.videoReader readVideoWithURL:url Completion:^{
        NSLog(@"\nDONE\n\n%@", [[ObjectTrackerLibrary instance] videoDebugInfoString]);
        [[UIApplication sharedApplication] setIdleTimerDisabled:NO];
    }];
}

- (void)setNewReferenceImage:(NSString*)imageName
{
    NSString* fullImageName = [imageName stringByAppendingString:@".jpg"];
    UIImage* image = [UIImage imageWithCGImage:[UIImage imageNamed:fullImageName].CGImage scale:1.0 orientation:UIImageOrientationLeft];
    
    [[ObjectTrackerLibrary instance] setObjectImageWithImage:image];
}

@end
