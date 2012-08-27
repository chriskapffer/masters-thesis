//
//  ObjectTrackerDebugger.cpp
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 24.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#include "ObjectTrackerDebugger.h"
#include "ModuleTypes.h"

using namespace std;
using namespace cv;

namespace ck {

string ObjectTrackerDebugger::debugString(TrackerDebugInfoStripped info)
{
    string debugString = string();
    if (info.currentModuleType == (*moduleTypeString.find(MODULE_TYPE_VALIDATION)).second) {
        
    }
    
    return debugString;
}

vector<pair<string, Mat> > ObjectTrackerDebugger::debugImages(TrackerDebugInfo info, bool drawTransformedRect, bool drawFilteredMatches, bool drawAllMatchess)
{
    vector<pair<string, Mat> > debugImages = vector<pair<string, Mat> >();
    if (info.currentModuleType == (*moduleTypeString.find(MODULE_TYPE_VALIDATION)).second) {
        Mat validationImage = info.sceneImage;
        vector<Point2f> corners = info.transformedObjectCorners;
        if (corners.size() == 4) {
            int lineWidth = 2;
            Scalar color = info.badHomography ? Scalar(0, 0, 255) : Scalar(0, 255, 0);
            line(validationImage, corners[0], corners[1], color, lineWidth);
            line(validationImage, corners[1], corners[2], color, lineWidth);
            line(validationImage, corners[2], corners[3], color, lineWidth);
            line(validationImage, corners[3], corners[0], color, lineWidth);
        }
        debugImages.push_back(make_pair("validation", validationImage));
    }
    
    return debugImages;
}

} // end of namespace
