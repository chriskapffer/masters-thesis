//
//  StartViewController.m
//  CVApplication
//
//  Created by Christoph Kapffer on 06.06.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import "StartViewController.h"
#import "MainViewController.h"

#import "CaptureManager.h"

@interface StartViewController ()

@end

@implementation StartViewController

@synthesize activityIndicator = _activityIndicator;

- (void)viewDidLoad
{
    [super viewDidLoad];

    [[UIApplication sharedApplication] setStatusBarHidden:YES withAnimation:NO];
}

- (void)viewDidUnload
{
    [super viewDidUnload];

    self.activityIndicator = nil;
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    
    [self.activityIndicator startAnimating];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(captureSessionDidStartRunning) name:AVCaptureSessionDidStartRunningNotification object:nil];
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    
    [[CaptureManager instance] setUpAndStartCaptureSession];
}

- (void)viewDidDisappear:(BOOL)animated
{
    [super viewDidDisappear:animated];
    
    [self.activityIndicator stopAnimating];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:AVCaptureSessionDidStartRunningNotification object:nil];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return YES;
}

#pragma mark - Capture session nofifications

- (void)captureSessionDidStartRunning
{
    MainViewController* mainViewController = [[MainViewController alloc] init];
    [mainViewController setModalPresentationStyle:UIModalPresentationFullScreen];
    [mainViewController setModalTransitionStyle:UIModalTransitionStyleCrossDissolve];
    [self presentModalViewController:mainViewController animated:YES];
}

@end
