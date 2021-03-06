//
//  ObjectTrackerTypesPublic.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 06.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_ObjectTrackerTypesPublic_h
#define CKObjectTrackerLib_ObjectTrackerTypesPublic_h

#include "SettingsPublicScope.h"
#include "ObjectTrackerDebugInfo.h"

#define OT_SETTING_DETEC_ENABLED "Detection Enabled"
#define OT_SETTING_DETEC_SEARCH_RECT_MIN "Search Rect Min (% image size)"
#define OT_SETTING_DETEC_SEARCH_RECT_MAX "Search Rect Max (% image size)"
#define OT_SETTING_DETEC_CAMSHIFT_TERM_MAX_ITER "max Iterations (until termination)"
#define OT_SETTING_DETEC_CAMSHIFT_TERM_MIN_EPS "min Epsilon (until termination)"

#define OT_SETTING_VALID_DETECTOR "Feature Detector"
#define OT_SETTING_VALID_EXTRACTOR "Descriptor Extractor"
#define OT_SETTING_VALID_EST_METHOD "Estimation Method"

#define OT_SETTING_VALID_CVT_TO_GRAY "Convert to Gray"
#define OT_SETTING_VALID_ALL_POINTS_OUT "Use all Points for Tracking"
#define OT_SETTING_VALID_REFINE_HOMOGRAPHY "Refine Homography"

#define OT_SETTING_VALID_MATCHES_SORT "Sort Matches"
#define OT_SETTING_VALID_MATCHES_CROP "Crop Matches"

#define OT_SETTING_VALID_FILTER_SYMM "Symmetry Test"
#define OT_SETTING_VALID_FILTER_RATIO "Ratio Test"

#define OT_SETTING_VALID_N_FEATURES "Number of Features"
#define OT_SETTING_VALID_N_BEST_MATCHES "n Best Matches"

#define OT_SETTING_VALID_RATIO_THRESHLD "Ratio"
#define OT_SETTING_VALID_RANSAC_THRESHLD "Ransac Threshold"
#define OT_SETTING_VALID_FAST_THRESHLD "Fast Threshold (FAST)"
#define OT_SETTING_VALID_HESS_THRESHLD "Hessian Threshold (SURF)"

#define OT_SETTING_TRACK_ENABLED "Tracking Enabled"

#define OT_SETTING_TRACK_MAX_PTS "Number of Points"
#define OT_SETTING_TRACK_MIN_PTS_ABS "n Points remaining"
#define OT_SETTING_TRACK_MIN_PTS_REL "% Points remaining"
#define OT_SETTING_TRACK_MAX_SUCC_FRAMES "max successive Frames (-1 -> oo)"
#define OT_SETTING_TRACK_MAX_DELTA_TRANS "max inter-frame Movement (in px)"

#define OT_SETTING_TRACK_TERM_MAX_ITER "max Iterations (until termination)"
#define OT_SETTING_TRACK_TERM_MIN_EPS "min Epsilon (until termination)"
#define OT_SETTING_TRACK_LK_SEARCH_WIN "Search Window Size"
#define OT_SETTING_TRACK_LK_MAX_PYR_LVS "max Pyramid Levels"
#define OT_SETTING_TRACK_LK_MIN_EIG_THRESHLD "min Eigenvalue Threshold"

#define OT_SETTING_TRACK_USE_SUB_PX "Sub-Pixel Accuracy"
#define OT_SETTING_TRACK_INTERFRAME_HOMOGRAPHY "calc Homography from frame to frame"
#define OT_SETTING_TRACK_FILTER_DIVERGE "Divergence Filter"
#define OT_SETTING_TRACK_DIVERGE_THRESHLD "Divergence Threshold"

namespace ck {

    struct Transform {
        float scale;
        cv::Mat rotation;
        cv::Point2f translation;
    };
    
    struct TrackerOutput {
        Transform objectInfo;
        bool isObjectPresent;
        bool failed;
    };
    
} // end of namespace

#endif
