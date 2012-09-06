//
//  PointOperations.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 06.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_PointOperations_h
#define CKObjectTrackerLib_PointOperations_h

namespace ck {
    
    struct PointOps {
        static void move(std::vector<cv::Point2f>& points, const cv::Point2f& offset);
        
        static float averageDistance(const std::vector<cv::Point2f>& points1, const std::vector<cv::Point2f>& points2);

        static void coordinatesOfKeyPoints(const std::vector<cv::KeyPoint>& keypoints, std::vector<cv::Point2f>& coordinates, const cv::Point2f& offset = cv::Point2f());
        
        static void coordinatesOfMatches(const std::vector<cv::DMatch>& matches, const std::vector<cv::KeyPoint>& keypoints1, const std::vector<cv::KeyPoint>& keypoints2, std::vector<cv::Point2f>& coordinates1, std::vector<cv::Point2f>& coordinates2, const cv::Point2f& offset1 = cv::Point2f(), const cv::Point2f& offset2 = cv::Point2f());
    };

} // end of namespace

#endif
