//
//  ColorConversion.cpp
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 06.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#include "ColorConversion.h"
#include "Neon.h"

using namespace cv;

namespace ck {
    
    void ColorConvert::bgrOrBgra2Gray(const Mat& imgIn, Mat& imgOut, ColorConversionMethod method)
    {
        if (imgIn.type() == CV_8UC1) {
            // do simple copy, because src is already gray
            imgIn.copyTo(imgOut);
            return;
        }
        
#if !defined(__ARM_NEON_IS_AVAILABLE__)
        method = COLOR_CONV_CV; // change method to default, if neon is not available
#endif
        if (method == COLOR_CONV_CV) {
            int code = imgIn.type() == CV_8UC3 ? CV_BGR2GRAY : CV_BGRA2GRAY;
            cvtColor(imgIn, imgOut, code);
            return;
        }
        
#if defined(__ARM_NEON_IS_AVAILABLE__)
        int imgSize = imgIn.rows * imgIn.cols;
        imgOut = Mat(imgIn.rows, imgIn.cols, CV_8UC1); // allocate memory before
        if (method == COLOR_CONV_NEON) {
            neon_bgr2gray(imgOut.data, imgIn.data, imgSize);
        } else if (method == COLOR_CONV_NEON_ASM) {
            neon_asm_bgr2gray(imgOut.data, imgIn.data, imgSize);
        }
#endif
    }
    
    void ColorConvert::bgrOrBgra2Hsv(const cv::Mat& imgIn, cv::Mat& imgOut, ColorConversionMethod method)
    {
//#if !defined(__ARM_NEON_IS_AVAILABLE__)
//        method = COLOR_CONV_CV; // change method to default, if neon is not available
//#endif
        method = COLOR_CONV_CV; // as long as neon conversion is not implemented, always fall back to default
        if (method == COLOR_CONV_CV) {
            if (imgIn.type() == CV_8UC4) {
                cvtColor(imgIn, imgOut, CV_BGRA2BGR);
                cvtColor(imgOut, imgOut, CV_BGR2HSV);
            } else {
                cvtColor(imgIn, imgOut, CV_BGR2HSV);
            }
            return;
        }
        
#if defined(__ARM_NEON_IS_AVAILABLE__)
        // TODO: add neon implementation
#endif
    }
    
} // end of namespace