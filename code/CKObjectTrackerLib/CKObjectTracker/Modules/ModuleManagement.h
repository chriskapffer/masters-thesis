//
//  ModuleManagement.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 25.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_ModuleManagement_h
#define CKObjectTrackerLib_ModuleManagement_h

#include "ModuleTypes.h"
#include "AbstractModule.h"
#include "DetectionModule.h"
#include "validationModule.h"
#include "TrackingModule.h"
#include "EmptyModule.h"

namespace ck {

struct ModuleCollection {
    inline static std::map<ModuleType, AbstractModule*> create() {
        std::map<ModuleType, AbstractModule*> modules;
        modules[MODULE_TYPE_DETECTION] = new DetectionModule();
        modules[MODULE_TYPE_VALIDATION] = new ValidationModule();
        modules[MODULE_TYPE_TRACKING] = new TrackingModule(500);
        modules[MODULE_TYPE_EMPTY] = new EmptyModule();
        return modules;
    }
};

class ModuleTransition {
    
private:
    struct TableProvider {
        inline static std::map<ModuleType, std::pair<ModuleType, ModuleType> > table() {
            std::map<ModuleType, std::pair<ModuleType, ModuleType> > t;
            t[MODULE_TYPE_DETECTION] = std::make_pair(MODULE_TYPE_VALIDATION, MODULE_TYPE_DETECTION);
            t[MODULE_TYPE_VALIDATION] = std::make_pair(MODULE_TYPE_TRACKING, MODULE_TYPE_DETECTION);
            t[MODULE_TYPE_TRACKING] = std::make_pair(MODULE_TYPE_TRACKING, MODULE_TYPE_VALIDATION);
            t[MODULE_TYPE_EMPTY] = std::make_pair(MODULE_TYPE_EMPTY, MODULE_TYPE_EMPTY);
            return t;
        }
    };
    
public:
    inline static ModuleType getSuccessor(ModuleType moduleType, bool didSucceed) {
        static std::map<ModuleType, std::pair<ModuleType, ModuleType> > transitionTable = TableProvider::table();
        return didSucceed ? transitionTable[moduleType].first : transitionTable[moduleType].second;
    }
};
    
} // end of namespace

#endif




