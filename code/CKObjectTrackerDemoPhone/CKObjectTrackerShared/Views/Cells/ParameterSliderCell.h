//
//  ParameterSliderCell.h
//  CKObjectTrackerDemoPhone
//
//  Created by Christoph Kapffer on 09.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "ParameterCell.h"

#define PARAMETER_SLIDER_CELL_HEIGHT 72

@interface ParameterSliderCell : ParameterCell

@property (nonatomic, strong) IBOutlet UILabel* valueLabel;
@property (nonatomic, strong) IBOutlet UISlider* valueSlider;

@property (nonatomic, assign) float value;
@property (nonatomic, assign) float minValue;
@property (nonatomic, assign) float maxValue;
@property (nonatomic, assign) int stepSize;

@property (nonatomic, assign) BOOL isFloat;
@property (nonatomic, assign) BOOL isInt;

- (IBAction)sliderValueChanged:(id)sender;
- (IBAction)sliderStoppedSliding:(id)sender;

@end
