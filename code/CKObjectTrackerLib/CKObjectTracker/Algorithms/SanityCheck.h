//
//  SanityCheck.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 27.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_SanityCheck_h
#define CKObjectTrackerLib_SanityCheck_h

namespace ck {
    
    struct SanityCheck {
        // points must be arranged in a clock wise manner
        static bool checkRectangle(const std::vector<cv::Point2f>& corners);
    };
    
} // end of namepsace

#endif
