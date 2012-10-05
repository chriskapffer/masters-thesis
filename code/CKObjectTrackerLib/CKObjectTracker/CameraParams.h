//
//  CameraParams.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 05.10.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_CameraParams_h
#define CKObjectTrackerLib_CameraParams_h

#include "Singleton.h"

namespace ck {

class CameraParams {
    DECLARE_SINGLETON(CameraParams)
    
public:
    
    void setFocalLength(const cv::Point2f& focalLength);
    void setPrincipalPoint(const cv::Point2f& principalPoint);
    
    inline cv::Point2f getFocalLength() const { return _focalLength; }
    inline cv::Point2f getPrincipalPoint() const { return _principalPoint; }
    inline const cv::Mat getIntrinsics() const { return _intrinsics; }
    inline const cv::Mat getInverseIntrinsics() const { return _inverseIntrinsics; }
    
private:
    cv::Point2f _focalLength;
    cv::Point2f _principalPoint;
    cv::Mat _intrinsics;
    cv::Mat _inverseIntrinsics;
};

}

#endif
