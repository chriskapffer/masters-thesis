//
//  DetectionModule.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 22.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_DetectionModule_h
#define CKObjectTrackerLib_DetectionModule_h

#include "AbstractModule.h"

namespace ck {

class DetectionModule : public AbstractModule {

public:

    DetectionModule();
    ~DetectionModule();
    void initWithObjectImage(const cv::Mat& objectImage);
    bool internalProcess(ModuleParams& params, TrackerDebugInfo& debugInfo);
    
#pragma mark
    
    inline void setEnabled(const bool& value) { _enabled = value; }
    inline bool getEnabled() const { return _enabled; }

    inline void setMinSearchRectSizeRelative(const int& value) { _minSearchRectSizeRelative = value / 100.0f; }
    inline int getMinSearchRectSizeRelative() const { return _minSearchRectSizeRelative * 100; }
    
    inline void setMaxSearchRectSizeRelative(const int& value) { _maxSearchRectSizeRelative = value / 100.0f; }
    inline int getMaxSearchRectSizeRelative() const { return _maxSearchRectSizeRelative * 100; }
    
    inline void setCamShiftTermIter(const int& value) { _terminationCriteria.maxCount = value; }
    inline int getCamShiftTermIter() const { return _terminationCriteria.maxCount; }
    
    inline void setCamShiftTermEpsilon(const float& value) { _terminationCriteria.epsilon = value; }
    inline float getCamShiftTermEpsilon() const { return _terminationCriteria.epsilon; }
    
private:
    bool isRectValid(const cv::Rect& rect, int imageWidth, int imageHeight);
    
    cv::Mat _objectImage;
    
    cv::MatND _objectHist;
    cv::TermCriteria _terminationCriteria;
    float _minSearchRectSizeRelative;
    float _maxSearchRectSizeRelative;
    bool _enabled;
};

} // end of namespace
    
#endif
