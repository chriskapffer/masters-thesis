//
//  DescriptorMatcherFilterer.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 27.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_DescriptorMatcherFilterer_h
#define CKObjectTrackerLib_DescriptorMatcherFilterer_h

typedef enum {
    FILTER_FLAG_CROP,
    FILTER_FLAG_RATIO,
    FILTER_FLAG_SYMMETRY,
} FilterFlag;

namespace ck {
    
    struct MatcherFilterer {
        static void getFilteredMatches(const cv::DescriptorMatcher& matcher, const cv::Mat& descriptors1, const cv::Mat& descriptors2, std::vector<cv::DMatch>& result, const std::vector<FilterFlag>& flags, bool sortMatches, float ratio, int nBestMatches, std::vector<std::pair<std::string, std::vector<cv::DMatch> > >& debugger);
        
        static void matchAndApplyFiltersAll(const cv::DescriptorMatcher& matcher, const cv::Mat& descriptors1, const cv::Mat& descriptors2, std::vector<cv::DMatch>& result, const std::vector<FilterFlag>& flags, bool sortMatches, float ratio, int nBestMatches, std::vector<std::pair<std::string, std::vector<cv::DMatch> > >& debugger);

        static void matchAndApplyFiltersNoRatio(const cv::DescriptorMatcher& matcher, const cv::Mat& descriptors1, const cv::Mat& descriptors2, std::vector<cv::DMatch>& result, const std::vector<FilterFlag>& flags, bool sortMatches, int nBestMatches, std::vector<std::pair<std::string, std::vector<cv::DMatch> > >& debugger);
        
        static void matchAndApplyFiltersNoSymmetry(const cv::DescriptorMatcher& matcher, const cv::Mat& descriptors1, const cv::Mat& descriptors2, std::vector<cv::DMatch>& result, const std::vector<FilterFlag>& flags, bool sortMatches, float ratio, int nBestMatches, std::vector<std::pair<std::string, std::vector<cv::DMatch> > >& debugger);

        static void matchAndApplyFiltersOnlyCrop(const cv::DescriptorMatcher& matcher, const cv::Mat& descriptors1, const cv::Mat& descriptors2, std::vector<cv::DMatch>& result, const std::vector<FilterFlag>& flags, bool sortMatches, int nBestMatches, std::vector<std::pair<std::string, std::vector<cv::DMatch> > >& debugger);
        
        static void filterMatchesWithMask(const std::vector<cv::DMatch>& matches, const std::vector<unsigned char>& mask, std::vector<cv::DMatch>& result, std::vector<std::pair<std::string, std::vector<cv::DMatch> > >& debugger);
    };
    
} // end of namespace

#endif
