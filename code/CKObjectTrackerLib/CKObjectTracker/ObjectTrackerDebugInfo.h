//
//  ObjectTrackerDebugInfo.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 06.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_ObjectTrackerDebugInfo_h
#define CKObjectTrackerLib_ObjectTrackerDebugInfo_h

namespace ck {

struct TrackerDebugInfo {
    // general
    double totalProcessingTime;
    std::vector<std::pair<std::string, double> > subTaskProcessingTimes;
    std::string currentModuleType;
    
    // detection
    cv::Mat probabilityMap;
    cv::Rect searchRect;
    bool searchRectValid;
    
    // validation
    std::vector<std::pair<std::string, std::vector<cv::DMatch> > > namedMatches;
    std::vector<cv::KeyPoint> objectKeyPoints;
    std::vector<cv::KeyPoint> sceneKeyPoints;
    
    cv::Mat objectImage;
    cv::Mat sceneImageFull;
    cv::Mat sceneImagePart;
    cv::Point2f imageOffset;
        
    // tracking
    std::vector<std::pair<std::string, std::vector<cv::Point2f> > > namedPointSets;
    int initialPointCount;
    float transformationDelta;
    float distortion;
    float avgError;
    
    // validation and tracking
    std::vector<cv::Point2f> objectCornersTransformed;
    cv::Mat homography;
    bool badHomography;
    
    TrackerDebugInfo() : totalProcessingTime(0), initialPointCount(0), transformationDelta(0), distortion(0), avgError(0), badHomography(false) {}
};

struct TrackerDebugInfoStripped
{
    // general
    double totalProcessingTime;
    std::vector<std::pair<std::string, double> > subTaskProcessingTimes;
    std::string currentModuleType;
    
    // validation
    std::vector<std::pair<std::string, float> > namedMatchCounts;
    int objectKeyPointCount;
    int sceneKeyPointCount;

    // tracking
    std::vector<std::pair<std::string, float> > namedPointCounts;
    int initialPointCount;
    float transformationDelta;
    float distortion;
    float avgError;

    // validation and tracking
    bool badHomography;

    // constructors
    TrackerDebugInfoStripped();
    TrackerDebugInfoStripped(const TrackerDebugInfo& infoFull);
    
    // operators
    TrackerDebugInfoStripped& operator+=(const TrackerDebugInfoStripped& other);
    TrackerDebugInfoStripped operator/(float factor) const;
};
    
} // end of namespace

#endif
