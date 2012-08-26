//
//  ModuleTypes.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 25.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_ModuleTypes_h
#define CKObjectTrackerLib_ModuleTypes_h

#include "MapCreator.h"

namespace ck {
    
    enum ModuleType {
        MODULE_TYPE_DETECTION,
        MODULE_TYPE_VALIDATION,
        MODULE_TYPE_TRACKING,
        MODULE_TYPE_EMPTY,
    };
    
    static const std::map<ModuleType, const char*> moduleTypeString = createMap<ModuleType, const char*>
        (MODULE_TYPE_DETECTION, "DetectionModule")
        (MODULE_TYPE_VALIDATION, "ValidationModule")
        (MODULE_TYPE_TRACKING, "TrackingModule")
        (MODULE_TYPE_EMPTY, "EmptyModule")
    ;
    
} // end of namespace

#endif
