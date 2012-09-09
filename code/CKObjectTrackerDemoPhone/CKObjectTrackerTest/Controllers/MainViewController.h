//
//  MainViewController.h
//  CKObjectTrackerTest
//
//  Created by Christoph Kapffer on 17.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface MainViewController : UIViewController <UIScrollViewDelegate>

@property (nonatomic, strong) IBOutlet UITextView* textView;
@property (nonatomic, strong) IBOutlet UIScrollView* scrollView;
@property (nonatomic, strong) IBOutlet UIPageControl* pageControl;
@property (nonatomic, strong) IBOutlet UINavigationItem* navBarItem;

- (IBAction)showSettingsButtonClicked:(id)sender;
- (IBAction)resourcePickerButtonClicked:(id)sender;

@end
