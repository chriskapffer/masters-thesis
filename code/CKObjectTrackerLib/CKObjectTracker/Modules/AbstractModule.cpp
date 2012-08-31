//
//  AbstractModule.cpp
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 24.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#include "AbstractModule.h"
#include "ModuleManagement.h"
#include "Profiler.h"

#define TIMER_TOTAL "TotalProcessingTimeAbstract"

using namespace std;

namespace ck {

void AbstractModule::process(ModuleParams& params, TrackerDebugInfo& debugInfo)
{
    assert(params.successor == _moduleType);
    
    Profiler* profiler = Profiler::Instance();
    profiler->clearAll();
    profiler->startTimer(TIMER_TOTAL);

    debugInfo.subTaskProcessingTimes.clear();
    params.isObjectPresent = false;
    params.successor = ModuleTransition::getSuccessor(_moduleType, internalProcess(params, debugInfo));
    debugInfo.totalProcessingTime = profiler->elapsedTime(TIMER_TOTAL);
    debugInfo.currentModuleType = ModuleType2String::convert(_moduleType);
    debugInfo.sceneImageFull = params.sceneImageCurrent;
    
    profiler->clearTimer(TIMER_TOTAL);
    map<string, double> timerValues = profiler->getCurrentTimerValues();
    for (map<string, double>::const_iterator iter = timerValues.begin(); iter != timerValues.end(); iter++) {
        debugInfo.subTaskProcessingTimes.push_back(make_pair((*iter).first, (*iter).second));
    }
}

} // end of namespace
