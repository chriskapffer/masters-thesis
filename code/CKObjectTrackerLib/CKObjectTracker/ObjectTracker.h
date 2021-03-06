//
//  ObjectTracker.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 17.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTracker_ObjectTracker_h
#define CKObjectTracker_ObjectTracker_h

#include "ObjectTrackerTypesPublic.h"

namespace ck {

class ObjectTracker {

public:
    ObjectTracker();
    ~ObjectTracker();
    
    const Settings getSettings() const;
    
    void setFocalLength(const cv::Point2f& focalLength);
    cv::Point2f getFocalLength() const;
    void setPrincipalPoint(const cv::Point2f& principalPoint);
    cv::Point2f getPrincipalPoint() const;
    
    void setObject(const cv::Mat& objectImage);
    void trackObjectInVideo(const cv::Mat& frame, TrackerOutput& output, TrackerDebugInfo& debugInfo);
    void trackObjectInStillImage(const cv::Mat& image, std::vector<TrackerOutput>& output, std::vector<TrackerDebugInfo>& debugInfo);
    
private:
    class Creator;
    class Implementation;
    Implementation* mImpl;
};

} // end of namespace

#endif
