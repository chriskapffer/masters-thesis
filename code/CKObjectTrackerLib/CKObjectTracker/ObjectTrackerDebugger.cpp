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
    int length = 1024;
    char buffer[length];
    
    snprintf(buffer, length, "%s took %.2f ms --> %.2f FPS\n", info.currentModuleType.c_str(), info.totalProcessingTime, 1000 / info.totalProcessingTime);
    
    if (info.currentModuleType == ModuleType2String::convert(MODULE_TYPE_VALIDATION)) {
        snprintf(buffer, length, "%sObjectKeyPoints: %d\n", buffer, info.objectKeyPointCount);
        snprintf(buffer, length, "%sSceneKeyPoints: %d\n", buffer, info.sceneKeyPointCount);
        for (int i = 0; i < info.namedMatchCounts.size(); i++) {
            snprintf(buffer, length, "%smatches %s: %d\n", buffer, info.namedMatchCounts[i].first.c_str(), info.namedMatchCounts[i].second);
        }
        double sumSubTimers = 0;
        for (int i = 0; i < info.subTaskProcessingTimes.size(); i++) {
            std::pair<std::string, double> item = info.subTaskProcessingTimes[i];
            snprintf(buffer, length, "%s%s:\t%f ms (%.2f%%)\n", buffer, item.first.c_str(), item.second, item.second / info.totalProcessingTime * 100);
            sumSubTimers += item.second;
        }
        double timeOther = info.totalProcessingTime - sumSubTimers;
        snprintf(buffer, length, "%sother:\t\t%f ms (%.2f%%)\n", buffer, timeOther, timeOther / info.totalProcessingTime * 100);
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
        }
    }
    
    int length = 1024;
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
    
static void drawTransformedRectToImage(Mat& image, const vector<Point2f>& corners, Point2f offset, float scale, Scalar color, int lineWidth)
{
    if (corners.size() != 4)
        return;
    
    line(image, corners[0] * scale + offset, corners[1] * scale + offset, color, lineWidth);
    line(image, corners[1] * scale + offset, corners[2] * scale + offset, color, lineWidth);
    line(image, corners[2] * scale + offset, corners[3] * scale + offset, color, lineWidth);
    line(image, corners[3] * scale + offset, corners[0] * scale + offset, color, lineWidth);
}
    
static Mat drawValidationImage(TrackerDebugInfo info, bool drawTransformedRect, bool drawFilteredMatches, bool drawAllMatches)
{
    Mat result;
    if (drawFilteredMatches || drawAllMatches) {
        result = Mat(info.sceneImageFull.cols, info.sceneImageFull.rows, CV_8UC3, Scalar(0, 0, 0));
        
        Mat imgScene = info.sceneImagePart;
        Mat imgObject = info.objectImage;
        
        float factorObj = MIN(result.cols / (float)imgObject.cols, result.rows * 0.5f / (float)imgObject.rows);
        resize(imgObject, imgObject, Size(imgObject.cols * factorObj, imgObject.rows * factorObj));
        Point2f offsetObj = Point2f((result.cols - imgObject.cols) * 0.5f, 0);
        imgObject.copyTo(result(Rect(offsetObj.x, offsetObj.y, imgObject.cols, imgObject.rows)));
        
        float factorScn = MIN(result.cols / (float)imgScene.cols, result.rows * 0.5f / (float)imgScene.rows);
        resize(imgScene, imgScene, Size(imgScene.cols * factorScn, imgScene.rows * factorScn));
        Point2f offsetScn = Point2f((result.cols - imgScene.cols) * 0.5f, result.rows * 0.5f);
        imgScene.copyTo(result(Rect(offsetScn.x, offsetScn.y, imgScene.cols, imgScene.rows)));
        
        if (!drawAllMatches && drawFilteredMatches && info.namedMatches.size() > 0) {
            drawMatches(result, info.namedMatches[info.namedMatches.size() - 1].second, info.objectKeyPoints, info.sceneKeyPoints, Scalar(0, 255, 255), offsetObj, offsetScn, factorObj, factorScn);
        }
        
        if (drawAllMatches) {
            int size = (int)info.namedMatches.size();
            for (int i = 0; i < size; i++) {
                if (i == 0 && size > 1)
                    continue;
                int brightness = (int)(255 * (i + 1) / size);
                drawMatches(result, info.namedMatches[i].second, info.objectKeyPoints, info.sceneKeyPoints, Scalar(0, brightness, brightness), offsetObj, offsetScn, factorObj, factorScn);
            }
        }

        if (drawTransformedRect) {
            Scalar color = info.badHomography ? Scalar(0, 0, 255) : Scalar(0, 255, 0);
            drawTransformedRectToImage(result, info.transformedObjectCorners, offsetScn, factorScn, color, 2);
        }
        
        // rotate image
        flip(result, result, 1);
        transpose(result, result);
    } else {
        result = info.sceneImageFull;
        if (drawTransformedRect) {
            Point2f offset = Point2f(info.searchRect.x, info.searchRect.y);
            Scalar color = info.badHomography ? Scalar(0, 0, 255) : Scalar(0, 255, 0);
            drawTransformedRectToImage(result, info.transformedObjectCorners, offset, 1, color, 2);
        }
    }
    return result;
}
    
vector<pair<string, Mat> > ObjectTrackerDebugger::debugImages(TrackerDebugInfo info, bool drawTransformedRect, bool drawFilteredMatches, bool drawAllMatches)
{
    vector<pair<string, Mat> > debugImages = vector<pair<string, Mat> >();
    Mat blackImage = Mat(info.sceneImageFull.rows, info.sceneImageFull.cols, CV_8UC1, Scalar(0));
    
    if (info.currentModuleType == ModuleType2String::convert(MODULE_TYPE_DETECTION)) {
        debugImages.push_back(make_pair("detection", blackImage));
        debugImages.push_back(make_pair("validation", blackImage));
        debugImages.push_back(make_pair("tracking", blackImage));
    } else if (info.currentModuleType == ModuleType2String::convert(MODULE_TYPE_VALIDATION)) {
        debugImages.push_back(make_pair("detection", blackImage));
        debugImages.push_back(make_pair("validation", drawValidationImage(info, drawTransformedRect, drawFilteredMatches, drawAllMatches)));
        debugImages.push_back(make_pair("tracking", blackImage));
    } else if (info.currentModuleType == ModuleType2String::convert(MODULE_TYPE_TRACKING)) {
        debugImages.push_back(make_pair("detection", blackImage));
        debugImages.push_back(make_pair("validation", blackImage));
        debugImages.push_back(make_pair("tracking", blackImage));
    }
    
    return debugImages;
}

} // end of namespace
