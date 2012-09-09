//
//  ParameterSwitchCell.m
//  CKObjectTrackerDemoPhone
//
//  Created by Christoph Kapffer on 09.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import "ParameterSwitchCell.h"

@implementation ParameterSwitchCell

@synthesize valueSwitch = _valueSwitch;

@synthesize value = _value;

- (void)setValue:(BOOL)value
{
    if (value == _value) { return; }
    _value = value;
    
    [self.valueSwitch setOn:self.value];
}

- (id)initWithCoder:(NSCoder *)coder
{
    self = [super initWithCoder:coder];
    if (self) {
        _value = false;
    }
    return self;
}

- (IBAction)switchValueChanged:(id)sender
{
    self.value = self.valueSwitch.on;
    if ([self.delegate respondsToSelector:@selector(parameterCellValueDidChange:)]) {
        [self.delegate parameterCellValueDidChange:self];
    }
}

@end
