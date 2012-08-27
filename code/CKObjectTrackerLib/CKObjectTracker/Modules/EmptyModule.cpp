//
//  EmptyModule.cpp
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 25.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#include "EmptyModule.h"

namespace ck {

EmptyModule::EmptyModule() : AbstractModule(MODULE_TYPE_EMPTY)
{
    
}

EmptyModule::~EmptyModule()
{

}

void EmptyModule::initWithObjectImage(const cv::Mat &objectImage)
{
    
}

bool EmptyModule::internalProcess(ModuleParams& params, TrackerDebugInfo& debugInfo)
{
    return false;
}

} // end of namespace
