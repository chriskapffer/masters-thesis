//
//  FilterAlgorithm.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 06.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_FilterAlgorithm_h
#define CKObjectTrackerLib_FilterAlgorithm_h

namespace ck {

    inline static bool compareKnnMatch(std::vector<cv::DMatch> i, std::vector<cv::DMatch> j)
    {
        // DMatch has '<' operator defined
        return (i[0] < j[0]);
    }
    
    struct FilterAlgorithm {
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
