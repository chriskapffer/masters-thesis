//
//  AbstractModule.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 24.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_AbstractModule_h
#define CKObjectTrackerLib_AbstractModule_h

#include "ModuleTypes.h"
#include "ModuleParams.h"
#include "ObjectTrackerTypeDefinitions.h"

namespace ck {
    
class AbstractModule {
    
public:
    virtual ~AbstractModule() { };
    virtual void initWithObjectImage(const cv::Mat& objectImage) = 0;
    void process(ModuleParams& params, TrackerDebugInfo& debugInfo);
    inline ModuleType getType() const { return _moduleType; }
    
protected:
    AbstractModule(ModuleType moduleType) : _moduleType(moduleType) { };
    virtual bool internalProcess(ModuleParams& params, TrackerDebugInfo& debugInfo) = 0;
    
private:
    ModuleType _moduleType;
};

} // end of namespace
    
#endif
