//
//  CKObjectTracker.h
//  CKObjectTracker
//
//  Created by Christoph Kapffer on 17.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTracker_CKObjectTracker_h
#define CKObjectTracker_CKObjectTracker_h

#define CK_TRACKING_STATE_INIT     0
#define CK_TRACKING_STATE_DETECT   1
#define CK_TRACKING_STATE_VALIDATE 2
#define CK_TRACKING_STATE_TRACK    3
#define CK_TRACKING_STATE_RECOVER  4
#define CK_TRACKING_STATE_FAIL     5

struct CKObjectData {
    cv::Mat image;
    cv::MatND histColor;
    cv::MatND histShape;
    std::vector<cv::KeyPoint> keypoints;
};

struct CKTrackingResult {
    cv::Mat object;
    cv::Mat homography;
    cv::Rect searchRect;
    std::vector<cv::Point2f> points;
};

struct CKTrackingInfo {
    int trackingState;
};

class CKObjectTracker {
    
public:
    explicit CKObjectTracker(bool inputIsStillImage = false);
    
    void setObject(const cv::Mat& objectImage);
    bool track(const cv::Mat& frame, CKTrackingResult& result, CKTrackingInfo& info);
    
private:
    bool _inputIsStillImage;
    CKObjectData _objectData;
    CKTrackingResult _lastFrame;
};

#endif
