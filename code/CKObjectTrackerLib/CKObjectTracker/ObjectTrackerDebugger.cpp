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

inline static bool compareTimers(pair<string, double> i, pair<string, double> j)
{
    return (i.second > j.second);
}
    
string ObjectTrackerDebugger::debugString(TrackerDebugInfoStripped info)
{
    int length = 1028;
    char buffer[length];
    
    snprintf(buffer, length, "%s took %.2f ms --> %.2f FPS\n", info.currentModuleType.c_str(), info.totalProcessingTime, 1000 / info.totalProcessingTime);
    
    // general info
    double sumSubTimers = 0;
    sort(info.subTaskProcessingTimes.begin(), info.subTaskProcessingTimes.end(), compareTimers);
    for (int i = 0; i < info.subTaskProcessingTimes.size(); i++) {
        std::pair<std::string, double> item = info.subTaskProcessingTimes[i];
        snprintf(buffer, length, "%s%s:\t%f ms (%.2f%%)\n", buffer, item.first.c_str(), item.second, item.second / info.totalProcessingTime * 100);
        sumSubTimers += item.second;
    }
    double timeOther = info.totalProcessingTime - sumSubTimers;
    snprintf(buffer, length, "%sother:\t\t%f ms (%.2f%%)\n", buffer, timeOther, timeOther / info.totalProcessingTime * 100);
    
    // module specific info
    if (info.currentModuleType == ModuleType2String::convert(MODULE_TYPE_DETECTION)) {
        snprintf(buffer, length, "%sobjectContours: %d\n", buffer, info.objectContourCount);
        snprintf(buffer, length, "%ssceneCountours: %d\n", buffer, info.sceneContourCount);
    } else if (info.currentModuleType == ModuleType2String::convert(MODULE_TYPE_VALIDATION)) {
        snprintf(buffer, length, "%sobjectKeyPoints: %d\n", buffer, info.objectKeyPointCount);
        snprintf(buffer, length, "%ssceneKeyPoints: %d\n", buffer, info.sceneKeyPointCount);
        for (int i = 0; i < info.namedMatchCounts.size(); i++) {
            snprintf(buffer, length, "%smatches %s: %d\n", buffer, info.namedMatchCounts[i].first.c_str(), info.namedMatchCounts[i].second);
        }

    } else if (info.currentModuleType == ModuleType2String::convert(MODULE_TYPE_TRACKING)) {
        snprintf(buffer, length, "%sinitial point set: %d\n", buffer, info.initialPointCount);
        for (int i = 0; i < info.namedPointCounts.size(); i++) {
            if (i == 0) {
                snprintf(buffer, length, "%scurrent point set: %d\n", buffer, info.namedPointCounts[i].second);
            } else {
                snprintf(buffer, length, "%slost after %s: %d\n", buffer, info.namedPointCounts[i].first.c_str(), info.namedPointCounts[i - 1].second - info.namedPointCounts[i].second);
            }
        }
        snprintf(buffer, length, "%sdistance range: %.3f\n", buffer, info.movementVariation);
        snprintf(buffer, length, "%saverage error: %.3f\n", buffer, info.avgError);
        snprintf(buffer, length, "%sjitterAmount: %.3f\n", buffer, info.jitterAmount);
    }

    
    return string(buffer);
}


