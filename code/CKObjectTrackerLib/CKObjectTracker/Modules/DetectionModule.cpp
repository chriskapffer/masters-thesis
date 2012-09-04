//
//  DetectionModule.cpp
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 22.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#include "DetectionModule.h"
#include "Profiler.h"
#include "Utils.h"

#define TIMER_PREPROC "preprocIng"
#define TIMER_CONVERT "converting"
#define TIMER_EXTRACT "extraction"
#define TIMER_MATCH "matching"

using namespace std;
using namespace cv;

namespace ck {
    
DetectionModule::DetectionModule() : AbstractModule(MODULE_TYPE_DETECTION)
{
    _detectionThreshold = 0.7f;
    _preProcess = true;
    _byPass = false;
}

DetectionModule::~DetectionModule()
{

}
    
void DetectionModule::initEdges(const cv::Mat &objectImage)
{
    Profiler* profiler = Profiler::Instance();
    
    profiler->startTimer(TIMER_CONVERT);
    Mat gray;
    utils::bgrOrBgra2Gray(objectImage, gray);
    _objectImage = gray < 128;
    profiler->stopTimer(TIMER_CONVERT);
    
    if (_preProcess) {
        profiler->startTimer(TIMER_PREPROC);
        blur(_objectImage, _objectImage, Size(5,5));
        profiler->stopTimer(TIMER_PREPROC);
    }
    
    profiler->startTimer(TIMER_EXTRACT);
    vector<vector<Point> > tmp;
    findContours(_objectImage, tmp, _objectHierarchy, CV_RETR_CCOMP, CHAIN_APPROX_SIMPLE);
    for(int i = 0; i >= 0; i = _objectHierarchy[i][0])
    {
        _objectContours.push_back(tmp[i]);
    }
    profiler->stopTimer(TIMER_EXTRACT);
}
    
void DetectionModule::initHist(const cv::Mat &objectImage)
{
    Mat hsv, hue, mask, hist;
    int vmin = 5, vmax = 256, smin = 60;
    int hsize = 16;
    float hranges[] = {0,180};
    const float* phranges = hranges;
    
    cvtColor(objectImage, hsv, CV_BGR2HSV);
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
    namedWindow("histimg");
    imshow("histimg", histimg);
}

void DetectionModule::initWithObjectImage(const cv::Mat &objectImage)
{
    if (false) {
        initEdges(objectImage);
    } else {
        initHist(objectImage);
    }
}
    
bool DetectionModule::matchEdges(ModuleParams& params, TrackerDebugInfo& debugInfo) {
    
    Mat sceneImage;
    vector<Vec4i> sceneHierarchy;
    vector<vector<Point> > sceneContours;
    vector<CMatch> matches;
    
    Profiler* profiler = Profiler::Instance();
    
    profiler->startTimer(TIMER_CONVERT);
    Mat gray;
    utils::bgrOrBgra2Gray(params.sceneImageCurrent, gray);
    sceneImage = gray < 128;
    profiler->stopTimer(TIMER_CONVERT);
    
    // blur, etc.
    if (_preProcess) {
        profiler->startTimer(TIMER_PREPROC);
        blur(sceneImage, sceneImage, Size(5,5));
        profiler->stopTimer(TIMER_PREPROC);
    }

    // find contours
    profiler->startTimer(TIMER_EXTRACT);
    vector<vector<Point> > tmp;
    findContours(sceneImage, tmp, sceneHierarchy, CV_RETR_CCOMP, CHAIN_APPROX_SIMPLE);
    for(int i = 0; i >= 0; i = sceneHierarchy[i][0])
    {
        sceneContours.push_back(tmp[i]);
    }
    profiler->stopTimer(TIMER_EXTRACT);
    
    // match contours
    profiler->startTimer(TIMER_MATCH);
//    for (int i = 0; i >= 0; i = _objectHierarchy[i][0]) {
//        float maxScore = 0;
//        int bestMatchIdx = 0;
//        for (int j = 0; j >= 0; j = sceneHierarchy[j][0]) {
//            float score = matchShapes(_objectContours[i], sceneContours[j], CV_CONTOURS_MATCH_I2, 0);
//            if (score > maxScore) {
//                maxScore = score;
//                bestMatchIdx = j;
//            }
//        }
//        matches.push_back(CMatch(i, bestMatchIdx, maxScore));
//    }
    for (int i = 0; i < _objectContours.size(); i++) {
        float maxScore = 0;
        int bestMatchIdx = 0;
        for (int j = 0; j < sceneContours.size(); j++) {
            float score = matchShapes(_objectContours[i], sceneContours[j], CV_CONTOURS_MATCH_I1, 0);
            if (score > maxScore) {
                maxScore = score;
                bestMatchIdx = j;
            }
        }
        matches.push_back(CMatch(i, bestMatchIdx, maxScore));
    }
    profiler->stopTimer(TIMER_MATCH);
    
    debugInfo.objectImage = _objectImage;
    debugInfo.objectContours = _objectContours;
    debugInfo.sceneContours = sceneContours;
    debugInfo.contourMatches = matches;
    
    // TODO create probability map and extract regions
    params.searchRect = Rect(0, 0, params.sceneImageCurrent.cols, params.sceneImageCurrent.rows);
    debugInfo.searchRect = params.searchRect;
    return true;
}
    
bool DetectionModule::matchHist(ModuleParams& params, TrackerDebugInfo& debugInfo) {
    
    Profiler* profiler = Profiler::Instance();

    float hranges[] = {0,180};
    const float* phranges = hranges;
    int vmin = 5, vmax = 256, smin = 60;
    Mat image, hsv, hue, mask, backproj;
    
    params.sceneImageCurrent.copyTo(image);

    cvtColor(image, hsv, CV_BGR2HSV);
    inRange(hsv, Scalar(0, smin, MIN(vmin,vmax)), Scalar(180, 256, MAX(vmin, vmax)), mask);
    
    int ch[] = {0, 0};
    hue.create(hsv.size(), hsv.depth());
    mixChannels(&hsv, 1, &hue, 1, ch, 1);
    
    Rect trackWindow = params.searchRect;
    if( trackWindow.area() <= 1 )
    {
        int cols = image.cols, rows = image.rows, r = (MIN(cols, rows) + 5)/6;
        trackWindow = Rect((cols-r) / 2, (rows-r) / 2, r, r);
    }
    
    calcBackProject(&hue, 1, 0, _objectHist, backproj, &phranges);
    backproj &= mask;
    
    RotatedRect trackBox = CamShift(backproj, trackWindow, TermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 20, 0.1f ));
    Rect boundingBox = trackBox.boundingRect();
    boundingBox.x = MAX(boundingBox.x, 0);
    boundingBox.y = MAX(boundingBox.y, 0);
    boundingBox.width = MIN(boundingBox.width, backproj.cols - boundingBox.x);
    boundingBox.height = MIN(boundingBox.height, backproj.rows - boundingBox.y);
    cvtColor(backproj, backproj, CV_GRAY2BGR);
    rectangle(backproj, boundingBox.tl(), boundingBox.br(), Scalar(0,0,255), 3);

    params.searchRect = boundingBox;
    debugInfo.searchRect = params.searchRect;
    debugInfo.probabilityMap = backproj;
    return boundingBox.area() > 10 && boundingBox.area() < backproj.cols * backproj.rows * 0.8f;
}
    
bool DetectionModule::internalProcess(ModuleParams& params, TrackerDebugInfo& debugInfo)
{
    if (_byPass) {
        // search in whole image
        params.searchRect = Rect(0, 0, params.sceneImageCurrent.cols, params.sceneImageCurrent.rows);
        debugInfo.probabilityMap = Mat(params.sceneImageCurrent.rows, params.sceneImageCurrent.cols, CV_8UC1, Scalar(0));
        return true;
    }
    
    if (false)
        return matchEdges(params, debugInfo);
    else
        return matchHist(params, debugInfo);
}

} // end of namespaces
    
// preprocess, erode, delate, blur, reduce colors

// moments, histogram comparison, backprojection, mser, blob, chamfer

// angucken: http://docs.opencv.org/modules/imgproc/doc/miscellaneous_transformations.html
// angucken: http://docs.opencv.org/modules/imgproc/doc/structural_analysis_and_shape_descriptors.html