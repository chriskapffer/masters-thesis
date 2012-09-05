//
//  ObjectTrackerInitializer.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 03.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_ObjectTrackerInitializer_h
#define CKObjectTrackerLib_ObjectTrackerInitializer_h

#include "ObjectTrackerImplementation.h"
#include "SettingsProjectScope.h"
#include "ValidationModule.h"

namespace ck {
    
    struct ObjectTracker::Initializer {
        static void initTracker(ObjectTracker::Implementation& tracker)
        {
            ValidationModule* module = dynamic_cast<ValidationModule*>(tracker._allModules[MODULE_TYPE_VALIDATION]);
            Settings validationSettings = Settings("Validation Settings");
            validationSettings.registerFloat("Ratio", module, &ValidationModule::setRatio, &ValidationModule::getRatio, 0.0f, 1.0f);
            
            tracker._settings.addCategory(validationSettings);
        }
    };
    
} // end of namespace

#endif
