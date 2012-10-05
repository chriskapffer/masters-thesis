//
//  CameraParams.cpp
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 05.10.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#include "CameraParams.h"
#include "ObjectTrackerTypesProject.h"

using namespace cv;

namespace ck {
    
    DEFINE_SINGLETON(CameraParams)
    
    CameraParams::CameraParams()
    {
        _focalLength = Point2f(DEFAULT_FOCAL_LENGTH_X, DEFAULT_FOCAL_LENGTH_Y);
        _principalPoint = Point2f(DEFAULT_PRINCIPAL_POINT_X, DEFAULT_PRINCIPAL_POINT_Y);
        _intrinsics = (Mat_<double>(3,3) <<
            _focalLength.x,              0, _principalPoint.x,
                         0, _focalLength.y, _principalPoint.y,
                         0,              0,                 1);
        _inverseIntrinsics = _intrinsics.inv();
    }
    
    void CameraParams::setFocalLength(const cv::Point2f& focalLength)
    {
        _focalLength = focalLength;
        _intrinsics.at<double>(0,0) = _focalLength.x;
        _intrinsics.at<double>(1,1) = _focalLength.y;
        _inverseIntrinsics = _intrinsics.inv();
        std::cout << _intrinsics << std::endl;
    }
    
    void CameraParams::setPrincipalPoint(const cv::Point2f& principalPoint)
    {
        _principalPoint = principalPoint;
        _intrinsics.at<double>(0,2) = _principalPoint.x;
        _intrinsics.at<double>(1,2) = _principalPoint.y;
        _inverseIntrinsics = _intrinsics.inv();
        std::cout << _intrinsics << std::endl;
    }
}

