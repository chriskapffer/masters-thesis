//
//  Utils.cpp
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 22.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#include "Utils.h"

#if defined(__has_include)
    #if defined(__ARM_NEON__) && __has_include(<arm_neon.h>)
        #define __NECESSARY_APPLE_FEATURES_AVAILABLE__
        #include <arm_neon.h>
    #endif
#endif

using namespace cv;
using namespace std;

namespace ck {
    
#pragma mark conversion
    
    static void bgr2GrayNeon(const Mat& imgIn, Mat& imgOut, bool assembler);
    
    void utils::bgr_a_2Gray(const Mat& imgIn, Mat& imgOut, int method)
    {
        // do simple copy, because src is already gray
        if (imgIn.type() == CV_8UC1) {
            imgIn.copyTo(imgOut);
            return;
        }
        
#if !defined(__NECESSARY_APPLE_FEATURES_AVAILABLE__)
        method = COLOR_CONV_CV;
#endif
        if (method == COLOR_CONV_CV) {
            int code = imgIn.type() == CV_8UC3 ? CV_BGR2GRAY : CV_BGRA2GRAY;
            cvtColor(imgIn, imgOut, code);
        } else {
            bgr2GrayNeon(imgIn, imgOut, method == COLOR_CONV_NEON_ASM);
        }
    }
    
    void utils::get2DCoordinatesOfKeyPoints(const vector<KeyPoint>& keypoints, std::vector<cv::Point2f>& coordinates)
    {
        if (!coordinates.empty()) { coordinates.clear(); }
        vector<KeyPoint>::const_iterator iter;
        for (iter = keypoints.begin(); iter != keypoints.end(); iter++) {
            coordinates.push_back((*iter).pt);
        }
    }
    
    void utils::get2DCoordinatesOfMatches(const vector<DMatch>& matches, const vector<KeyPoint>& keypoints1, const vector<KeyPoint>& keypoints2, vector<Point2f>& coordinates1, vector<Point2f>& coordinates2)
    {
        if (!coordinates1.empty()) { coordinates1.clear(); }
        if (!coordinates2.empty()) { coordinates2.clear(); }
        for (vector<DMatch>::const_iterator iter = matches.begin(); iter != matches.end(); iter++) {
            // get indices of keypoints from matches and add keypoint positions to coordinate lists
            coordinates1.push_back(keypoints1[(*iter).queryIdx].pt);
            coordinates2.push_back(keypoints2[(*iter).trainIdx].pt);
        }
    }
    
#pragma mark matching
    
    void utils::ratioTest(const vector<vector<DMatch> >& matches, vector<DMatch>& result, float ratio)
    {
        if (!result.empty()) { result.clear(); }
        vector<vector<DMatch> >::const_iterator iter = matches.begin();
        // for all matches
		for (; iter!= matches.end(); iter++) {
            // ignore matches with less than two neighbors
            if (iter->size() < 2)
                continue;
            // add match if first and second match are far apart from each other
            if ((*iter)[0].distance / (*iter)[1].distance < ratio)
                result.push_back((*iter)[0]);
		}
    }
    
    void utils::ratioTest(const vector<vector<DMatch> >& matches, vector<vector<DMatch> >& result, float ratio)
    {
        if (!result.empty()) { result.clear(); }
        vector<vector<DMatch> >::const_iterator iter = matches.begin();
        // for all matches
		for (; iter!= matches.end(); iter++) {
            // ignore matches with less than two neighbors
            if (iter->size() < 2)
                continue;
            // add match if first and second match are far apart from each other
            if ((*iter)[0].distance / (*iter)[1].distance < ratio)
                result.push_back(*iter);
		}
    }
    
    void utils::symmetryTest(const vector<DMatch>& matches12, const vector<DMatch>& matches21, vector<DMatch>& result)
    {
        if (!result.empty()) { result.clear(); }
        vector<DMatch>::const_iterator iter12 = matches12.begin();
        vector<DMatch>::const_iterator iter21 = matches21.begin();
        // for all matches image 1 -> image 2
        for (; iter12 != matches12.end(); iter12++) {
            // for all matches image 2 -> image 1
            for (; iter21 != matches21.end(); iter21++) {
                // test if match12 points to same descriptors as match21
                if ((*iter12).queryIdx == (*iter21).trainIdx
                    && (*iter21).queryIdx == (*iter12).trainIdx) {
                    // add match if symmetrical
                    result.push_back(DMatch((*iter12).queryIdx, (*iter12).trainIdx, (*iter12).distance));
                    break; // don't need to investigate matches image 2 -> image 1 any further
                }
            }
        }
    }
    
    void utils::symmetryTest(const vector<vector<DMatch> >& matches12, const vector<vector<DMatch> >& matches21, vector<vector<DMatch> >& result)
    {
        if (!result.empty()) { result.clear(); }
        vector<vector<DMatch> >::const_iterator iter12 = matches12.begin();
        vector<vector<DMatch> >::const_iterator iter21 = matches21.begin();
        // for all matches image 1 -> image 2
        for (; iter12 != matches12.end(); iter12++) {
            if (iter12->size() < 2)
                continue; // ignore matches with less than two neighbors
            
            // for all matches image 2 -> image 1
            for (; iter21 != matches21.end(); iter21++) {
                if (iter21->size() < 2)
                    continue; // ignore matches with less than two neighbors
                
                // test if match12 points to same descriptors as match21
                if ((*iter12)[0].queryIdx == (*iter21)[0].trainIdx
                    && (*iter21)[0].queryIdx == (*iter12)[0].trainIdx) {
                    // add match if symmetrical
                    vector<DMatch> symMatch;
                    symMatch.push_back(DMatch((*iter12)[0].queryIdx, (*iter12)[0].trainIdx, (*iter12)[0].distance));
                    symMatch.push_back(DMatch((*iter12)[1].queryIdx, (*iter12)[1].trainIdx, (*iter12)[1].distance));
                    result.push_back(symMatch);
                    break; // don't need to investigate matches image 2 -> image 1 any further
                }
            }
        }
    }
    
