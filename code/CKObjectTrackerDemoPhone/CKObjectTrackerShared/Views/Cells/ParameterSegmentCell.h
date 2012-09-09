//
//  ParameterSegmentCell.h
//  CKObjectTrackerDemoPhone
//
//  Created by Christoph Kapffer on 09.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "ParameterCell.h"

#define PARAMETER_SEGMENT_CELL_HEIGHT 79

@interface ParameterSegmentCell : ParameterCell

@property (nonatomic, strong) IBOutlet UISegmentedControl* valueSegControl;

@property (nonatomic, strong) NSString* value;
@property (atomic, strong) NSArray* values;

@property (nonatomic, assign) BOOL isString;
@property (nonatomic, assign) BOOL isInt;

- (IBAction)segControlValueChanged:(id)sender;

@end
