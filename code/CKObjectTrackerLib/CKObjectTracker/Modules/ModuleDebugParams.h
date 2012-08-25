//
//  ModuleDebugParams.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 25.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_ModuleDebugParams_h
#define CKObjectTrackerLib_ModuleDebugParams_h

#include "ModuleTypes.h"

struct ModuleDebugParams {
    double totalProcessingTime;
    std::vector<double> individualProcessingTimes;
    ModuleType currentModuleType;
};

#endif
