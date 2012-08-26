//
//  EmptyModule.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 25.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_EmptyModule_h
#define CKObjectTrackerLib_EmptyModule_h

#include "AbstractModule.h"

namespace ck {

class EmptyModule : public AbstractModule {
    
private:
    
public:
    EmptyModule();
    ~EmptyModule();
    void initWithObjectImage(const cv::Mat& objectImage);
    bool internalProcess(ModuleInOutParams& params, ModuleDebugParams& debugInfo);
    
};

} // end of namespace

#endif
