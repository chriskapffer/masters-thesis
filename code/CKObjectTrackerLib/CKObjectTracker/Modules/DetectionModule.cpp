//
//  DetectionModule.cpp
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 22.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#include "DetectionModule.h"

#include "ObjectTrackerTypesProject.h"

#include "ColorConversion.h"
#include "PointOperations.h"
#include "Profiler.h"

using namespace std;
using namespace cv;

namespace ck {
    
DetectionModule::DetectionModule() : AbstractModule(MODULE_TYPE_DETECTION)
{
    _enabled = false;
    _terminationCriteria = TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 20, 0.1f);
    _minSearchRectSizeRelative = 0.1f;
    _maxSearchRectSizeRelative = 0.9f;
}

DetectionModule::~DetectionModule()
{

}

void DetectionModule::initWithObjectImage(const cv::Mat &objectImage)
{
    Mat hsv, hue, mask, hist;
    int vmin = 5, vmax = 256, smin = 60;
    int hsize = 16;
    float hranges[] = {0,180};
    const float* phranges = hranges;
    
    ColorConvert::bgrOrBgra2Hsv(objectImage, hsv);
    inRange(hsv, Scalar(0, smin, MIN(vmin,vmax)), Scalar(180, 256, MAX(vmin, vmax)), mask);
    
    int ch[] = {0, 0};
    hue.create(hsv.size(), hsv.depth());
    mixChannels(&hsv, 1, &hue, 1, ch, 1);
    
    calcHist(&hue, 1, 0, mask, hist, 1, &hsize, &phranges);
    normalize(hist, hist, 0, 255, CV_MINMAX);
    _objectHist = hist;
    
    Mat histimg = Mat::zeros(200, 320, CV_8UC3);
    histimg = Scalar::all(0);
    int binW = histimg.cols / hsize;
    Mat buf(1, hsize, CV_8UC3);
    for( int i = 0; i < hsize; i++ )
        buf.at<Vec3b>(i) = Vec3b(saturate_cast<uchar>(i*180./hsize), 255, 255);
    cvtColor(buf, buf, CV_HSV2BGR);
    
    for( int i = 0; i < hsize; i++ )
    {
        int val = saturate_cast<int>(hist.at<float>(i)*histimg.rows/255);
        rectangle( histimg, Point(i*binW,histimg.rows),
                  Point((i+1)*binW,histimg.rows - val),
                  Scalar(buf.at<Vec3b>(i)), -1, 8 );
    }
    //namedWindow("histimg");
    //imshow("histimg", histimg);
}
 
    

    
bool DetectionModule::internalProcess(ModuleParams& params, TrackerDebugInfo& debugInfo)
{
    if (!_enabled) {
        // search in whole image
        params.searchRect = Rect(0, 0, params.sceneImageCurrent.cols, params.sceneImageCurrent.rows);
        debugInfo.searchRect = Rect(0, 0, params.sceneImageCurrent.cols, params.sceneImageCurrent.rows);
        debugInfo.probabilityMap = Mat(params.sceneImageCurrent.rows, params.sceneImageCurrent.cols, CV_8UC1, Scalar(0));
        return true;
    }
    
    // declaration & initialization
    float hranges[] = {0,180};
    const float* phranges = hranges;
    int vmin = 5, vmax = 256, smin = 60;
    Mat image, hsv, hue, colorMask, backproj;
    params.sceneImageCurrent.copyTo(image);
    Profiler* profiler = Profiler::Instance();
    Rect searchRect = params.searchRect;
    
    // validate search rect (do this here for initialization)
    profiler->startTimer(TIMER_VALIDATE);
    if (!isRectValid(searchRect, image.cols, image.rows)) {
        Point2f center = Point2f(image.cols * 0.5f, image.rows * 0.5f);
        int width = MIN(image.cols * _minSearchRectSizeRelative * 1.5f, image.cols * _maxSearchRectSizeRelative);
        int height = MIN(image.rows * _minSearchRectSizeRelative * 1.5f, image.rows * _maxSearchRectSizeRelative);
        searchRect = Rect(center.x, center.y, width, height);
    }
    profiler->stopTimer(TIMER_VALIDATE);
    
    // convert to hue saturation value
    profiler->startTimer(TIMER_CONVERT);
    ColorConvert::bgrOrBgra2Hsv(image, hsv);
    inRange(hsv, Scalar(0, smin, MIN(vmin,vmax)), Scalar(180, 256, MAX(vmin, vmax)), colorMask);
    int ch[] = {0, 0};
    hue.create(hsv.size(), hsv.depth());
    mixChannels(&hsv, 1, &hue, 1, ch, 1);
    
    profiler->stopTimer(TIMER_CONVERT);

    // calc back projection and apply cam shift algorithm
    profiler->startTimer(TIMER_TRACK);
    calcBackProject(&hue, 1, 0, _objectHist, backproj, &phranges);
    backproj &= colorMask;
    searchRect = CamShift(backproj, searchRect, _terminationCriteria).boundingRect();
    profiler->stopTimer(TIMER_TRACK);
    
    // validate new search rect 
    profiler->startTimer(TIMER_VALIDATE);
    PointOps::cropRect(searchRect, Rect(0, 0, image.cols, image.rows));
    bool rectIsValid = isRectValid(searchRect, image.cols, image.rows);
    profiler->stopTimer(TIMER_VALIDATE);
    
    // set output params
    params.searchRect = searchRect;
    
    // set debug info values
    debugInfo.searchRect = searchRect;
    debugInfo.searchRectValid = rectIsValid;
    debugInfo.probabilityMap = backproj;
    
    return rectIsValid;
}

bool DetectionModule::isRectValid(const Rect& rect, int imageWidth, int imageHeight) {
    return true
    && rect.width >= imageWidth * _minSearchRectSizeRelative
    && rect.width <= imageWidth * _maxSearchRectSizeRelative
    && rect.height >= imageHeight * _minSearchRectSizeRelative
    && rect.height <= imageHeight * _maxSearchRectSizeRelative;
}
    
} // end of namespaces
    
// preprocess, erode, delate, blur, reduce colors

// moments, histogram comparison, backprojection, mser, blob, chamfer

// angucken: http://docs.opencv.org/modules/imgproc/doc/miscellaneous_transformations.html
// angucken: http://docs.opencv.org/modules/imgproc/doc/structural_analysis_and_shape_descriptors.html