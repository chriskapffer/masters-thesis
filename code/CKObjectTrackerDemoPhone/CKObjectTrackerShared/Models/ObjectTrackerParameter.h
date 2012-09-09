//
//  ObjectTrackerParameter.h
//  CKObjectTrackerDemoPhone
//
//  Created by Christoph Kapffer on 08.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef enum {
    ObjectTrackerParameterTypeBool,
    ObjectTrackerParameterTypeInt,
    ObjectTrackerParameterTypeFloat,
    ObjectTrackerParameterTypeString,
} ObjectTrackerParameterType;

@interface ObjectTrackerParameter : NSObject

@property (nonatomic, assign) ObjectTrackerParameterType type;
@property (nonatomic, assign, getter = isCritical) BOOL critical;
@property (nonatomic, strong) NSString* name;

@property (nonatomic, assign) BOOL boolValue;

@property (nonatomic, assign) int intValue;
@property (nonatomic, strong) NSArray* intValues;
@property (nonatomic, assign) int intMin;
@property (nonatomic, assign) int intMax;

@property (nonatomic, assign) float floatValue;
@property (nonatomic, assign) float floatMin;
@property (nonatomic, assign) float floatMax;

@property (nonatomic, strong) NSString* stringValue;
@property (nonatomic, strong) NSArray* stringValues;

@end

@interface ObjectTrackerParameterCollection : NSObject

@property (nonatomic, strong) NSString* name;
@property (nonatomic, strong) NSArray* parameters;
@property (nonatomic, strong) NSArray* subCollections;

@end