//
//  TrackingModule.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 22.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_TrackingModule_h
#define CKObjectTrackerLib_TrackingModule_h

#include "AbstractModule.h"

namespace ck {

class TrackingModule : public AbstractModule {

public:
    TrackingModule(int maxPoints, int minPointsAbs = 10, float minPointsRel = 0.1f, bool useSubPixels = false, bool calcHomRelToFrame = false, bool filterDistortions = true, float distortionThreshold = 1.0f);
    ~TrackingModule();

    void initWithObjectImage(const cv::Mat& objectImage);
    bool internalProcess(ModuleParams& params, TrackerDebugInfo& debugInfo);
    
#pragma mark
    
    inline void setEnabled(const bool& value) { _enabled = value; }
    inline bool getEnabled() const { return _enabled; }
    
    inline void setMaxTransformationDelta(const int& value) { _maxTransformationDelta = value; }
    inline int getMaxTransformationDelta() const { return _maxTransformationDelta; }
    
    inline void setMaxSuccessiveFrames(const int& value) { _maxSuccessiveFrames = value; }
    inline int getMaxSuccessiveFrames() const { return _maxSuccessiveFrames; }
    
    inline void setMaxPointsAbsolute(const int& value) { _maxPointsAbsolute = value; }
    inline int getMaxPointsAbsolute() const { return _maxPointsAbsolute; }
    
    inline void setMinPointsAbsolute(const int& value) { _minPointsAbsolute = value; }
    inline int getMinPointsAbsolute() const { return _minPointsAbsolute; }
    
    inline void setMinPointsRelative(const int& value) { _minPointsRelative = value / 100.0f; }
    inline int getMinPointsRelative() const { return _minPointsRelative * 100; }
    
    inline void setUseSubPixels(const bool& value) { _useSubPixels = value; }
    inline bool getUseSubPixels() const { return _useSubPixels; }
    
    inline void setCalcHomRelToFrame(const bool& value) { _calcHomRelToFrame = value; }
    inline bool getCalcHomRelToFrame() const { return _calcHomRelToFrame; }
    
    inline void setFilterDistortions(const bool& value) { _filterDistortions = value; }
    inline bool getFilterDistortions() const { return _filterDistortions; }
    
    inline void setDistortionThreshold(const float& value) { _distortionThreshold = value; }
    inline float getDistortionThreshold() const { return _distortionThreshold; }

    inline void setSPWinSize(const int& value) { _winSizeSubPix = cv::Size(value, value); }
    inline int getSPWinSize() const { return MIN(_winSizeSubPix.width, _winSizeSubPix.height); }
    
    inline void setLKTermIter(const int& value) { _terminationCriteria.maxCount = value; }
    inline int getLKTermIter() const { return _terminationCriteria.maxCount; }

    inline void setLKTermEpsilon(const float& value) { _terminationCriteria.epsilon = value; }
    inline float getLKTermEpsilon() const { return _terminationCriteria.epsilon; }
    
    inline void setLKWinSize(const int& value) { _winSizeFlow = cv::Size(value, value); }
    inline int getLKWinSize() const { return MIN(_winSizeFlow.width, _winSizeFlow.height); }
    
    inline void setLKZeroZone(const int& value) { _zeroZone = cv::Size(value, value); }
    inline int getLKZeroZone() const { return MIN(_zeroZone.width, _zeroZone.height); }
    
    inline void setLKMinEigenThreshold(const float& value) { _minEigenThreshold = value; }
    inline float getLKMinEigenThreshold() const { return _minEigenThreshold; }
    
    inline void setLKMaxLevel(const int& value) { _maxLevel = value; }
    inline int getLKMaxLevel() const { return _maxLevel; }
    
    inline void setLKFlags(const int& value) { _lkFlags = value; }
    inline int getLKFlags() const { return _lkFlags; }

private:
    bool prepareForProcessing(const cv::Mat& homography, std::vector<cv::Point2f>& pointsIn, std::vector<cv::Point2f>& prevCorners, TrackerDebugInfo& debugInfo);
    
    static void removeUntrackedPoints(std::vector<cv::Point2f>& pointsIn, std::vector<cv::Point2f>& pointsOut, std::vector<cv::Point2f>& initialPoints, const std::vector<uchar>& status, const std::vector<float>& error, float& avgError, float& avgDistance, float& maxDistance, float& minDistance);
    static void filterPointsByMovingDistance(std::vector<cv::Point2f>& pointsIn, std::vector<cv::Point2f>& pointsOut, std::vector<cv::Point2f>& initialPoints, float averageDistance, float distortionThreshold);
    
    // tracker params
    bool _enabled;
    
    float _maxTransformationDelta;
    int _maxSuccessiveFrames;
    int _maxPointsAbsolute;
    int _minPointsAbsolute;
    float _minPointsRelative;
    
    bool _useSubPixels;
    bool _calcHomRelToFrame;
    bool _filterDistortions;
    float _distortionThreshold;
    
    // optical flow and sub pix params
    cv::TermCriteria _terminationCriteria;
    cv::Size _winSizeSubPix;
    cv::Size _winSizeFlow;
    cv::Size _zeroZone;
    float _minEigenThreshold;
    int _maxLevel;
    int _lkFlags;

    // internal data
    std::vector<cv::Point2f> _objectCorners;
    std::vector<cv::Point2f> _initialPoints;
    int _initialPointCount;
    cv::Mat _initialHomography;
    bool _isInitialCall;
    int _succFrameCount;
};

} // end of namespace
    
#endif
