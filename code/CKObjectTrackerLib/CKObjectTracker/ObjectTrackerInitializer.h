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

namespace ck {
    
    struct ObjectTracker::Initializer {
        static void initTracker(ObjectTracker::Implementation& tracker)
        {
            Settings asd;
            tracker._settings = asd;
        }
    };
    
} // end of namespace

#endif