string ObjectTrackerDebugger::debugString(vector<TrackerDebugInfoStripped> info)
{
    int badHomographyCount = 0;
    int frameCountDetection = 0, frameCountValidation = 0, frameCountTracking = 0;
    TrackerDebugInfoStripped averageDetection, averageValidation, averageTracking;
    averageDetection.currentModuleType = ModuleType2String::convert(MODULE_TYPE_DETECTION);
    averageValidation.currentModuleType = ModuleType2String::convert(MODULE_TYPE_VALIDATION);
    averageTracking.currentModuleType = ModuleType2String::convert(MODULE_TYPE_TRACKING);
    
    for (int i = 0; i < info.size(); i++) {
        TrackerDebugInfoStripped frame = info[i];
        if (frame.currentModuleType == ModuleType2String::convert(MODULE_TYPE_DETECTION)) {
            averageDetection += frame;
            frameCountDetection++;
            
        } else if (frame.currentModuleType == ModuleType2String::convert(MODULE_TYPE_VALIDATION)) {
            averageValidation += frame;
            frameCountValidation++;
            if (frame.badHomography) {
                badHomographyCount++;
            }
        } else if (frame.currentModuleType == ModuleType2String::convert(MODULE_TYPE_TRACKING)) {
            averageTracking += frame;
            frameCountTracking++;
            if (frame.badHomography) {
                badHomographyCount++;
            }
        }
    }
    
    int length = 2048;
    char buffer[length];
    
    snprintf(buffer, length, "%s\nProcessed %ld frames (%d detection, %d validation, %d tracking)\n\n",
             "#### #### #### #### Summary #### #### #### ####", info.size(),
             frameCountDetection, frameCountValidation, frameCountTracking);
    snprintf(buffer, length, "%s%s(averaged)\n\n%s(averaged)\n\n%s(averaged)\n\n%d garbage homographies", buffer,
             debugString(averageDetection / frameCountDetection).c_str(),
             debugString(averageValidation / frameCountValidation).c_str(),
             debugString(averageTracking / frameCountTracking).c_str(),
             badHomographyCount);
    
    return buffer;
}

// -------------------- drawing --------------------

static void drawPoints(Mat& result, const vector<Point2f>& points, Scalar color, Point2f offset, float scale)
{
    vector<Point2f>::const_iterator iter;
    for (iter = points.begin(); iter != points.end(); iter++) {
        Point2f p = (*iter) * scale + offset;
        circle(result, p, 2, color);
    }
}
    
static void drawKeyPoints(Mat& result, const vector<KeyPoint>& keyPoints, Scalar color, Point2f offset, float scale)
{
    vector<KeyPoint>::const_iterator iter;
    for (iter = keyPoints.begin(); iter != keyPoints.end(); iter++) {
        Point2f p = (*iter).pt * scale + offset;
        circle(result, p, 2, color);
    }
}
    
static void drawMatches(Mat& result, const vector<DMatch>& matches, const vector<KeyPoint>& keyPoints1, const vector<KeyPoint>& keyPoints2, Scalar color, Point2f offset1, Point2f offset2, float scale1, float scale2)
{
    for (vector<DMatch>::const_iterator iter = matches.begin(); iter != matches.end(); iter++) {
        Point2f p1 = keyPoints1[(*iter).queryIdx].pt * scale1 + offset1;
        Point2f p2 = keyPoints2[(*iter).trainIdx].pt * scale2 + offset2;
        circle(result, p1, 3, color);
        circle(result, p2, 3, color);
        line(result, p1, p2, color);
    }
}
    
static void drawContourImages(Mat& img1, Mat& img2, const vector<CMatch>& matches, const vector<vector<Point> >& contours1, const vector<vector<Point> >& contours2)
{
    for (vector<CMatch>::const_iterator iter = matches.begin(); iter != matches.end(); iter++) {
        Scalar color = Scalar(rand()&255, rand()&255, rand()&255);
        drawContours(img1, contours1, (*iter).queryIdx, color, 1);
        drawContours(img2, contours2, (*iter).trainIdx, color, 1);
    }
//    for (int i = 0; i < contours1.size(); i++) {
//        Scalar color = Scalar(rand()&255, rand()&255, rand()&255);
//        drawContours(img1, contours1, i, color, 1);
//    }
//    for (int i = 0; i < contours2.size(); i++) {
//        Scalar color = Scalar(rand()&255, rand()&255, rand()&255);
//        drawContours(img2, contours2, i, color, 1);
//    }

}
    
static void drawTransformedRectToImage(Mat& image, const vector<Point2f>& corners, Point2f offset, float scale, Scalar color, int lineWidth)
{
    if (corners.size() != 4)
        return;
    
    line(image, corners[0] * scale + offset, corners[1] * scale + offset, color, lineWidth);
    line(image, corners[1] * scale + offset, corners[2] * scale + offset, color, lineWidth);
    line(image, corners[2] * scale + offset, corners[3] * scale + offset, color, lineWidth);
    line(image, corners[3] * scale + offset, corners[0] * scale + offset, color, lineWidth);
}

