//
//  ObjectTrackerTypesProject.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 06.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_ObjectTrackerTypesProject_h
#define CKObjectTrackerLib_ObjectTrackerTypesProject_h

#define TIMER_CONVERT "converting"
#define TIMER_DETECT "detecting"
#define TIMER_EXTRACT "extracting"
#define TIMER_MATCH "matching"
#define TIMER_OPTIMIZE "optimizing"
#define TIMER_TRACK "tracking"
#define TIMER_FILTER "filtering"
#define TIMER_ESTIMATE "estimating"
#define TIMER_VALIDATE "validating"
#define TIMER_TOTAL "TotalProcessingTimeAbstract"

#define MATCHES_TOTAL "total"
#define MATCHES_MASKED "masked"
#define MATCHES_CROPPED "cropped"
#define MATCHES_AFTER_RATIO "ratio"
#define MATCHES_AFTER_SYMMETRY "symmetry"

#define POINTS_TOTAL "initializing"
#define POINTS_TRACKED "tracking"
#define POINTS_FILTERED "filtering"

#define EST_METHOD_RANSAC "Ransac"
#define EST_METHOD_LMEDS "Least Median"
#define EST_METHOD_DEFAULT "All Points"

#define MIN_HOMOGRAPHY_INPUT 4
#define MIN_VALID_ANGLE_DEG 70.0f

#define DEFAULT_FOCAL_LENGTH_X 600
#define DEFAULT_FOCAL_LENGTH_Y 600
#define DEFAULT_PRINCIPAL_POINT_X 320
#define DEFAULT_PRINCIPAL_POINT_Y 240

namespace ck {

} // end of namespace

#endif
