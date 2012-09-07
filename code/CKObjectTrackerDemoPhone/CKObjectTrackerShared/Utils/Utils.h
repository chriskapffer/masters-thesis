//
//  Utils.h
//  CKObjectTrackerShared
//
//  Created by Christoph Kapffer on 07.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#import <sys/utsname.h>

static inline BOOL isRunningInSimulator()
{
#ifdef __i386__
    return YES;
#else
    return NO;
#endif
}

static inline NSString* deviceModelName()
{
    struct utsname systemInfo;
    uname(&systemInfo);
    NSString *modelName = [NSString stringWithCString:systemInfo.machine encoding:NSUTF8StringEncoding];
    
    if([modelName isEqualToString:@"i386"]) {
        modelName = @"simulator";
    }
    else if([modelName isEqualToString:@"iPhone1,1"]) {
        modelName = @"iPhone";
    }
    else if([modelName isEqualToString:@"iPhone1,2"]) {
        modelName = @"iPhone3G";
    }
    else if([modelName isEqualToString:@"iPhone2,1"]) {
        modelName = @"iPhone3GS";
    }
    else if([modelName isEqualToString:@"iPhone3,1"]) {
        modelName = @"iPhone4";
    }
    else if([modelName isEqualToString:@"iPhone4,1"]) {
        modelName = @"iPhone4S";
    }
    else if([modelName isEqualToString:@"iPod1,1"]) {
        modelName = @"iPod1stGen";
    }
    else if([modelName isEqualToString:@"iPod2,1"]) {
        modelName = @"iPod2ndGen";
    }
    else if([modelName isEqualToString:@"iPod3,1"]) {
        modelName = @"iPod3rdGen";
    }
    else if([modelName isEqualToString:@"iPad1,1"]) {
        modelName = @"iPad";
    }
    else if([modelName isEqualToString:@"iPad2,1"]) {
        modelName = @"iPad2(WiFi)";
    }
    else if([modelName isEqualToString:@"iPad2,2"]) {
        modelName = @"iPad2(GSM)";
    }
    else if([modelName isEqualToString:@"iPad2,3"]) {
        modelName = @"iPad2(CDMA)";
    }
    
    return modelName;
}
