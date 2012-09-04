//
//  ObjectTrackerInitializer.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 03.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_ObjectTrackerInitializer_h
#define CKObjectTrackerLib_ObjectTrackerInitializer_h

#include "ObjectTrackerImpl.h"
#include "ValidationModule.h"
#include "Settings.h"

namespace ck {
    
    struct ObjectTracker::Initializer {
        static void initTracker(ObjectTracker::Implementation& tracker)
        {
            ValidationModule module = ValidationModule(std::vector<FilterFlag>(0));
            
            Settings settings = tracker._settings;
            settings.registerFloat("test", module, &ValidationModule::setRatio, &ValidationModule::getRatio, 0.0f, 1.0f);
            
        }
    };
    
} // end of namespace

#endif
