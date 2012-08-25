//
//  Utils.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 22.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_Utils_h
#define CKObjectTrackerLib_Utils_h

#define BGRA2GRAY_CV       0
#define BGRA2GRAY_NEON     1
#define BGRA2GRAY_NEON_ASM 2

class Utils
{
    
public:
    
    static std::vector<cv::MatND> calcHistograms(const cv::Mat& image, int numOfBins);
    static cv::Mat imageFromHistograms(const std::vector<cv::MatND>& histgrams, int width, int height, int bins);
    
    static void matchSimple(const cv::Ptr<cv::DescriptorMatcher>& matcher,
                            const cv::Mat& descriptors1,
                            const cv::Mat& descriptors2,
                            std::vector<cv::DMatch>& matches,
                            bool crossCheck = false);
    
    static void matchAdvanced(const cv::Ptr<cv::DescriptorMatcher>& matcher,
                              const cv::Mat& descriptors1,
                              const cv::Mat& descriptors2,
                              std::vector<cv::DMatch>& matches,
                              int knn = 2);
    
    static void get2DCoordinatesOfMatches(const std::vector<cv::DMatch>& matches,
                                          const std::vector<cv::KeyPoint>& keypoints1,
                                          const std::vector<cv::KeyPoint>& keypoints2,
                                          std::vector<cv::Point2f>& coordinates1,
                                          std::vector<cv::Point2f>& coordinates2);
    
    static void calcHomography(const std::vector<cv::KeyPoint>& keypoints1,
                               const std::vector<cv::KeyPoint>& keypoints2,
                               const std::vector<cv::DMatch>& matches,
                               cv::Mat& H, int method, double ransacThreshold);
    
    static void calcHomography(const std::vector<cv::Point2f>& coordinates1,
                               const std::vector<cv::Point2f>& coordinates2,
                               cv::Mat& H, int method, double ransacThreshold);
    
    static std::vector<char> createMaskWithOriginalPointSet(const std::vector<cv::Point2f>& coordinates1,
                                                            const std::vector<cv::Point2f>& coordinates2,
                                                            const cv::Mat& H, double ransacThreshold);
    
    static std::vector<char> createMaskWithTransformedPointSet(const std::vector<cv::Point2f>& coordinates1,
                                                               const std::vector<cv::Point2f>& coordinates2,
                                                               double ransacThreshold);
    
    static std::vector<char> invertedMask(const std::vector<char>& original);
    
    static std::vector<cv::DMatch> filteredMatchesWithMask(const std::vector<cv::DMatch>& matches,
                                                           const std::vector<char>& mask);
    
    static std::vector<cv::DMatch> nBestMatches(const std::vector<cv::DMatch>& matches,
                                                int numberOfMatchesToKeep);
    
    static int positiveMatches(const std::vector<char>& mask);
    
    static void maxMinDistance(const std::vector<cv::DMatch>& matches, float& maxDistance, float& minDistance);
    
    static double distanceBetween(const cv::Point2f& v1,
                                  const cv::Point2f& v2);
    
    static double cumulatedDistance(const std::vector<cv::Point2f>& c1,
                                    const std::vector<cv::Point2f>& c2);
    
    static static cv::Mat bgra2Gray(cv::Mat imgIn, int method);
};

#endif
