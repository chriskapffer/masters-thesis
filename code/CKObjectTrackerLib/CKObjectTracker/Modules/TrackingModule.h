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

private:
    cv::TermCriteria _terminationCriteria;
    cv::Size _winSizeSubPix;
    cv::Size _winSizeFlow;
    cv::Size _zeroZone;
    float _minEigenThreshold;
    int _maxLevel;
    int _lkFlags;
    
    int _maxPointsAbsolute;
    int _minPointsAbsolute;
    float _minPointsRelative;
    float _regularityThreshold;
    bool _filterIrregularPoints;
    bool _useSubPixels;
    bool _byPass;
    int _maxSuccessiveFrames;
    int _succFrameCount;
    
    bool _isInitialPointSet;
    int _initialCount;
 
    std::vector<cv::Point2f> _objectCorners;
    
public:
    TrackingModule();
    ~TrackingModule();
    void initWithObjectImage(const cv::Mat& objectImage);
    bool internalProcess(ModuleParams& params, TrackerDebugInfo& debugInfo);
    
};

} // end of namespace
    
#endif
