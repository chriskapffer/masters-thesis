//
//  ObjectTrackerTypeDefinitions.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 25.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_ObjectTrackerTypeDefinitions_h
#define CKObjectTrackerLib_ObjectTrackerTypeDefinitions_h

namespace ck {

struct TrackerOutput {
    cv::Mat homography;
    bool isObjectPresent;
    bool failed;
};

struct TrackerDebugInfo {
    // general
    std::string currentModuleType;
    std::vector<std::pair<std::string, double> > subTaskProcessingTimes;
    double totalProcessingTime;
    
    // detection
    
    // validation
    std::vector<std::pair<std::string, std::vector<cv::DMatch> > > namedMatches;
    std::vector<cv::Point2f> transformedObjectCorners;
    std::vector<cv::KeyPoint> objectKeyPoints;
    std::vector<cv::KeyPoint> sceneKeyPoints;

    cv::Mat objectImage;
    cv::Mat sceneImage;
    cv::Mat homography;
    bool badHomography;

    // tracking
};

struct TrackerDebugInfoStripped
{
    // general
    std::string currentModuleType;
    std::vector<std::pair<std::string, double> > subTaskProcessingTimes;
    double totalProcessingTime;
    
    // detection
    
    // validation
    std::vector<std::pair<std::string, int> > namedMatchCounts;
    int objectKeyPointCount;
    int sceneKeyPointCount;
    bool badHomography;
    
    // tracking

    // constructor
    TrackerDebugInfoStripped(const TrackerDebugInfo& params) {
        currentModuleType = params.currentModuleType;
        subTaskProcessingTimes = params.subTaskProcessingTimes;
        totalProcessingTime = params.totalProcessingTime;
        objectKeyPointCount = (int)params.objectKeyPoints.size();
        sceneKeyPointCount = (int)params.sceneKeyPoints.size();
        badHomography = params.badHomography;
        
        namedMatchCounts = std::vector<std::pair<std::string, int> >();
        std::vector<std::pair<std::string, std::vector<cv::DMatch> > >::const_iterator iter;
        for (iter = params.namedMatches.begin(); iter != params.namedMatches.end(); iter++) {
            namedMatchCounts.push_back(make_pair((*iter).first, (*iter).second.size()));
        }
    }
};
    
} // end of namespace
    
#endif
