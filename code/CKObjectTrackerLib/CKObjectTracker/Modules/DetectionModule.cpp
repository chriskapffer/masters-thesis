//
//  DetectionModule.cpp
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 22.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#include "DetectionModule.h"

using namespace std;
using namespace cv;

namespace ck {

DetectionModule::DetectionModule() : AbstractModule(MODULE_TYPE_DETECTION)
{

}

DetectionModule::~DetectionModule()
{

}

void DetectionModule::initWithObjectImage(const cv::Mat &objectImage)
{

}

bool DetectionModule::internalProcess(ModuleInOutParams& params, ModuleDebugParams& debugInfo)
{
    return true;
}

} // end of namespaces
    
// preprocess, erode, delate, blur, reduce colors

// moments, histogram comparison, backprojection, mser, blob, chamfer

// angucken: http://docs.opencv.org/modules/imgproc/doc/miscellaneous_transformations.html
// angucken: http://docs.opencv.org/modules/imgproc/doc/structural_analysis_and_shape_descriptors.html