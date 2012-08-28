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
    cv::Rect searchRect;
    
    // validation
    std::vector<std::pair<std::string, std::vector<cv::DMatch> > > namedMatches;
    std::vector<cv::Point2f> transformedObjectCorners;
    std::vector<cv::KeyPoint> objectKeyPoints;
    std::vector<cv::KeyPoint> sceneKeyPoints;

    cv::Mat objectImage;
    cv::Mat sceneImageFull;
    cv::Mat sceneImagePart;
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
    TrackerDebugInfoStripped() {
        currentModuleType = "undefined";
        subTaskProcessingTimes = std::vector<std::pair<std::string, double> >();
        totalProcessingTime = 0;
        objectKeyPointCount = 0;
        sceneKeyPointCount = 0;
        badHomography = false;
        
        namedMatchCounts = std::vector<std::pair<std::string, int> >();
    }
    
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
    
    // operators
    TrackerDebugInfoStripped& operator+=(const TrackerDebugInfoStripped& other)
    {
        // general
        assert(currentModuleType == other.currentModuleType);
        totalProcessingTime += other.totalProcessingTime;
        for (int i = 0; i < other.subTaskProcessingTimes.size(); i++) {
            bool found = false;
            for (int j = 0; j < subTaskProcessingTimes.size(); j++) {
                if (subTaskProcessingTimes[j].first == other.subTaskProcessingTimes[i].first) {
                    subTaskProcessingTimes[j].second += other.subTaskProcessingTimes[i].second;
                    found = true;
                    break;
                }
            }
            if (!found) {
                subTaskProcessingTimes.push_back(other.subTaskProcessingTimes[i]);
            }
        }

        // detection
        // validation
        objectKeyPointCount += other.objectKeyPointCount; 
        sceneKeyPointCount += other.sceneKeyPointCount;
        badHomography |= other.badHomography;
        for (int i = 0; i < other.namedMatchCounts.size(); i++) {
            bool found = false;
            for (int j = 0; j < namedMatchCounts.size(); j++) {
                if (namedMatchCounts[j].first == other.namedMatchCounts[i].first) {
                    namedMatchCounts[j].second += other.namedMatchCounts[i].second;
                    found = true;
                    break;
                }
            }
            if (!found) {
                namedMatchCounts.push_back(other.namedMatchCounts[i]);
            }
        }
        
        // tracking
        
        return *this;
    }
    
    TrackerDebugInfoStripped operator/(float factor) const
    {
        TrackerDebugInfoStripped res;
        res.currentModuleType = currentModuleType;
        res.totalProcessingTime = totalProcessingTime / factor;
        for (int i = 0; i < subTaskProcessingTimes.size(); i++) {
            std::pair<std::string, double> thisPair = subTaskProcessingTimes[i];
            res.subTaskProcessingTimes.push_back(std::make_pair(thisPair.first, thisPair.second / factor));
        }
        
        // detection
        
        // validation
        res.objectKeyPointCount = objectKeyPointCount / factor;
        res.sceneKeyPointCount = sceneKeyPointCount / factor;
        res.badHomography = badHomography;
        for (int i = 0; i < namedMatchCounts.size(); i++) {
            std::pair<std::string, int> thisPair = namedMatchCounts[i];
            res.namedMatchCounts.push_back(std::make_pair(thisPair.first, thisPair.second / factor));
        }
        // tracking
        
        return res;
    }
};
    
} // end of namespace
    
#endif
