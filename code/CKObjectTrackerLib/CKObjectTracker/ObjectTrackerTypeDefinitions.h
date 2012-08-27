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
    std::string currentModuleType;
    std::vector<double> subTaskProcessingTimes;
    double totalProcessingTime;
    
    
    // validation
    std::vector<std::pair<std::string, std::vector<cv::DMatch> > > namedMatches;
    cv::Mat sceneImage;
};

struct TrackerDebugInfoStripped
{
    
    // validation
    std::vector<std::pair<std::string, int> > namedMatchCounts;
    
    TrackerDebugInfoStripped(const TrackerDebugInfo& params) {
        
    }
};
    
} // end of namespace
    
#endif
