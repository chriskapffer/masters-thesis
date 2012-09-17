//
//  ObjectTrackerDebugger.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 24.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_ObjectTrackerDebugger_h
#define CKObjectTrackerLib_ObjectTrackerDebugger_h

#include "ObjectTrackerDebugInfo.h"

namespace ck {

class ObjectTrackerDebugger {
  
public:
    static std::string getDebugString(const TrackerDebugInfoStripped& info);
    // returns the average values for all debug infos (like a summary of video playback)
    static std::string getDebugString(const std::vector<TrackerDebugInfoStripped>& info);

    static cv::Mat getHistogramImage(const cv::MatND& hist);
    static bool getDetectionModuleDebugImage(cv::Mat& image, const TrackerDebugInfo info, bool drawSearchRect = true);
    static bool getValidationModuleDebugImage(cv::Mat& image, const TrackerDebugInfo info, bool drawObjectRect = true, bool drawObjectKeyPoints = false, bool drawSceneKeyPoints = false, bool drawFilteredMatches = true, bool drawAllMatches = false);
    static bool getTrackingModuleDebugImage(cv::Mat& image, const TrackerDebugInfo info, bool drawObjectRect = true, bool drawFilteredPoints = true, bool drawAllPoints = false, bool drawSearchRect = false);
    
private:
    static std::string getDetectionModuleDebugString(const TrackerDebugInfoStripped& info);
    static std::string getValidationModuleDebugString(const TrackerDebugInfoStripped& info);
    static std::string getTrackingModuleDebugString(const TrackerDebugInfoStripped& info);
    static std::string getProcessingTimersDebugString(std::vector<std::pair<std::string, double> > timers, double totalProcessingTime);
    static void decomposeDebugInfoVector(const std::vector<TrackerDebugInfoStripped>& info, TrackerDebugInfoStripped& sumDetectionInfos, TrackerDebugInfoStripped& sumValidationInfos, TrackerDebugInfoStripped& sumTrackingInfos, int& frameCountDetection, int& frameCountValidation, int& frameCountTracking, int& badHomographyCount);
    
    static void drawPoints(cv::Mat& image, const std::vector<cv::Point2f>& points, cv::Scalar color, cv::Point2f offset = cv::Point2f(), float scale = 1.0f);
    static void drawKeyPoints(cv::Mat& image, const std::vector<cv::KeyPoint>& keyPoints, cv::Scalar color, cv::Point2f offset = cv::Point2f(), float scale = 1.0f);
    static void drawMatches(cv::Mat& image, const std::vector<cv::DMatch>& matches, const std::vector<cv::KeyPoint>& keyPoints1, const std::vector<cv::KeyPoint>& keyPoints2, cv::Scalar color, cv::Point2f offset1 = cv::Point2f(), cv::Point2f offset2 = cv::Point2f(), float scale1 = 1.0f, float scale2 = 1.0f);
    static void drawRect(cv::Mat& image, const std::vector<cv::Point2f>& corners, cv::Scalar color, cv::Point2f offset = cv::Point2f(), float scale = 1.0f, int lineWidth = 2);
    static void scaleImageToSize(cv::Mat& image, cv::Size targetSize, float& scale, cv::Point2f& offset, bool centerImage = true);
};

} // end of namespace
    
#endif
