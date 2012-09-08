//
//  MainViewController.m
//  CKObjectTrackerDemo
//
//  Created by Christoph Kapffer on 17.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import "MainViewController.h"

#import "ObjectTrackerLibrary.h"
#import "CaptureManager.h"

#import "UIImage+Scaling.h"

@interface MainViewController () <CaptureManagerDelegate, ObjectTrackerLibraryDelegate, UIActionSheetDelegate, UIImagePickerControllerDelegate>

@end

@implementation MainViewController

#pragma mark - properties

@synthesize imageView = _imageView;

#pragma mark - view lifecycle

- (void)viewDidLoad
{
    [super viewDidLoad];
    [[UIApplication sharedApplication] setStatusBarHidden:NO withAnimation:NO];
    
    [[CaptureManager instance] setPixelFormat:kCVPixelFormatType_32BGRA];
    [[CaptureManager instance] setSessionPreset:AVCaptureSessionPresetMedium];
    [[CaptureManager instance] setDelegate:self];
    [[ObjectTrackerLibrary instance] setDelegate:self]; 
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    
    self.imageView = nil;
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

- (IBAction)objectImagePickerButtonClicked:(id)sender
{
    UIActionSheet* actionSheet = [[UIActionSheet alloc] initWithTitle:@"Pick an image of the object to track." delegate:self cancelButtonTitle:@"Cancel" destructiveButtonTitle:nil otherButtonTitles:@"From Camera", @"From Photo Album", nil];
    [actionSheet showInView:self.view];
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
    UIImage* image = [info objectForKey:UIImagePickerControllerEditedImage];
    if (!image) { image = [info objectForKey:UIImagePickerControllerOriginalImage]; }
    
    [[ObjectTrackerLibrary instance] setObjectImageWithImage:[image scaledImageWithSize:targetSize]];
    [[ObjectTrackerLibrary instance] clearVideoDebugInfo];
    [self dismissViewControllerAnimated:YES completion:nil];
}

- (void)imagePickerControllerDidCancel:(UIImagePickerController *)picker
{
    [self dismissViewControllerAnimated:YES completion:nil];
}

#pragma mark - capture manager delegate

- (void)didCaptureFrameWithPixelBuffer:(CVPixelBufferRef)pixelBuffer
{
    [[ObjectTrackerLibrary instance] trackObjectInVideoWithBuffer:pixelBuffer];
}

#pragma mark - object tracker library delegate

- (void)trackerLibraryDidProcessFrame
{
    UIImage* debugImage;
    if ([[ObjectTrackerLibrary instance] trackingDebugImage:&debugImage WithObjectRect:YES FilteredPoints:YES AllPoints:NO SearchWindow:NO]) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [self.imageView setImage:debugImage];
        });
    }
    NSLog(@"\n%@", [[ObjectTrackerLibrary instance] frameDebugInfoString]);
}

@end
