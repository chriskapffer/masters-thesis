//
//  ObjectTrackerDebugger.cpp
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 24.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#include "ObjectTrackerDebugger.h"

using namespace std;
using namespace cv;

namespace ck {

string ObjectTrackerDebugger::debugString(TrackerDebugInfo info)
{
    return string();
}

vector<Mat> ObjectTrackerDebugger::debugImages(TrackerDebugInfo info)
{
    return vector<Mat>();
}

} // end of namespace
