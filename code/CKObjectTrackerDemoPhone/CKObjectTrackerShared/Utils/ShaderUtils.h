//
//  ShaderUtils.h
//  CKObjectTrackerDemoPhone
//
//  Created by Christoph Kapffer on 13.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface ShaderUtils : NSObject

+ (BOOL)createProgram:(GLuint*)program WithVertexShaderPath:(NSString*)vertexShaderPath FragmentShaderPath:(NSString*)fragmentShaderPath Attributes:(NSDictionary*)attributes;

@end
