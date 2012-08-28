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
    
enum ModuleType {
    MODULE_TYPE_DETECTION,
    MODULE_TYPE_VALIDATION,
    MODULE_TYPE_TRACKING,
    MODULE_TYPE_EMPTY,
};
    
class ModuleType2String {
    
private:
    struct TableProvider {
        inline static std::map<ModuleType, const char*> table() {
            std::map<ModuleType, const char*> t;
            t[MODULE_TYPE_DETECTION] = "DetectionModule";
            t[MODULE_TYPE_VALIDATION] = "ValidationModule";
            t[MODULE_TYPE_TRACKING] = "TrackingModule";
            t[MODULE_TYPE_EMPTY] = "EmptyModule";
            return t;
        }
    };

public:
    inline static std::string convert(ModuleType type) {
        static std::map<ModuleType, const char*> stringTable = TableProvider::table();
        return (*stringTable.find(type)).second;
    }
};
    
} // end of namespace

#endif