    void utils::nBestMatches(const vector<DMatch>& matches, vector<DMatch>& result, int n, bool sorted)
    {
        n = MIN((int)matches.size(), n);
        
        if (sorted) {
            // copy the first n matches into result vector
            result.assign(matches.begin(), matches.begin() + n);
        } else {
            // copy all matches
            result = matches;
            // make sure that the n best elements are at the first n positions (not ordered themself)
            nth_element(result.begin(), result.begin() + n - 1, result.end());
            // remove all others
            result.erase(result.begin() + n, result.end());
        }
    }
    
    void utils::nBestMatches(const vector<vector<DMatch> >& matches, vector<vector<DMatch> >& result, int n, bool sorted)
    {
        n = MIN((int)matches.size(), n);
        
        if (sorted) {
            // copy the first n matches into result vector
            result.assign(matches.begin(), matches.begin() + n);
        } else {
            // copy all matches
            result = matches;
            // make sure that the n best elements are at the first n positions (not ordered themself)
            nth_element(result.begin(), result.begin() + n - 1, result.end(), compareKnnMatch);
            // remove all others
            result.erase(result.begin() + n, result.end());
        }
    }
    
    vector<DMatch> utils::stripNeighbors(const vector<vector<DMatch> >& matches)
    {
        vector<DMatch> result;
        // for all matches - add first neighbor to result and ignore others
		for (vector<vector<DMatch> >::const_iterator iter = matches.begin(); iter!= matches.end(); iter++) {
            result.push_back((*iter)[0]);
		}
        return result;
    }
    
#pragma mark - neon
    
#if !defined(__NECESSARY_APPLE_FEATURES_AVAILABLE__)
    
    static void bgr2GrayNeon(const Mat& imgIn, Mat& imgOut, bool assembler)
    {
        throw "NEON is not available, don't use this function!";
    }
    
#else
    
    // taken from http://computer-vision-talks.com/2011/02/a-very-fast-bgra-to-grayscale-conversion-on-iphone/
    static void neon_convert (uint8_t * __restrict dest, uint8_t * __restrict src, int numPixels)
    {
        int i;
        // Y = 0.299 R + 0.587 G + 0.114 B http://en.wikipedia.org/wiki/YCbCr
        uint8x8_t rfac = vdup_n_u8 (77);
        uint8x8_t gfac = vdup_n_u8 (150);
        uint8x8_t bfac = vdup_n_u8 (29);
        int n = numPixels / 8;
        
        // Convert per eight pixels
        for (i=0; i < n; ++i)
        {
            uint16x8_t  temp;
            uint8x8x4_t rgb  = vld4_u8 (src);
            uint8x8_t result;
            
            temp = vmull_u8 (rgb.val[0],      bfac);
            temp = vmlal_u8 (temp,rgb.val[1], gfac);
            temp = vmlal_u8 (temp,rgb.val[2], rfac);
            
            result = vshrn_n_u16 (temp, 8);
            vst1_u8 (dest, result);
            src  += 8*4;
            dest += 8;
        }
    }
    
    static void neon_asm_convert(uint8_t * __restrict dest, uint8_t * __restrict src, int numPixels)
    {
        __asm__ volatile("lsr          %2, %2, #3      \n"
                         "# build the three constants: \n"
                         "mov         r4, #29          \n" // Blue channel multiplier
                         "mov         r5, #150         \n" // Green channel multiplier
                         "mov         r6, #77          \n" // Red channel multiplier
                         "vdup.8      d4, r4           \n"
                         "vdup.8      d5, r5           \n"
                         "vdup.8      d6, r6           \n"
                         "0:						   \n"
                         "# load 8 pixels:             \n"
                         "vld4.8      {d0-d3}, [%1]!   \n"
                         "# do the weight average:     \n"
                         "vmull.u8    q7, d0, d4       \n"
                         "vmlal.u8    q7, d1, d5       \n"
                         "vmlal.u8    q7, d2, d6       \n"
                         "# shift and store:           \n"
                         "vshrn.u16   d7, q7, #8       \n" // Divide q3 by 256 and store in the d7
                         "vst1.8      {d7}, [%0]!      \n"
                         "subs        %2, %2, #1       \n" // Decrement iteration count
                         "bne         0b            \n" // Repeat unil iteration count is not zero
                         :
                         : "r"(dest), "r"(src), "r"(numPixels)
                         : "r4", "r5", "r6"
                         );
    }

    static void bgr2GrayNeon(const Mat& imgIn, Mat& imgOut, bool assembler)
    {
        imgOut = Mat(imgIn.rows, imgIn.cols, CV_8UC1);
        if (assembler) {
            neon_convert(imgOut.data, imgIn.data, size);
        } else {
            neon_asm_convert(imgOut.data, imgIn.data, size);
        }
    }
    
#endif
    
} // end of namespace
