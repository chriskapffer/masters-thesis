//
//  ParameterSliderCell.m
//  CKObjectTrackerDemoPhone
//
//  Created by Christoph Kapffer on 09.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import "ParameterSliderCell.h"

@implementation ParameterSliderCell

@synthesize valueLabel = _valueLabel;
@synthesize valueSlider = _valueSlider;

@synthesize value = _value;
@synthesize minValue = _minValue;
@synthesize maxValue = _maxValue;
@synthesize stepSize = _stepSize;

@synthesize isFloat = _isFloat;
@synthesize isInt = _isInt;

- (void)setValue:(float)value
{
    if (value == _value) { return; }
    _value = value;
    
    self.valueSlider.value = _value;
}

- (void)setMinValue:(float)minValue
{
    if (minValue == _minValue) { return; }
    _minValue = minValue;
    
    if (_value < _minValue) { _value = _minValue; }
    self.valueSlider.minimumValue = _minValue;
}

- (void)setMaxValue:(float)maxValue
{
    if (maxValue == _maxValue) { return; }
    _maxValue = maxValue;
    
    if (_value > _maxValue) { _value = _maxValue; }
    self.valueSlider.maximumValue = _maxValue;
}

- (void)setIsFloat:(BOOL)isFloat
{
    if (isFloat == _isFloat) { return; }
    _isFloat = isFloat;

    if (_isFloat) { self.valueLabel.text = [NSString stringWithFormat:@"%.4f", _value]; }
    self.isInt = !_isFloat;
}

- (void)setIsInt:(BOOL)isInt
{
    if (isInt == _isInt) { return; }
    _isInt = isInt;
    
    if (_isInt) { self.valueLabel.text = [NSString stringWithFormat:@"%d", (int)_value]; }
    self.isFloat = !_isInt;
}

- (id)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    if (self) {
        _maxValue = 100;
        _minValue = 0;
        _stepSize = 10;
        _value = 50;
        _isFloat = NO;
        _isInt = NO;
    }
    return self;
}

- (IBAction)sliderValueChanged:(id)sender
{
    self.value = self.valueSlider.value;
    if (_isFloat) { self.valueLabel.text = [NSString stringWithFormat:@"%.4f", _value]; }
    if (_isInt) { self.valueLabel.text = [NSString stringWithFormat:@"%d", (int)_value]; }
    if ([self.delegate respondsToSelector:@selector(parameterCellValueDidChange:)]) {
        [self.delegate parameterCellValueDidChange:self];
    }
}

@end
