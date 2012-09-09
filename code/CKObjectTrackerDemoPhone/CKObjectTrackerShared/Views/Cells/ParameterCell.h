//
//  ParameterCell.h
//  CKObjectTrackerDemoPhone
//
//  Created by Christoph Kapffer on 09.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import <UIKit/UIKit.h>

@protocol ParameterCellDelegate;

@interface ParameterCell : UITableViewCell

@property (nonatomic, strong) IBOutlet UILabel* nameLabel;
@property (nonatomic, strong) NSString* name;
@property (nonatomic, assign) BOOL critical;
@property (nonatomic, weak) id<ParameterCellDelegate> delegate;

@end

@protocol ParameterCellDelegate <NSObject>

- (void)parameterCellValueDidChange:(ParameterCell*)cell;

@end
