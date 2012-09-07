//
//  ColorConversion.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 06.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_ColorConversion_h
#define CKObjectTrackerLib_ColorConversion_h

namespace ck {
    
    typedef enum {
        COLOR_CONV_CV,
        COLOR_CONV_NEON,
        COLOR_CONV_NEON_ASM,
    } ColorConversionMethod;
    
    struct ColorConvert {
        // source image can also be gray scale
        static void bgrOrBgra2Gray(const cv::Mat& imgIn, cv::Mat& imgOut, ColorConversionMethod method = COLOR_CONV_CV);
    };

} // end of namespace

#endif
