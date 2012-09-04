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

struct CMatch {
    int queryIdx;
    int trainIdx;
    float score;
    CMatch(int queryIdx, int trainIdx, float score) : queryIdx(queryIdx), trainIdx(trainIdx), score(score) {};
};
    
struct TrackerDebugInfo {
    // general
    std::string currentModuleType;
    std::vector<std::pair<std::string, double> > subTaskProcessingTimes;
    double totalProcessingTime;
    
    // detection
    cv::Mat probabilityMap;
    cv::Rect searchRect;
    
    // validation
    std::vector<std::pair<std::string, std::vector<cv::DMatch> > > namedMatches;
    std::vector<cv::KeyPoint> objectKeyPoints;
    std::vector<cv::KeyPoint> sceneKeyPoints;
    
    cv::Mat objectImage;
    cv::Mat sceneImageFull;
    cv::Mat sceneImagePart;
    
    // validation and tracking
    std::vector<cv::Point2f> objectCornersTransformed;
    cv::Mat homography;
    bool badHomography;
    
    // tracking
    std::vector<std::pair<std::string, std::vector<cv::Point2f> > > namedPointSets;
    int initialPointCount;
    float movementVariation;
    float jitterAmount;
    float avgError;
    
    TrackerDebugInfo() : totalProcessingTime(0), initialPointCount(0), movementVariation(0), jitterAmount(0), avgError(0), badHomography(false) {}
};

struct TrackerDebugInfoStripped
{
    // general
    std::string currentModuleType;
    std::vector<std::pair<std::string, double> > subTaskProcessingTimes;
    double totalProcessingTime;
    
    // validation
    std::vector<std::pair<std::string, int> > namedMatchCounts;
    int objectKeyPointCount;
    int sceneKeyPointCount;

    // validation and tracking
    bool badHomography;
    
    // tracking
    std::vector<std::pair<std::string, int> > namedPointCounts;
    int initialPointCount;
    float movementVariation;
    float jitterAmount;
    float avgError;
    
    // constructor
    TrackerDebugInfoStripped() {
        currentModuleType = "undefined";
        subTaskProcessingTimes = std::vector<std::pair<std::string, double> >();
        totalProcessingTime = 0;
        objectKeyPointCount = 0;
        sceneKeyPointCount = 0;
        badHomography = false;
        
        namedMatchCounts = std::vector<std::pair<std::string, int> >();
        namedPointCounts = std::vector<std::pair<std::string, int> >();
        initialPointCount = 0;
        movementVariation = 0;
        jitterAmount = 0;
        avgError = 0;
    }
    
    TrackerDebugInfoStripped(const TrackerDebugInfo& params) {
        currentModuleType = params.currentModuleType;
        subTaskProcessingTimes = params.subTaskProcessingTimes;
        totalProcessingTime = params.totalProcessingTime;
        objectKeyPointCount = (int)params.objectKeyPoints.size();
        sceneKeyPointCount = (int)params.sceneKeyPoints.size();
        badHomography = params.badHomography;
        
        namedMatchCounts = std::vector<std::pair<std::string, int> >();
        std::vector<std::pair<std::string, std::vector<cv::DMatch> > >::const_iterator iterM;
        for (iterM = params.namedMatches.begin(); iterM != params.namedMatches.end(); iterM++) {
            namedMatchCounts.push_back(make_pair((*iterM).first, (*iterM).second.size()));
        }
        namedPointCounts = std::vector<std::pair<std::string, int> >();
        std::vector<std::pair<std::string, std::vector<cv::Point2f> > >::const_iterator iterP;
        for (iterP = params.namedPointSets.begin(); iterP != params.namedPointSets.end(); iterP++) {
            namedPointCounts.push_back(make_pair((*iterP).first, (*iterP).second.size()));
        }
        initialPointCount = params.initialPointCount;
        movementVariation = params.movementVariation;
        jitterAmount = params.jitterAmount;
        avgError = params.avgError;
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
        for (int i = 0; i < other.namedPointCounts.size(); i++) {
            bool found = false;
            for (int j = 0; j < namedPointCounts.size(); j++) {
                if (namedPointCounts[j].first == other.namedPointCounts[i].first) {
                    namedPointCounts[j].second += other.namedPointCounts[i].second;
                    found = true;
                    break;
                }
            }
            if (!found) {
                namedPointCounts.push_back(other.namedPointCounts[i]);
            }
        }
        initialPointCount += other.initialPointCount;
        movementVariation += other.movementVariation;
        jitterAmount += other.jitterAmount;
        avgError += other.avgError;
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
        
        // validation
        res.objectKeyPointCount = objectKeyPointCount / factor;
        res.sceneKeyPointCount = sceneKeyPointCount / factor;
        res.badHomography = badHomography;
        for (int i = 0; i < namedMatchCounts.size(); i++) {
            std::pair<std::string, int> thisPair = namedMatchCounts[i];
            res.namedMatchCounts.push_back(std::make_pair(thisPair.first, thisPair.second / factor));
        }
        // tracking
        for (int i = 0; i < namedPointCounts.size(); i++) {
            std::pair<std::string, int> thisPair = namedPointCounts[i];
            res.namedPointCounts.push_back(std::make_pair(thisPair.first, thisPair.second / factor));
        }
        res.initialPointCount = initialPointCount / factor;
        res.movementVariation = movementVariation / factor;
        res.jitterAmount = jitterAmount / factor;
        res.avgError = avgError / factor;
        return res;
    }
};
    
} // end of namespace
    
#endif