static Mat drawDetectionImage(TrackerDebugInfo info) {
    Mat result = Mat(info.sceneImageFull.cols, info.sceneImageFull.rows, CV_8UC3, Scalar(0, 0, 0));
    Mat imgObject = Mat(info.objectImage.rows, info.objectImage.cols, CV_8UC3, Scalar(0, 0, 0));
    Mat imgScene = Mat(info.sceneImageFull.rows, info.sceneImageFull.cols, CV_8UC3, Scalar(0, 0, 0));
    if (imgObject.type() == CV_8UC1) { cvtColor(imgObject, imgObject, CV_GRAY2BGR); }
    if (imgScene.type() == CV_8UC1) { cvtColor(imgScene, imgScene, CV_GRAY2BGR); }
    
    drawContourImages(imgObject, imgScene, info.contourMatches, info.objectContours, info.sceneContours);
    
    float scaleObj = MIN(result.cols / (float)imgObject.cols, result.rows * 0.5f / (float)imgObject.rows);
    resize(imgObject, imgObject, Size(imgObject.cols * scaleObj, imgObject.rows * scaleObj));
    Point2f offsetObj = Point2f((result.cols - imgObject.cols) * 0.5f, 0);
    imgObject.copyTo(result(Rect(offsetObj.x, offsetObj.y, imgObject.cols, imgObject.rows)));
    
    float scaleScn = MIN(result.cols / (float)imgScene.cols, result.rows * 0.5f / (float)imgScene.rows);
    resize(imgScene, imgScene, Size(imgScene.cols * scaleScn, imgScene.rows * scaleScn));
    Point2f offsetScn = Point2f((result.cols - imgScene.cols) * 0.5f, result.rows * 0.5f);
    imgScene.copyTo(result(Rect(offsetScn.x, offsetScn.y, imgScene.cols, imgScene.rows)));
    
    // rotate image
    flip(result, result, 1);
    transpose(result, result);
    return result;
}
    
static Mat drawValidationImage(TrackerDebugInfo info, bool drawTransformedRect, bool drawFilteredMatches, bool drawAllMatches, bool drawObjectKeyPoints, bool drawSceneKeyPoints)
{
    Mat result;
    if (drawFilteredMatches || drawAllMatches) {
        result = Mat(info.sceneImageFull.cols, info.sceneImageFull.rows, CV_8UC3, Scalar(0, 0, 0));
        
        Mat imgScene = info.sceneImagePart;
        Mat imgObject = info.objectImage;
        if (imgScene.type() == CV_8UC1) { cvtColor(imgScene, imgScene, CV_GRAY2BGR); }
        if (imgObject.type() == CV_8UC1) { cvtColor(imgObject, imgObject, CV_GRAY2BGR); }        
        
        float scaleObj = MIN(result.cols / (float)imgObject.cols, result.rows * 0.5f / (float)imgObject.rows);
        resize(imgObject, imgObject, Size(imgObject.cols * scaleObj, imgObject.rows * scaleObj));
        Point2f offsetObj = Point2f((result.cols - imgObject.cols) * 0.5f, 0);
        imgObject.copyTo(result(Rect(offsetObj.x, offsetObj.y, imgObject.cols, imgObject.rows)));
        
        float scaleScn = MIN(result.cols / (float)imgScene.cols, result.rows * 0.5f / (float)imgScene.rows);
        resize(imgScene, imgScene, Size(imgScene.cols * scaleScn, imgScene.rows * scaleScn));
        Point2f offsetScn = Point2f((result.cols - imgScene.cols) * 0.5f, result.rows * 0.5f);
        imgScene.copyTo(result(Rect(offsetScn.x, offsetScn.y, imgScene.cols, imgScene.rows)));
        
        if (!drawAllMatches && drawFilteredMatches && info.namedMatches.size() > 0) {
            drawMatches(result, info.namedMatches[info.namedMatches.size() - 1].second, info.objectKeyPoints, info.sceneKeyPoints, Scalar(0, 255, 255), offsetObj, offsetScn, scaleObj, scaleScn);
        }
        
        if (drawObjectKeyPoints) {
            drawKeyPoints(result, info.objectKeyPoints, Scalar(255, 0, 0), offsetObj, scaleObj);
        }
        if (drawSceneKeyPoints) {
            drawKeyPoints(result, info.sceneKeyPoints, Scalar(255, 0, 0), offsetScn, scaleScn);
        }
        
        if (drawAllMatches) {
            int size = (int)info.namedMatches.size();
            for (int i = 0; i < size; i++) {
                if (i == 0 && size > 1)
                    continue;
                int brightness = (int)(255 * (i + 1) / size);
                drawMatches(result, info.namedMatches[i].second, info.objectKeyPoints, info.sceneKeyPoints, Scalar(0, brightness, brightness), offsetObj, offsetScn, scaleObj, scaleScn);
            }
        }

        if (drawTransformedRect) {
            Point2f pointOffset = Point2f(-info.searchRect.x, -info.searchRect.y) * scaleScn + offsetScn;
            Scalar color = info.badHomography ? Scalar(0, 0, 255) : Scalar(0, 255, 0);
            drawTransformedRectToImage(result, info.objectCornersTransformed, pointOffset, scaleScn, color, 2);
        }
        
        // rotate image
        flip(result, result, 1);
        transpose(result, result);
    } else {
        result = info.sceneImageFull;
        if (drawTransformedRect) {
            Point2f offset = Point2f(-info.searchRect.x, -info.searchRect.y);
            Scalar color = info.badHomography ? Scalar(0, 0, 255) : Scalar(0, 255, 0);
            drawTransformedRectToImage(result, info.objectCornersTransformed, offset, 1, color, 2);
        }
    }
    return result;
}

