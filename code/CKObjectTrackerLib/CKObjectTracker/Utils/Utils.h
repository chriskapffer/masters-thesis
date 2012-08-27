//
//  Utils.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 22.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_Utils_h
#define CKObjectTrackerLib_Utils_h

namespace ck {

template <typename T>
inline static bool contains(const std::vector<T>& vec, T elem) {
    return find(vec.begin(), vec.end(), elem) != vec.end();
}
    
inline static bool compareKnnMatch (std::vector<cv::DMatch> i, std::vector<cv::DMatch> j)
{
    // DMatch has '<' operator defined
    return (i[0] < j[0]);
}
    
struct utils {
    
    static void get2DCoordinatesOfMatches(const std::vector<cv::DMatch>& matches, const std::vector<cv::KeyPoint>& keypoints1, const std::vector<cv::KeyPoint>& keypoints2, std::vector<cv::Point2f>& coordinates1, std::vector<cv::Point2f>& coordinates2);
    
    static void ratioTest(const std::vector<std::vector<cv::DMatch> >& matches, std::vector<cv::DMatch>& result, float ratio);
    
    static void ratioTest(const std::vector<std::vector<cv::DMatch> >& matches, std::vector<std::vector<cv::DMatch> >& result, float ratio);
    
    static void symmetryTest(const std::vector<cv::DMatch>& matches12, const std::vector<cv::DMatch>& matches21, std::vector<cv::DMatch>& result);
    
    static void symmetryTest(const std::vector<std::vector<cv::DMatch> >& matches12, const std::vector<std::vector<cv::DMatch> >& matches21, std::vector<std::vector<cv::DMatch> >& result);
    
    static void nBestMatches(const std::vector<cv::DMatch>& matches, std::vector<cv::DMatch>& result, int n, bool sorted);
    
    static void nBestMatches(const std::vector<std::vector<cv::DMatch> >& matches, std::vector<std::vector<cv::DMatch> >& result, int n, bool sorted);
    
    static std::vector<cv::DMatch> stripNeighbors(const std::vector<std::vector<cv::DMatch> >& matches);
};

} // end of namespace
    
#endif
