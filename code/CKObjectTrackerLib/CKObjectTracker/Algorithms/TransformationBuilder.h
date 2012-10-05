//
//  TransformationBuilder.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 05.10.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_TransformationBuilder_h
#define CKObjectTrackerLib_TransformationBuilder_h

#include "ObjectTrackerTypesPublic.h"

namespace ck {

    class TransformationBuilder {
    private:
        static cv::Mat getRotation(const cv::Mat& homography);
        static cv::Point2f getTranslation(const std::vector<cv::Point2f>& objectCornersTransformed, const cv::Size2f& imageSize);
        static float getScale(const std::vector<cv::Point2f>& objectCornersTransformed, const std::vector<cv::Point2f>& objectCorners);
    public:
        static Transform getTransform(const cv::Mat& homography, const std::vector<cv::Point2f>& objectCorners, const cv::Size2f& imageSize);
    };
}

#endif
