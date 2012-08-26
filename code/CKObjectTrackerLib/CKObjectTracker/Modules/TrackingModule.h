//
//  TrackingModule.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 22.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_TrackingModule_h
#define CKObjectTrackerLib_TrackingModule_h

#include "AbstractModule.h"

namespace ck {

class TrackingModule : public AbstractModule {

private:
    
public:
    TrackingModule();
    ~TrackingModule();
    void initWithObjectImage(const cv::Mat& objectImage);
    bool internalProcess(ModuleInOutParams& params, ModuleDebugParams& debugInfo);
    
};

} // end of namespace
    
#endif
