//
//  ParameterSwitchCell.h
//  CKObjectTrackerDemoPhone
//
//  Created by Christoph Kapffer on 09.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "ParameterCell.h"

#define PARAMETER_SWITCH_CELL_HEIGHT 44

@interface ParameterSwitchCell : ParameterCell

@property (nonatomic, strong) IBOutlet UISwitch* valueSwitch;

@property (nonatomic, assign) BOOL value;

- (IBAction)switchValueChanged:(id)sender;

@end
