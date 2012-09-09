//
//  ObjectTrackerParameter.m
//  CKObjectTrackerDemoPhone
//
//  Created by Christoph Kapffer on 08.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import "ObjectTrackerParameter.h"

@implementation ObjectTrackerParameter

@synthesize type;
@synthesize critical;
@synthesize name;

@synthesize boolValue;

@synthesize intValue;
@synthesize intValues;
@synthesize intMin;
@synthesize intMax;

@synthesize floatValue;
@synthesize floatMin;
@synthesize floatMax;

@synthesize stringValue;
@synthesize stringValues;

@end

@implementation ObjectTrackerParameterCollection

@synthesize name;
@synthesize parameters;
@synthesize subCollections;

@end