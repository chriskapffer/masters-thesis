//
//  ResourceViewController.m
//  CKObjectTrackerTest
//
//  Created by Christoph Kapffer on 10.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import "ResourceViewController.h"

@interface ResourceViewController ()

@property (nonatomic, strong) NSArray* imageArray;
@property (nonatomic, strong) NSArray* videoArray;

@property (nonatomic, assign) int selectedImageIndex;
@property (nonatomic, assign) int selectedVideoIndex;
@property (nonatomic, assign) int prevSelectedImageIndex;
@property (nonatomic, assign) int prevSelectedVideoIndex;

@end

@implementation ResourceViewController

#pragma mark - properties

@synthesize tableView = _tableView;
@synthesize delegate = _delegate;

@synthesize imageArray = _imageArray;
@synthesize videoArray = _videoArray;
@synthesize selectedImageIndex = _selectedImageIndex;
@synthesize selectedVideoIndex = _selectedVideoIndex;
@synthesize prevSelectedImageIndex = _prevSelectedImageIndex;
@synthesize prevSelectedVideoIndex = _prevSelectedVideoIndex;

#pragma mark - view lifecycle

- (void)viewDidLoad
{
    [super viewDidLoad];

    _imageArray = [self createImageArray];
    _videoArray = [self createVideoArray];
    _selectedImageIndex = 0;
    _selectedVideoIndex = 0;
    _prevSelectedImageIndex = -1;
    _prevSelectedVideoIndex = -1;
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    self.tableView = nil;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 2;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return (section == 0) ? self.imageArray.count : self.videoArray.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *CellIdentifier = @"ResourceCell";
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier];
    }
    
    int selectedIndex = (indexPath.section == 0) ? self.selectedImageIndex : self.selectedVideoIndex;
    NSArray* sourceArray = (indexPath.section == 0) ? self.imageArray : self.videoArray;
    cell.textLabel.text = [sourceArray objectAtIndex:indexPath.row];
    cell.accessoryType = (indexPath.row == selectedIndex)
        ? UITableViewCellAccessoryCheckmark
        : UITableViewCellAccessoryNone;
    return cell;
}

#pragma mark - Table view delegate

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    int prevSelectedIndex = (indexPath.section == 0) ? self.selectedImageIndex : self.selectedVideoIndex;
    NSIndexPath* prevSelectedIndexPath = [NSIndexPath indexPathForRow:prevSelectedIndex inSection:indexPath.section];
    [(UITableViewCell*)[tableView cellForRowAtIndexPath:prevSelectedIndexPath] setAccessoryType:UITableViewCellAccessoryNone];
    [(UITableViewCell*)[tableView cellForRowAtIndexPath:indexPath] setAccessoryType:UITableViewCellAccessoryCheckmark];
    
    if (indexPath.section == 0) {
        self.selectedImageIndex = indexPath.row;
    } else {
        self.selectedVideoIndex = indexPath.row;
    }
}

#pragma mark - interface builder action

- (IBAction)cancelButtonClicked:(id)sender
{
    self.selectedImageIndex = self.prevSelectedImageIndex;
    self.selectedVideoIndex = self.prevSelectedVideoIndex;
    
    if ([self.delegate respondsToSelector:@selector(resourceControllerCanceled:)])
        [self.delegate resourceControllerCanceled:self];
}

- (IBAction)commitButtonClicked:(id)sender
{
    NSString* imageName = [self.imageArray objectAtIndex:self.selectedImageIndex];
    NSString* videoName = [self.videoArray objectAtIndex:self.selectedVideoIndex];
    
    self.prevSelectedImageIndex = self.selectedImageIndex;
    self.prevSelectedVideoIndex = self.selectedVideoIndex;
    
    if ([self.delegate respondsToSelector:@selector(resourceController:SelectedImage:Video:)])
        [self.delegate resourceController:self SelectedImage:imageName Video:videoName];
}

#pragma mark - helper methods

- (NSArray*)createImageArray
{
    return [NSArray arrayWithObjects:
            @"img_204x153_book1",
            @"img_204x153_book2",
            @"img_204x153_pen",
            @"img_408x306_book1",
            @"img_408x306_book2",
            @"img_408x306_pen",
            @"img_816x612_book1",
            @"img_816x612_book2",
            @"img_816x612_pen",
            @"img_1632x1224_book1",
            @"img_1632x1224_book2",
            @"img_1632x1224_pen",
            @"img_3264x2448_book1",
            @"img_3264x2448_book2",
            @"img_3264x2448_pen",
            nil];
}

