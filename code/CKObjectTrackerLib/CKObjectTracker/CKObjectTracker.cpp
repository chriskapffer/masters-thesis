//
//  CKObjectTracker.cpp
//  CKObjectTracker
//
//  Created by Christoph Kapffer on 17.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#include "CKObjectTracker.h"

using namespace std;
using namespace cv;

CKObjectTracker::CKObjectTracker(bool inputIsStillImage)
{
    _inputIsStillImage = inputIsStillImage;
}

void CKObjectTracker::setObject(const cv::Mat& objectImage)
{

}

bool CKObjectTracker::track(const cv::Mat& frame, CKTrackingResult& result, CKTrackingInfo& info)
{
    return false;
}