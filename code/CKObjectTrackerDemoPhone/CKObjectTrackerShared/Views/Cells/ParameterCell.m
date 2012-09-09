//
//  ParameterCell.m
//  CKObjectTrackerDemoPhone
//
//  Created by Christoph Kapffer on 09.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import "ParameterCell.h"

@implementation ParameterCell

@synthesize nameLabel = _nameLabel;
@synthesize name = _name;
@synthesize critical = _critical;

@synthesize delegate = _delegate;

- (void)setName:(NSString *)name
{
    _name = name;
    self.nameLabel.text = _name;
}

@end
