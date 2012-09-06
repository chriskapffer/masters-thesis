//
//  ColorConversion.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 06.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_ColorConversion_h
#define CKObjectTrackerLib_ColorConversion_h

#define COLOR_CONV_CV       0
#define COLOR_CONV_NEON     1
#define COLOR_CONV_NEON_ASM 2

#define COLOR_CONV_METHOD COLOR_CONV_CV

namespace ck {
    
    struct ColorConvert {
        // source image can also be gray scale
        static void bgrOrBgra2Gray(const cv::Mat& imgIn, cv::Mat& imgOut, int method = COLOR_CONV_METHOD);
    };

} // end of namespace

#endif
