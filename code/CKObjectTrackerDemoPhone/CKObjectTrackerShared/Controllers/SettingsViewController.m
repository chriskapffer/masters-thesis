//
//  SettingsViewController.m
//  CKObjectTrackerDemoPhone
//
//  Created by Christoph Kapffer on 08.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import "SettingsViewController.h"

#import "ObjectTrackerLibrary.h"
#import "ObjectTrackerParameter.h"

#import "ParameterSliderCell.h"
#import "ParameterSwitchCell.h"
#import "ParameterSegmentCell.h"

#import "UIView+EasyFrame.h"

#define SliderCellIdentifier NSStringFromClass([ParameterSliderCell class])
#define SwitchCellIdentifier NSStringFromClass([ParameterSwitchCell class])
#define SegmentCellIdentifier NSStringFromClass([ParameterSegmentCell class])

#define EMPTY_TITLE_HEADER @"General"

@interface SettingsViewController () <UITableViewDataSource, UITableViewDelegate, UIScrollViewDelegate, ParameterCellDelegate>

@property (nonatomic, assign) BOOL changedCriticalParameter;
@property (nonatomic, strong) NSMutableArray* tableViews;
@property (nonatomic, strong, readonly) ObjectTrackerParameterCollection* parameters;

@end

@implementation SettingsViewController

#pragma mark - properties

@synthesize scrollView = _scrollView;
@synthesize pageControl = _pageControl;
@synthesize navBarItem = _navBarItem;
@synthesize activityIndicator = _activityIndicator;

@synthesize changedCriticalParameter = _changedCriticalParameter;
@synthesize parameters = _parameters;
@synthesize delegate = _delegate;

@synthesize tableViews = _tableViews;

- (ObjectTrackerParameterCollection*)parameters
{
    return [[ObjectTrackerLibrary instance] parameters];
}

#pragma mark - view lifecycle


- (void)viewDidLoad
{
    [super viewDidLoad];
    
    self.tableViews = [NSMutableArray array];
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    
    self.scrollView = nil;
    self.pageControl = nil;
    self.navBarItem = nil;
    self.activityIndicator = nil;
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    [self.activityIndicator setHidden:NO];
    [self.activityIndicator startAnimating];
    [self setChangedCriticalParameter:NO];
}

- (void)viewDidAppear:(BOOL)animated
{
    [self recreateTableViewsWithCount:self.parameters.subCollections.count];
    [self updateNavBarItem];
    [self.activityIndicator stopAnimating];
    [self.activityIndicator setHidden:YES];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

#pragma mark - table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    ObjectTrackerParameterCollection* collection = [self topLevelCollectionFromTableView:tableView];
    return collection.subCollections.count + 1;
}

- (NSString*)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
    ObjectTrackerParameterCollection* collection = [self topLevelCollectionFromTableView:tableView];
    if (section == 0) { return EMPTY_TITLE_HEADER; }
    return [(ObjectTrackerParameterCollection*)[collection.subCollections objectAtIndex:section - 1] name];
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    ObjectTrackerParameterCollection* collection = [self topLevelCollectionFromTableView:tableView];
    if (section == 0) { return collection.parameters.count; }
    collection = [collection.subCollections objectAtIndex:section - 1];
    return collection.parameters.count;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    ObjectTrackerParameter* parameter = [self parameterFromTableView:tableView AndIndexPath:indexPath];
    return [self cellHeightForParameter:parameter];
}

- (UITableViewCell*)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    ObjectTrackerParameter* parameter = [self parameterFromTableView:tableView AndIndexPath:indexPath];
    ParameterCell* cell;
    switch (parameter.type) {
        case ObjectTrackerParameterTypeBool:
            cell = [self cellFromBoolParameter:parameter WithTableView:tableView];
            break;
        case ObjectTrackerParameterTypeInt:
            cell = [self cellFromIntParameter:parameter WithTableView:tableView];
            break;
        case ObjectTrackerParameterTypeFloat:
            cell = [self cellFromFloatParameter:parameter WithTableView:tableView];
            break;
        case ObjectTrackerParameterTypeString:
            cell = [self cellFromStringParameter:parameter WithTableView:tableView];
            break;
    }
    [cell setSelectionStyle:UITableViewCellSelectionStyleNone];
    [cell setDelegate:self];
    return cell;
}

#pragma mark - interface builder actions

