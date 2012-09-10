//
//  Neon.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 06.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_Neon_h
#define CKObjectTrackerLib_Neon_h

#if defined(__has_include)
    #if defined(__ARM_NEON__) && __has_include(<arm_neon.h>)
        #define __ARM_NEON_IS_AVAILABLE__
    #include <arm_neon.h>
    #endif
#endif

#if defined(__ARM_NEON_IS_AVAILABLE__)

// taken from http://computer-vision-talks.com/2011/02/a-very-fast-bgra-to-grayscale-conversion-on-iphone/
static void neon_bgra2gray(uint8_t * __restrict dest, uint8_t * __restrict src, int numPixels)
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

static void neon_bgr2gray(uint8_t * __restrict dest, uint8_t * __restrict src, int numPixels)
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
        uint8x8x3_t rgb  = vld3_u8 (src);
        uint8x8_t result;
        
        temp = vmull_u8 (rgb.val[0],      bfac);
        temp = vmlal_u8 (temp,rgb.val[1], gfac);
        temp = vmlal_u8 (temp,rgb.val[2], rfac);
        
        result = vshrn_n_u16 (temp, 8);
        vst1_u8 (dest, result);
        src  += 8*3;
        dest += 8;
    }
}

// taken from http://computer-vision-talks.com/2011/02/a-very-fast-bgra-to-grayscale-conversion-on-iphone/
static void neon_asm_bgra2gray(uint8_t * __restrict dest, uint8_t * __restrict src, int numPixels)
{
    __asm__ volatile("lsr          %2, %2, #3      \n"
                     "# build the three constants: \n"
                     "mov         r4, #29          \n" // Blue channel multiplier
                     "mov         r5, #150         \n" // Green channel multiplier
                     "mov         r6, #77          \n" // Red channel multiplier
                     "vdup.8      d4, r4           \n"
                     "vdup.8      d5, r5           \n"
                     "vdup.8      d6, r6           \n"
                     "0:						   \n" // start loop
                     "# load 8 pixels:             \n"
                     "vld4.8      {d0-d3}, [%1]!   \n" // four channels
                     "# do the weight average:     \n"
                     "vmull.u8    q7, d0, d4       \n"
                     "vmlal.u8    q7, d1, d5       \n"
                     "vmlal.u8    q7, d2, d6       \n"
                     "# shift and store:           \n"
                     "vshrn.u16   d7, q7, #8       \n" // Divide q3 by 256 and store in the d7
                     "vst1.8      {d7}, [%0]!      \n"
                     "subs        %2, %2, #1       \n" // Decrement iteration count
                     "bne         0b               \n" // Repeat unil iteration count is not zero
                     :
                     : "r"(dest), "r"(src), "r"(numPixels)
                     : "r4", "r5", "r6"
                     );
}

// don't use this! there is a bug
static void neon_asm_bgr2gray(uint8_t * __restrict dest, uint8_t * __restrict src, int numPixels)
{
    __asm__ volatile("lsr          %2, %2, #3      \n"
                     "# build the three constants: \n"
                     "mov         r4, #29          \n" // Blue channel multiplier
                     "mov         r5, #150         \n" // Green channel multiplier
                     "mov         r6, #77          \n" // Red channel multiplier
                     "vdup.8      d4, r4           \n"
                     "vdup.8      d5, r5           \n"
                     "vdup.8      d6, r6           \n"
                     "0:						   \n" // start loop
                     "# load 8 pixels:             \n"
                     "vld3.8      {d0-d2}, [%1]!   \n" // four channels
                     "# do the weight average:     \n"
                     "vmull.u8    q7, d0, d4       \n"
                     "vmlal.u8    q7, d1, d5       \n"
                     "vmlal.u8    q7, d2, d6       \n"
                     "# shift and store:           \n"
                     "vshrn.u16   d7, q7, #8       \n" // Divide q3 by 256 and store in the d7
                     "vst1.8      {d7}, [%0]!      \n"
                     "subs        %2, %2, #1       \n" // Decrement iteration count
                     "bne         0b               \n" // Repeat unil iteration count is not zero
                     :
                     : "r"(dest), "r"(src), "r"(numPixels)
                     : "r3", "r4", "r5"
                     );
}

#endif

#endif
