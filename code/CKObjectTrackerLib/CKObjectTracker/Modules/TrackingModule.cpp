//
//  TrackingModule.cpp
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 22.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#include "TrackingModule.h"
#include "Profiler.h"
#include "Utils.h"

using namespace std;
using namespace cv;

namespace ck {

TrackingModule::TrackingModule() : AbstractModule(MODULE_TYPE_TRACKING)
{
    
}

TrackingModule::~TrackingModule()
{
    
}

void TrackingModule::initWithObjectImage(const cv::Mat &objectImage)
{
    
}

bool TrackingModule::internalProcess(ModuleInOutParams& params, ModuleDebugParams& debugInfo)
{
    return true;
}

} // end of namespace
    
//bool TrackingModule::track(const Mat& frame, const CKObjectData& objData, CKTrackingResult& previous, CKTrackingResult& current, CKTrackingInfo& info)
//{
//    vector<float> error;
//    vector<uchar> status;
//    Mat gray, prevGray;
//    cvtColor(frame, gray, CV_BGRA2GRAY);
//    cvtColor(previous.processedFrame, prevGray, CV_BGRA2GRAY);
//    
//    TermCriteria termcrit(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03);
//    Size subPixWinSize(10,10), winSize(31,31);
//    cornerSubPix(prevGray, previous.points, subPixWinSize, Size(-1,-1), termcrit);
//    calcOpticalFlowPyrLK(prevGray, gray, previous.points, current.points, status, error, winSize, 3, termcrit, 0, 0.001);
//    cornerSubPix(prevGray, current.points, subPixWinSize, Size(-1,-1), termcrit);
//    
//    int method = CV_RANSAC;
//    int _ransacThreshold = 3;
//    Mat homographyDelta;
//    Profiler* profiler = Profiler::Instance();
//    profiler->startTimer(HOMOGRAPHY_TIMER_NAME);
//    Utils::calcHomography(previous.points, current.points, homographyDelta, method, _ransacThreshold);
//    current.homography = previous.homography * homographyDelta;
//    profiler->stopTimer(HOMOGRAPHY_TIMER_NAME);
//    
//    info.corners.clear();
//    int width = objData.image.cols;
//    int height = objData.image.rows;
//    vector<Point2f> objectCorners(4);
//    vector<Point2f> objectCornersTransformed;
//    
//    objectCorners[0] = cvPoint(    0,      0);
//    objectCorners[1] = cvPoint(width,      0);
//    objectCorners[2] = cvPoint(width, height);
//    objectCorners[3] = cvPoint(    0, height);
//    perspectiveTransform(objectCorners, objectCornersTransformed, current.homography);
//    info.corners = objectCornersTransformed;
//    
//    current.valid = true;
//    return true;
//}

// opticalflow, camshift, meanshift, kalman