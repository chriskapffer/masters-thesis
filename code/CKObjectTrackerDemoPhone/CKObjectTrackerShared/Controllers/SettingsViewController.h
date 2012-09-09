//
//  SettingsViewController.h
//  CKObjectTrackerDemoPhone
//
//  Created by Christoph Kapffer on 08.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import <UIKit/UIKit.h>

@protocol SettingsViewControllerDelegate <NSObject>

- (void)settingsControllerfinished;

@end

@class ObjectTrackerParameterCollection;

@interface SettingsViewController : UIViewController <UIScrollViewDelegate>

@property (nonatomic, strong) IBOutlet UIScrollView* scrollView;
@property (nonatomic, strong) IBOutlet UIPageControl* pageControl;
@property (nonatomic, strong) IBOutlet UINavigationItem* navBarItem;
@property (nonatomic, weak) id<SettingsViewControllerDelegate> delegate;

- (IBAction)doneButtonClicked:(id)sender;

@end
