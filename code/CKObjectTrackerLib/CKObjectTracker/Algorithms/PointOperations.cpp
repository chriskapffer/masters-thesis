//
//  PointOperations.cpp
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 06.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#include "PointOperations.h"

using namespace std;
using namespace cv;

namespace ck {
    
    
    void PointOps::move(vector<Point2f>& points, const Point2f& offset)
    {
        vector<Point2f>::iterator iter;
        for (iter = points.begin(); iter != points.end(); iter++) {
            (*iter) += offset;
        }
    }
    
    float PointOps::averageDistance(const vector<Point2f>& points1, const vector<Point2f>& points2)
    {
        size_t count1 = points1.size();
        size_t count2 = points2.size();
        if (count1 == 0 || count2 == 0)
            return 0;
        
        float avgDistance = 0;
        assert(count1 == count2);
        for (int i = 0; i < count1; i++) {
            Point2f vec = (points2[i] - points1[i]);
            avgDistance += vec.x * vec.x + vec.y * vec.y;
        }
        avgDistance /= count1;
        return sqrt(avgDistance);
    }
    
    void PointOps::coordinatesOfKeyPoints(const vector<KeyPoint>& keypoints, std::vector<cv::Point2f>& coordinates, const Point2f& offset)
    {
        if (!coordinates.empty()) { coordinates.clear(); }
        vector<KeyPoint>::const_iterator iter;
        for (iter = keypoints.begin(); iter != keypoints.end(); iter++) {
            coordinates.push_back((*iter).pt + offset);
        }
    }
    
    void PointOps::coordinatesOfMatches(const vector<DMatch>& matches, const vector<KeyPoint>& keypoints1, const vector<KeyPoint>& keypoints2, vector<Point2f>& coordinates1, vector<Point2f>& coordinates2, const Point2f& offset1, const Point2f& offset2)
    {
        if (!coordinates1.empty()) { coordinates1.clear(); }
        if (!coordinates2.empty()) { coordinates2.clear(); }
        for (vector<DMatch>::const_iterator iter = matches.begin(); iter != matches.end(); iter++) {
            // get indices of keypoints from matches and add keypoint positions to coordinate lists
            coordinates1.push_back(keypoints1[(*iter).queryIdx].pt + offset1);
            coordinates2.push_back(keypoints2[(*iter).trainIdx].pt + offset2);
        }
    }
    
} // end of namespace