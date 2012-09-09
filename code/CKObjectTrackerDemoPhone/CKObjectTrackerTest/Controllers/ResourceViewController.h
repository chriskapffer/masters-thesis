//
//  ResourceViewController.h
//  CKObjectTrackerTest
//
//  Created by Christoph Kapffer on 10.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import <UIKit/UIKit.h>

@protocol ResourceViewControllerDelegate;

@interface ResourceViewController : UIViewController <UITableViewDelegate, UITableViewDataSource>

@property (nonatomic, strong) IBOutlet UITableView* tableView;
@property (nonatomic, strong) IBOutlet UILabel* labelImage;
@property (nonatomic, strong) IBOutlet UILabel* labelVideo;

@property (nonatomic, assign) id<ResourceViewControllerDelegate> delegate;
@property (nonatomic, strong) NSString* resourceFolderPath;

- (IBAction)cancelButtonClicked:(id)sender;
- (IBAction)commitButtonClicked:(id)sender;

@end

@protocol ResourceViewControllerDelegate <NSObject>

- (void)resourceControllerCanceled:(ResourceViewController*)resourceController;
- (void)resourceController:(ResourceViewController*)resourceController SelectedImage:(NSString*)imageName Video:(NSString*)videoName;

@end