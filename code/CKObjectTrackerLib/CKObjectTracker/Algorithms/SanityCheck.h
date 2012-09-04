//
//  SanityCheck.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 27.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_SanityCheck_h
#define CKObjectTrackerLib_SanityCheck_h

#define MAX_ANGLE_DEG 110.0f
#define MIN_ANGLE_DEG 70.0f

namespace ck {
    
    struct SanityCheck {
        // points must be arranged in a clock wise manner
        static bool checkRectangle(const std::vector<cv::Point2f>& cornersTransformed);
        static bool checkBoundaries(const std::vector<cv::Point2f>& cornersTransformed, int width, int height);
        static bool checkMaxMinAngles(const std::vector<cv::Point2f>& cornersTransformed, float maxAngleDeg, float minAngleDeg);
        static bool validate(const cv::Mat& homography, const cv::Size& imageSize, const std::vector<cv::Point2f>& corners, std::vector<cv::Point2f>& cornersTransformed);
        static bool validate(const cv::Mat& homography, const cv::Size& imageSize, const std::vector<cv::Point2f>& corners, std::vector<cv::Point2f>& cornersTransformed, cv::Rect& boundingRect, bool cropBoundingRectToImageSize = false);
    };
    
} // end of namepsace

#endif