- (NSArray*)createVideoArray
{
    return [NSArray arrayWithObjects:
            @"vid_192x144_light_book1",
            @"vid_192x144_light_book2",
            @"vid_192x144_light_pen",
            @"vid_192x144_zoom_book1",
            @"vid_192x144_zoom_book2",
            @"vid_192x144_zoom_pen",
            @"vid_192x144_rotation_book1",
            @"vid_192x144_rotation_book2",
            @"vid_192x144_rotation_pen",
            @"vid_192x144_orbit_book1",
            @"vid_192x144_orbit_book2",
            @"vid_192x144_orbit_pen",
            @"vid_192x144_occlusion_book1",
            @"vid_192x144_occlusion_book2",
            @"vid_192x144_occlusion_pen",
            @"vid_192x144_enter_exit_book1",
            @"vid_192x144_enter_exit_book2",
            @"vid_192x144_enter_exit_pen",
            @"vid_192x144_fast_book1",
            @"vid_192x144_fast_book2",
            @"vid_192x144_fast_pen",
            @"vid_192x144_scene_still",
            @"vid_192x144_scene_motion",
            @"vid_352x288_light_book1",
            @"vid_352x288_light_book2",
            @"vid_352x288_light_pen",
            @"vid_352x288_zoom_book1",
            @"vid_352x288_zoom_book2",
            @"vid_352x288_zoom_pen",
            @"vid_352x288_rotation_book1",
            @"vid_352x288_rotation_book2",
            @"vid_352x288_rotation_pen",
            @"vid_352x288_orbit_book1",
            @"vid_352x288_orbit_book2",
            @"vid_352x288_orbit_pen",
            @"vid_352x288_occlusion_book1",
            @"vid_352x288_occlusion_book2",
            @"vid_352x288_occlusion_pen",
            @"vid_352x288_enter_exit_book1",
            @"vid_352x288_enter_exit_book2",
            @"vid_352x288_enter_exit_pen",
            @"vid_352x288_fast_book1",
            @"vid_352x288_fast_book2",
            @"vid_352x288_fast_pen",
            @"vid_352x288_scene_still",
            @"vid_352x288_scene_motion",
            @"vid_480x360_light_book1",
            @"vid_480x360_light_book2",
            @"vid_480x360_light_pen",
            @"vid_480x360_zoom_book1",
            @"vid_480x360_zoom_book2",
            @"vid_480x360_zoom_pen",
            @"vid_480x360_rotation_book1",
            @"vid_480x360_rotation_book2",
            @"vid_480x360_rotation_pen",
            @"vid_480x360_orbit_book1",
            @"vid_480x360_orbit_book2",
            @"vid_480x360_orbit_pen",
            @"vid_480x360_occlusion_book1",
            @"vid_480x360_occlusion_book2",
            @"vid_480x360_occlusion_pen",
            @"vid_480x360_enter_exit_book1",
            @"vid_480x360_enter_exit_book2",
            @"vid_480x360_enter_exit_pen",
            @"vid_480x360_fast_book1",
            @"vid_480x360_fast_book2",
            @"vid_480x360_fast_pen",
            @"vid_480x360_scene_still",
            @"vid_480x360_scene_motion",
            @"vid_640x480_light_book1",
            @"vid_640x480_light_book2",
            @"vid_640x480_light_pen",
            @"vid_640x480_zoom_book1",
            @"vid_640x480_zoom_book2",
            @"vid_640x480_zoom_pen",
            @"vid_640x480_rotation_book1",
            @"vid_640x480_rotation_book2",
            @"vid_640x480_rotation_pen",
            @"vid_640x480_orbit_book1",
            @"vid_640x480_orbit_book2",
            @"vid_640x480_orbit_pen",
            @"vid_640x480_occlusion_book1",
            @"vid_640x480_occlusion_book2",
            @"vid_640x480_occlusion_pen",
            @"vid_640x480_enter_exit_book1",
            @"vid_640x480_enter_exit_book2",
            @"vid_640x480_enter_exit_pen",
            @"vid_640x480_fast_book1",
            @"vid_640x480_fast_book2",
            @"vid_640x480_fast_pen",
            @"vid_640x480_scene_still",
            @"vid_640x480_scene_motion",
            nil];
}

@end
