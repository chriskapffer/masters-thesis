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
            ValidationModule module = ValidationModule(std::vector<FilterFlag>(0));
            
            Settings validationSettings("Validation Settings");
            //validationSettings.set
            
            // TODO: make this work
            //validationSettings.registerFloat("test", module, &ValidationModule::setRatio, &ValidationModule::getRatio, 0.0f, 1.0f);
            
            Settings* objectTrackerSettings = new Settings("Object Tracker Settings");
            objectTrackerSettings->addCategory(validationSettings);
            tracker._settings = objectTrackerSettings;
        }
    };
    
} // end of namespace

#endif
