//
//  AbstractModule.cpp
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 24.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#include "AbstractModule.h"
#include "ModuleManagement.h"

namespace ck {

void AbstractModule::process(ModuleInOutParams& params, ModuleDebugParams& debugInfo)
{
    assert(params.successor == _moduleType);
    double startTime = (double)cv::getTickCount();
    params.isObjectPresent = false;
    params.successor = ModuleTransition::getSuccessor(_moduleType, internalProcess(params, debugInfo));
    debugInfo.totalProcessingTime = ((double)cv::getTickCount() - startTime) / cv::getTickFrequency() * 1000;
    debugInfo.currentModuleType = (*moduleTypeString.find(_moduleType)).second;
}

} // end of namespace