static Mat drawTrackingImage(TrackerDebugInfo info, bool drawTransformedRect, bool drawfilteredPoints, bool drawAllPoints)
{
    Mat result = info.sceneImageFull;
    Point2f offset = Point(0,0);//info.searchRect.tl();
    float scale = 1.0f;
    if (!drawAllPoints && drawfilteredPoints && info.namedPointSets.size() > 0) {
        drawPoints(result, info.namedPointSets[info.namedPointSets.size() - 1].second, Scalar(0, 255, 255), offset, scale);
    }
    
    if (drawAllPoints) {
        int size = (int)info.namedPointSets.size();
        for (int i = 0; i < size; i++) {
            int brightness = (int)(255 * (i + 1) / size);
            drawPoints(result, info.namedPointSets[i].second, Scalar(0, brightness, brightness), offset, scale);
        }
    }
    
    rectangle(result, info.searchRect.tl(), info.searchRect.br(), Scalar(0,0,255), 3);
    
    if (drawTransformedRect) {
        Scalar color = info.badHomography ? Scalar(0, 0, 255) : Scalar(0, 255, 0);
        drawTransformedRectToImage(result, info.objectCornersTransformed, offset, 1, color, 2);
    }
    return result;
}
    
vector<pair<string, Mat> > ObjectTrackerDebugger::debugImages(TrackerDebugInfo info, bool drawTransformedRect, bool drawFilteredMatches, bool drawAllMatches, bool drawObjectKeyPoints, bool drawSceneKeyPoints)
{
    vector<pair<string, Mat> > debugImages = vector<pair<string, Mat> >();

    if (info.currentModuleType == ModuleType2String::convert(MODULE_TYPE_DETECTION)) {
        debugImages.push_back(make_pair("detection", info.probabilityMap));//drawDetectionImage(info)));
    } else if (info.currentModuleType == ModuleType2String::convert(MODULE_TYPE_VALIDATION)) {
        debugImages.push_back(make_pair("validation", drawValidationImage(info, drawTransformedRect, drawFilteredMatches, drawAllMatches, drawObjectKeyPoints, drawSceneKeyPoints)));
    } else if (info.currentModuleType == ModuleType2String::convert(MODULE_TYPE_TRACKING)) {
        debugImages.push_back(make_pair("tracking", drawTrackingImage(info, drawTransformedRect, drawFilteredMatches, drawAllMatches)));
    } else if (info.currentModuleType == ModuleType2String::convert(MODULE_TYPE_EMPTY)) {
        debugImages.push_back(make_pair("empty", info.sceneImageFull));
    }

    return debugImages;
}

} // end of namespace
