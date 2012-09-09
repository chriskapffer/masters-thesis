//
//  ParameterSegmentCell.m
//  CKObjectTrackerDemoPhone
//
//  Created by Christoph Kapffer on 09.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import "ParameterSegmentCell.h"

@implementation ParameterSegmentCell

@synthesize valueSegControl = _valueSegControl;

@synthesize value = _value;
@synthesize values = _values;

@synthesize isString = _isString;
@synthesize isInt = _isInt;

- (void)setValue:(NSString *)value
{
    _value = value;
    self.valueSegControl.selectedSegmentIndex = [self.values indexOfObject:_value];
}

- (void)setValues:(NSArray *)values
{
    _values = values;
    [self.valueSegControl removeAllSegments];
    for (int i = 0; i < _values.count; i++) {
        [self.valueSegControl insertSegmentWithTitle:[_values objectAtIndex:i] atIndex:i animated:NO];
    }
    self.valueSegControl.selectedSegmentIndex = [self.values indexOfObject:_value];
}

- (void)setIsString:(BOOL)isString
{
    if (isString == _isString) { return; }
    _isString = isString;
    
    self.isInt = !_isString;
}

- (void)setIsInt:(BOOL)isInt
{
    if (isInt == _isInt) { return; }
    _isInt = isInt;
    
    self.isString = !_isInt;
}

- (id)initWithCoder:(NSCoder *)coder
{
    self = [super initWithCoder:coder];
    if (self) {
        _values = [NSArray array];
        _isString = YES;
        _isInt = NO;
    }
    return self;
}

- (IBAction)segControlValueChanged:(id)sender
{
    self.value = [self.values objectAtIndex:self.valueSegControl.selectedSegmentIndex];
    
    if ([self.delegate respondsToSelector:@selector(parameterCellValueDidChange:)]) {
        [self.delegate parameterCellValueDidChange:self];
    }
}

@end