- (IBAction)doneButtonClicked:(id)sender
{
    if ([self.delegate respondsToSelector:@selector(settingsControllerFinishedWithCriticalParameterChange:)]) {
        [self.delegate settingsControllerFinishedWithCriticalParameterChange:self.changedCriticalParameter];
    }
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

#pragma mark - parameter cell delegate

- (void)parameterCellValueDidChange:(ParameterCell *)cell
{
    ObjectTrackerLibrary* objectTrackerLibrary = [ObjectTrackerLibrary instance];
    
    if ([cell isKindOfClass:[ParameterSwitchCell class]]) {
        ParameterSwitchCell* switchCell = (ParameterSwitchCell*)cell;
        [objectTrackerLibrary setBoolParameterWithName:cell.name Value:switchCell.value];
    } else if ([cell isKindOfClass:[ParameterSegmentCell class]]) {
        ParameterSegmentCell* segmentCell = (ParameterSegmentCell*)cell;
        if (segmentCell.isString) {
            [objectTrackerLibrary setStringParameterWithName:cell.name Value:segmentCell.value];
        } else if (segmentCell.isInt) {
            [objectTrackerLibrary setintParameterWithName:cell.name Value:[segmentCell.value intValue]];
        }
    } else if ([cell isKindOfClass:[ParameterSliderCell class]]) {
        ParameterSliderCell* sliderCell = (ParameterSliderCell*)cell;
        if (sliderCell.isInt) {
            [objectTrackerLibrary setintParameterWithName:cell.name Value:(int)sliderCell.value];
        } else if (sliderCell.isFloat) {
            [objectTrackerLibrary setFloatParameterWithName:cell.name Value:sliderCell.value];
        }
    }
    if (cell.critical) {
        self.changedCriticalParameter = YES;
        if ([cell.superview isKindOfClass:[UITableView class]]) {
            // TODO: think of something better, because this takes too long
            UITableView* tableView = (UITableView*)cell.superview;
            NSIndexPath* indexPath = [tableView indexPathForCell:cell];
            int rowsInSection = [tableView.dataSource tableView:tableView numberOfRowsInSection:indexPath.section];
            NSMutableArray* indexPaths = [NSMutableArray arrayWithCapacity:rowsInSection];
            for (int i = 0; i < rowsInSection; i++) {
                [indexPaths addObject:[NSIndexPath indexPathForRow:i inSection:indexPath.section]];
            };
            [tableView reloadRowsAtIndexPaths:indexPaths withRowAnimation:UITableViewRowAnimationNone];
        }
    }
}

#pragma mark - parameter related methods

- (int)cellHeightForParameter:(ObjectTrackerParameter*)parameter
{
    int height = 0;
    switch (parameter.type) {
        case ObjectTrackerParameterTypeBool:
            height = PARAMETER_SWITCH_CELL_HEIGHT;
            break;
        case ObjectTrackerParameterTypeInt:
            height = (parameter.intValues.count > 0)
                ? PARAMETER_SEGMENT_CELL_HEIGHT
                : PARAMETER_SLIDER_CELL_HEIGHT;
            break;
        case ObjectTrackerParameterTypeFloat:
            height = PARAMETER_SLIDER_CELL_HEIGHT;
            break;
        case ObjectTrackerParameterTypeString:
            height = PARAMETER_SEGMENT_CELL_HEIGHT;
            break;
    }
    return height;
}

- (ParameterCell*)cellFromBoolParameter:(ObjectTrackerParameter*)parameter WithTableView:(UITableView *)tableView
{
    assert(parameter.type == ObjectTrackerParameterTypeBool);
    ParameterSwitchCell* cell = [tableView dequeueReusableCellWithIdentifier:SwitchCellIdentifier];
    [cell setValue:parameter.boolValue];
    
    [cell setName:parameter.name];
    [cell setCritical:parameter.critical];
    return cell;
}

- (ParameterCell*)cellFromIntParameter:(ObjectTrackerParameter*)parameter WithTableView:(UITableView *)tableView
{
    assert(parameter.type == ObjectTrackerParameterTypeInt);
    ParameterCell* cell;
    if (parameter.intValues.count > 0) {
        ParameterSegmentCell* segmentCell = [tableView dequeueReusableCellWithIdentifier:SegmentCellIdentifier];
        [segmentCell setValues:[self stringArrayFromIntArray:parameter.intValues]];
        [segmentCell setValue:[NSString stringWithFormat:@"%d", parameter.intValue]];
        [segmentCell setIsInt:YES];
        cell = segmentCell;
    } else {
        ParameterSliderCell* sliderCell = [tableView dequeueReusableCellWithIdentifier:SliderCellIdentifier];
        [sliderCell setMinValue:parameter.intMin];
        [sliderCell setMaxValue:parameter.intMax];
        [sliderCell setValue:parameter.intValue];
        [sliderCell setIsInt:YES];
        cell = sliderCell;
    }
    [cell setName:parameter.name];
    [cell setCritical:parameter.critical];
    return cell;
}

- (ParameterCell*)cellFromFloatParameter:(ObjectTrackerParameter*)parameter WithTableView:(UITableView *)tableView
{
    assert(parameter.type == ObjectTrackerParameterTypeFloat);
    ParameterSliderCell* cell = [tableView dequeueReusableCellWithIdentifier:SliderCellIdentifier];
    [cell setMinValue:parameter.floatMin];
    [cell setMaxValue:parameter.floatMax];
    [cell setValue:parameter.floatValue];
    [cell setIsFloat:YES];
    
    [cell setName:parameter.name];
    [cell setCritical:parameter.critical];
    return cell;
}

- (ParameterCell*)cellFromStringParameter:(ObjectTrackerParameter*)parameter WithTableView:(UITableView *)tableView
{
    assert(parameter.type == ObjectTrackerParameterTypeString);
    ParameterSegmentCell* cell = [tableView dequeueReusableCellWithIdentifier:SegmentCellIdentifier];
    [cell setValues:parameter.stringValues];
    [cell setValue:parameter.stringValue];
    [cell setIsString:YES];
    
    [cell setName:parameter.name];
    [cell setCritical:parameter.critical];
    return cell;
}

#pragma mark - helper methods

- (void)recreateTableViewsWithCount:(int)count
{
    // remove old table views
    for (UITableView* tableView in self.tableViews) {
        [tableView removeFromSuperview];
    }
    [self.tableViews removeAllObjects];
    
    // add new table views
    int width = self.scrollView.frame.size.width;
    for (int i = 0; i < count; i++) {
        UITableView* tableView = [[UITableView alloc] initWithFrame:self.scrollView.bounds
                                                              style:UITableViewStyleGrouped];
        [tableView registerNib:[[self class] parameterSliderCellNib] forCellReuseIdentifier:SliderCellIdentifier];
        [tableView registerNib:[[self class] parameterSwitchCellNib] forCellReuseIdentifier:SwitchCellIdentifier];
        [tableView registerNib:[[self class] parameterSegmentCellNib] forCellReuseIdentifier:SegmentCellIdentifier];
        [tableView setBackgroundColor:[UIColor groupTableViewBackgroundColor]];
        [tableView setFrameOriginX:width * i];
        [tableView setDataSource:self];
        [tableView setDelegate:self];
        [self.scrollView addSubview:tableView];
        [self.tableViews addObject:tableView];
    }
    
    [self.pageControl setCurrentPage:0];
    [self.pageControl setNumberOfPages:self.tableViews.count];
    [self.scrollView setContentSize:CGSizeMake(self.tableViews.count * width, self.scrollView.frame.size.height)];
}

- (void)updateNavBarItem
{
    [self.navBarItem setTitle:[(ObjectTrackerParameterCollection*)[self.parameters.subCollections objectAtIndex:self.pageControl.currentPage] name]];
}

- (ObjectTrackerParameterCollection*)topLevelCollectionFromTableView:(UITableView *)tableView
{
    return [self.parameters.subCollections objectAtIndex:[self.tableViews indexOfObject:tableView]];
}

- (ObjectTrackerParameter*)parameterFromTableView:(UITableView *)tableView AndIndexPath:(NSIndexPath *)indexPath
{
    ObjectTrackerParameterCollection* collection = [self topLevelCollectionFromTableView:tableView];
    
    NSArray* parameters;
    if (indexPath.section == 0) {
        parameters = collection.parameters;
    } else {
        ObjectTrackerParameterCollection* subCollection;
        subCollection = [collection.subCollections objectAtIndex:indexPath.section - 1];
        parameters = subCollection.parameters;
    }
    
    return [parameters objectAtIndex:indexPath.row];
}

- (NSArray*)stringArrayFromIntArray:(NSArray*)intArray
{
    NSMutableArray* stringArray = [NSMutableArray arrayWithCapacity:intArray.count];
    for (NSNumber* number in intArray) {
        [stringArray addObject:[NSString stringWithFormat:@"%d", [number intValue]]];
    }
    return stringArray;
}

+ (UINib*)parameterSliderCellNib
{
    static dispatch_once_t pred = 0;
    __strong static UINib* _sharedNibObject = nil;
    dispatch_once(&pred, ^{
        _sharedNibObject = [UINib nibWithNibName:NSStringFromClass([ParameterSliderCell class]) bundle:nil];
    });
    return _sharedNibObject;
}

+ (UINib*)parameterSwitchCellNib
{
    static dispatch_once_t pred = 0;
    __strong static UINib* _sharedNibObject = nil;
    dispatch_once(&pred, ^{
        _sharedNibObject = [UINib nibWithNibName:NSStringFromClass([ParameterSwitchCell class]) bundle:nil];
    });
    return _sharedNibObject;
}

+ (UINib*)parameterSegmentCellNib
{
    static dispatch_once_t pred = 0;
    __strong static UINib* _sharedNibObject = nil;
    dispatch_once(&pred, ^{
        _sharedNibObject = [UINib nibWithNibName:NSStringFromClass([ParameterSegmentCell class]) bundle:nil];
    });
    return _sharedNibObject;
}

@end
