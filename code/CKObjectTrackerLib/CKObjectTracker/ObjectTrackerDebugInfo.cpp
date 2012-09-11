//
//  ObjectTrackerDebugInfo.cpp
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 06.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#include "ObjectTrackerDebugInfo.h"
#include "Commons.h"

using namespace std;

namespace ck {
    
TrackerDebugInfoStripped::TrackerDebugInfoStripped() {
    // general
    totalProcessingTime = 0;
    subTaskProcessingTimes = vector<pair<string, double> >();
    currentModuleType = "undefined";
    
    // validation
    namedMatchCounts = vector<pair<string, float> >();
    objectKeyPointCount = 0;
    sceneKeyPointCount = 0;
    
    // tracking
    namedPointCounts = vector<pair<string, float> >();
    initialPointCount = 0;
    transformationDelta = 0;
    distortion = 0;
    avgError = 0;
    
    // validation and tracking
    badHomography = false;
}

TrackerDebugInfoStripped::TrackerDebugInfoStripped(const TrackerDebugInfo& infoFull) {
    // general
    totalProcessingTime = infoFull.totalProcessingTime;
    subTaskProcessingTimes = infoFull.subTaskProcessingTimes;
    currentModuleType = infoFull.currentModuleType;
    
    // validation
    setItemCounts(infoFull.namedMatches, namedMatchCounts);
    objectKeyPointCount = (int)infoFull.objectKeyPoints.size();
    sceneKeyPointCount = (int)infoFull.sceneKeyPoints.size();
    
    // tracking
    setItemCounts(infoFull.namedPointSets, namedPointCounts);
    initialPointCount = infoFull.initialPointCount;
    transformationDelta = infoFull.transformationDelta;
    distortion = infoFull.distortion;
    avgError = infoFull.avgError;
    
    // validation and tracking
    badHomography = infoFull.badHomography;
}

TrackerDebugInfoStripped& TrackerDebugInfoStripped::operator+=(const TrackerDebugInfoStripped& other)
{
    assert(currentModuleType == other.currentModuleType);
    // general
    totalProcessingTime += other.totalProcessingTime;
    sumItems(other.subTaskProcessingTimes, subTaskProcessingTimes);
    
    // validation
    sumItems(other.namedMatchCounts, namedMatchCounts);
    objectKeyPointCount += other.objectKeyPointCount;
    sceneKeyPointCount += other.sceneKeyPointCount;
    
    // tracking
    sumItems(other.namedPointCounts, namedPointCounts);
    initialPointCount += other.initialPointCount;
    transformationDelta += other.transformationDelta;
    distortion += other.distortion;
    avgError += other.avgError;
    
    // validation and tracking
    badHomography |= other.badHomography;

    return *this;
}
    
TrackerDebugInfoStripped TrackerDebugInfoStripped::operator/(float factor) const
{
    TrackerDebugInfoStripped result;
    // general
    result.totalProcessingTime = totalProcessingTime / factor;
    divideItems(subTaskProcessingTimes, result.subTaskProcessingTimes, factor);
    result.currentModuleType = currentModuleType;
    
    // validation
    divideItems(namedMatchCounts, result.namedMatchCounts, factor);
    result.objectKeyPointCount = objectKeyPointCount / factor;
    result.sceneKeyPointCount = sceneKeyPointCount / factor;
    
    // tracking
    divideItems(namedPointCounts, result.namedPointCounts, factor);
    result.initialPointCount = initialPointCount / factor;
    result.transformationDelta = transformationDelta / factor;
    result.distortion = distortion / factor;
    result.avgError = avgError / factor;
    
    // validation and tracking
    result.badHomography = badHomography;
    
    return result;
}
    
} // end of namespace