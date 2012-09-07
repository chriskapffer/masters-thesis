//
//  ModuleTypes.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 25.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_ModuleTypes_h
#define CKObjectTrackerLib_ModuleTypes_h

namespace ck {
    
    typedef enum {
        MODULE_TYPE_DETECTION,
        MODULE_TYPE_VALIDATION,
        MODULE_TYPE_TRACKING,
        MODULE_TYPE_EMPTY,
    } ModuleType;
    
} // end of namespace

#endif
