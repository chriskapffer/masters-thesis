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

- (NSArray*)contentFromDirectory:(NSString*)directory;
- (NSArray*)filterFiles:(NSArray*)files WithExtension:(NSString*)extension;

@end

@implementation ResourceViewController

#pragma mark - properties

@synthesize tableView = _tableView;
@synthesize labelImage = _labelImage;
@synthesize labelVideo = _labelVideo;

@synthesize delegate = _delegate;
@synthesize resourceFolderPath = _resourceFolderPath;

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

    NSArray* resources = [self contentFromDirectory:self.resourceFolderPath];
    _imageArray = [self filterFiles:resources WithExtension:@"jpg"];
    _videoArray = [self filterFiles:resources WithExtension:@"mov"];
    _selectedImageIndex = 0;
    _selectedVideoIndex = 0;
    _prevSelectedImageIndex = -1;
    _prevSelectedVideoIndex = -1;
    _labelImage.text = @"";
    _labelVideo.text = @"";
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    self.tableView = nil;
    self.labelImage = nil;
    self.labelVideo = nil;
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    
    self.labelImage.text = [self.imageArray objectAtIndex:self.selectedImageIndex];
    self.labelVideo.text = [self.videoArray objectAtIndex:self.selectedVideoIndex];
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
        self.labelImage.text = [self.imageArray objectAtIndex:indexPath.row];
    } else {
        self.selectedVideoIndex = indexPath.row;
        self.labelVideo.text = [self.videoArray objectAtIndex:indexPath.row];
    }
}

#pragma mark - interface builder action

- (IBAction)cancelButtonClicked:(id)sender
{
    self.selectedImageIndex = MAX(self.prevSelectedImageIndex, 0);
    self.selectedVideoIndex = MAX(self.prevSelectedVideoIndex, 0);
    
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

- (NSArray*)contentFromDirectory:(NSString*)directory
{
    NSString *bundleRoot = [NSString stringWithFormat:@"%@/%@", [[NSBundle mainBundle] bundlePath], directory];
    return [[NSFileManager defaultManager] contentsOfDirectoryAtPath:bundleRoot error:nil];
}

- (NSArray*)filterFiles:(NSArray*)files WithExtension:(NSString*)extension
{
    NSString* predicateFormat = [NSString stringWithFormat:@"self ENDSWITH '.%@'", extension];
    NSPredicate *filter = [NSPredicate predicateWithFormat:predicateFormat];
    return [files filteredArrayUsingPredicate:filter];
}

@end
