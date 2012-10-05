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
        static bool validate(const cv::Mat& homography, const cv::Size& imageSize, const std::vector<cv::Point2f>& corners, std::vector<cv::Point2f>& cornersTransformed);
        static bool validate(const cv::Mat& homography, const cv::Size& imageSize, const std::vector<cv::Point2f>& corners, std::vector<cv::Point2f>& cornersTransformed, cv::Rect& boundingRect, bool cropBoundingRectToImageSize = false);
        
        static bool checkRectangle(const std::vector<cv::Point2f>& cornersTransformed, cv::Point2f& intersectionPoint);
        static bool checkBoundaries(const std::vector<cv::Point2f>& cornersTransformed, int width, int height);
        static bool checkAngleSimilarity(const std::vector<cv::Point2f>& cornersTransformed, float maxAngleOffsetInDeg);
    };
    
} // end of namepsace

#endif
