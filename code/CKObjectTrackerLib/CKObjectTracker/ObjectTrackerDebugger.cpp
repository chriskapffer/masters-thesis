//
//  ObjectTrackerDebugger.cpp
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 24.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#include "ObjectTrackerDebugger.h"
#include "ModuleManagement.h"

using namespace std;
using namespace cv;

namespace ck {

inline static bool compareTimers(pair<string, double> i, pair<string, double> j)
{
    return (i.second > j.second);
}

void ObjectTrackerDebugger::decomposeDebugInfoVector(const std::vector<TrackerDebugInfoStripped>& info, TrackerDebugInfoStripped& sumDetectionInfos, TrackerDebugInfoStripped& sumValidationInfos, TrackerDebugInfoStripped& sumTrackingInfos, int& frameCountDetection, int& frameCountValidation, int& frameCountTracking, int& badHomographyCount)
{
    for (int i = 0; i < info.size(); i++) {
        TrackerDebugInfoStripped frame = info[i];
        if (frame.currentModuleType == ModuleType2String::convert(MODULE_TYPE_DETECTION)) {
            sumDetectionInfos += frame;
            frameCountDetection++;
            
        } else if (frame.currentModuleType == ModuleType2String::convert(MODULE_TYPE_VALIDATION)) {
            sumValidationInfos += frame;
            frameCountValidation++;
            if (frame.badHomography) {
                badHomographyCount++;
            }
        } else if (frame.currentModuleType == ModuleType2String::convert(MODULE_TYPE_TRACKING)) {
            sumTrackingInfos += frame;
            frameCountTracking++;
            if (frame.badHomography) {
                badHomographyCount++;
            }
        }
    }
}

std::string ObjectTrackerDebugger::getProcessingTimersDebugString(std::vector<std::pair<std::string, double> > timers, double totalProcessingTime)
{
    int length = 1028;
    char buffer[length];
    double sumSubTimers = 0;
    snprintf(buffer, length, ""); // init with empty string
    
    // task specific timers (sorted by duration)
    sort(timers.begin(), timers.end(), compareTimers);
    vector<pair<string, double> >::const_iterator iter;
    for (iter = timers.begin(); iter != timers.end(); iter++) {
        snprintf(buffer, length, "%s%s:\t%f ms (%.2f%%)\n", buffer,
                 (*iter).first.c_str(), (*iter).second,
                 (*iter).second / totalProcessingTime * 100);
        sumSubTimers += (*iter).second;
    }
    // time which was not used for a specific task
    double timeOther = totalProcessingTime - sumSubTimers;
    snprintf(buffer, length, "%sother:\t\t%f ms (%.2f%%)\n", buffer,
             timeOther, timeOther / totalProcessingTime * 100);
    
    return string(buffer);
}
    
std::string ObjectTrackerDebugger::getDetectionModuleDebugString(const TrackerDebugInfoStripped& info)
{
    assert(info.currentModuleType == ModuleType2String::convert(MODULE_TYPE_DETECTION));
    int length = 1028;
    char buffer[length];
    snprintf(buffer, length, ""); // init with empty string
    return string(buffer);
}

std::string ObjectTrackerDebugger::getValidationModuleDebugString(const TrackerDebugInfoStripped& info)
{
    assert(info.currentModuleType == ModuleType2String::convert(MODULE_TYPE_VALIDATION));
    int length = 1028;
    char buffer[length];
    
    snprintf(buffer, length, ""); // init with empty string
    snprintf(buffer, length, "%sobjectKeyPoints: %d\n", buffer, info.objectKeyPointCount);
    snprintf(buffer, length, "%ssceneKeyPoints: %d\n", buffer, info.sceneKeyPointCount);
    for (int i = 0; i < info.namedMatchCounts.size(); i++) {
        snprintf(buffer, length, "%smatches %s: %d\n", buffer,
                 info.namedMatchCounts[i].first.c_str(), (int)info.namedMatchCounts[i].second);
    }
    
    snprintf(buffer, length, "%sdelta transform (px): %.3f\n", buffer, info.transformationDelta);
    return string(buffer);
}

std::string ObjectTrackerDebugger::getTrackingModuleDebugString(const TrackerDebugInfoStripped& info)
{
    assert(info.currentModuleType == ModuleType2String::convert(MODULE_TYPE_TRACKING));
    int length = 1028;
    char buffer[length];
    
    snprintf(buffer, length, ""); // init with empty string
    snprintf(buffer, length, "%sinitial point set: %d\n", buffer, info.initialPointCount);
    for (int i = 0; i < info.namedPointCounts.size(); i++) {
        if (i == 0) {
            snprintf(buffer, length, "%scurrent point set: %f.3\n", buffer, info.namedPointCounts[i].second);
        } else {
            snprintf(buffer, length, "%slost after %s: %.3f\n", buffer, info.namedPointCounts[i].first.c_str(),
                     info.namedPointCounts[i - 1].second - info.namedPointCounts[i].second);
        }
    }
    
    snprintf(buffer, length, "%sdelta transform (px): %.3f\n", buffer, info.transformationDelta);
    snprintf(buffer, length, "%sdistance range (px): %.3f\n", buffer, info.distortion);
    snprintf(buffer, length, "%saverage error: %.3f\n", buffer, info.avgError);
    return string(buffer);
}

std::string ObjectTrackerDebugger::getDebugString(const TrackerDebugInfoStripped& info)
{
    // compose with general info
    int length = 1028;
    char buffer[length];
    
    // module info header
    snprintf(buffer, length, "%s took %.2f ms --> %.2f FPS\n", info.currentModuleType.c_str(), info.totalProcessingTime, 1000 / info.totalProcessingTime);
    string result = string(buffer);
    
    // append timers and module specific info
    result.append(getProcessingTimersDebugString(info.subTaskProcessingTimes, info.totalProcessingTime));
    if (info.currentModuleType == ModuleType2String::convert(MODULE_TYPE_DETECTION)) {
        result.append(getDetectionModuleDebugString(info));
    } else if (info.currentModuleType == ModuleType2String::convert(MODULE_TYPE_VALIDATION)) {
        result.append(getValidationModuleDebugString(info));
    } else if (info.currentModuleType == ModuleType2String::convert(MODULE_TYPE_TRACKING)) {
        result.append(getTrackingModuleDebugString(info));
    }

    return result;
}
    
std::string ObjectTrackerDebugger::getDebugString(const std::vector<TrackerDebugInfoStripped>& info)
{
    int badHomographyCount = 0;
    int frameCountDetection = 0, frameCountValidation = 0, frameCountTracking = 0;
    // init specific debug info string containers
    TrackerDebugInfoStripped sumDetectionInfos, sumValidationInfos, sumTrackingInfos;
    sumDetectionInfos.currentModuleType = ModuleType2String::convert(MODULE_TYPE_DETECTION);
    sumValidationInfos.currentModuleType = ModuleType2String::convert(MODULE_TYPE_VALIDATION);
    sumTrackingInfos.currentModuleType = ModuleType2String::convert(MODULE_TYPE_TRACKING);
    // fill them with their appropriate instances from debug info vector
    decomposeDebugInfoVector(info, sumDetectionInfos, sumValidationInfos, sumTrackingInfos, frameCountDetection, frameCountValidation, frameCountTracking, badHomographyCount);
    
    int length = 2048;
    char buffer[length];
    // summary header
    snprintf(buffer, length, "%s\nProcessed %ld frames (%d detection, %d validation, %d tracking)\n\n",
             "#### #### #### Summary #### #### ####", info.size(),
             frameCountDetection, frameCountValidation, frameCountTracking);
    // compose everything
    snprintf(buffer, length, "%s%s(averaged)\n\n%s(averaged)\n\n%s(averaged)\n\n%d garbage homographies", buffer,
             getDebugString(sumDetectionInfos / frameCountDetection).c_str(),
             getDebugString(sumValidationInfos / frameCountValidation).c_str(),
             getDebugString(sumTrackingInfos / frameCountTracking).c_str(),
             badHomographyCount);
    
    return string(buffer);
}

#pragma mark

void ObjectTrackerDebugger::drawPoints(cv::Mat& image, const std::vector<cv::Point2f>& points, cv::Scalar color, cv::Point2f offset, float scale)
{
    vector<Point2f>::const_iterator iter;
    for (iter = points.begin(); iter != points.end(); iter++) {
        Point2f p = (*iter) * scale + offset;
        circle(image, p, 2, color);
    }
}

void ObjectTrackerDebugger::drawKeyPoints(cv::Mat& image, const std::vector<cv::KeyPoint>& keyPoints, cv::Scalar color, cv::Point2f offset, float scale)
{
    vector<KeyPoint>::const_iterator iter;
    for (iter = keyPoints.begin(); iter != keyPoints.end(); iter++) {
        Point2f p = (*iter).pt * scale + offset;
        circle(image, p, 2, color);
    }
}

void ObjectTrackerDebugger::drawMatches(cv::Mat& image, const std::vector<cv::DMatch>& matches, const std::vector<cv::KeyPoint>& keyPoints1, const std::vector<cv::KeyPoint>& keyPoints2, cv::Scalar color, cv::Point2f offset1, cv::Point2f offset2, float scale1, float scale2)
{
    for (vector<DMatch>::const_iterator iter = matches.begin(); iter != matches.end(); iter++) {
        Point2f p1 = keyPoints1[(*iter).queryIdx].pt * scale1 + offset1;
        Point2f p2 = keyPoints2[(*iter).trainIdx].pt * scale2 + offset2;
        circle(image, p1, 3, color);
        circle(image, p2, 3, color);
        line(image, p1, p2, color);
    }
}

void ObjectTrackerDebugger::drawRect(cv::Mat& image, const std::vector<cv::Point2f>& corners, cv::Scalar color, cv::Point2f offset, float scale, int lineWidth)
{
    if (corners.size() != 4)
        return;
    
    line(image, corners[0] * scale + offset, corners[1] * scale + offset, color, lineWidth);
    line(image, corners[1] * scale + offset, corners[2] * scale + offset, color, lineWidth);
    line(image, corners[2] * scale + offset, corners[3] * scale + offset, color, lineWidth);
    line(image, corners[3] * scale + offset, corners[0] * scale + offset, color, lineWidth);
}

void ObjectTrackerDebugger::scaleImageToSize(cv::Mat& image, cv::Size targetSize, float& scale, cv::Point2f& offset, bool centerImage)
{
    offset.x = 0; offset.y = 0;
    scale = MIN(targetSize.width / (float)image.cols, targetSize.height / (float)image.rows);
    resize(image, image, Size(image.cols * scale, image.rows * scale));
    if (scale != 1 && centerImage) {
        Mat blackBg = Mat(targetSize.height, targetSize.width, image.type(), Scalar(0));
        offset = Point2f((targetSize.width - image.cols) * 0.5f, (targetSize.height - image.rows) * 0.5f);
        image.copyTo(blackBg(Rect(offset.x, offset.y, image.cols, image.rows)));
        swap(image, blackBg);
    }
}
    
#pragma mark
    
cv::Mat ObjectTrackerDebugger::getHistogramImage(const cv::MatND& hist)
{
    return Mat(); // TODO: implementation
}

bool ObjectTrackerDebugger::getDetectionModuleDebugImage(cv::Mat& image, const TrackerDebugInfo& info, bool drawSearchRect)
{
    if (info.currentModuleType != ModuleType2String::convert(MODULE_TYPE_DETECTION)) { return false; }
    
    info.probabilityMap.copyTo(image);
    if (drawSearchRect) {
        cvtColor(image, image, CV_GRAY2BGR);
        Scalar color = info.searchRectValid ? Scalar(255, 0, 255) : Scalar(0, 0, 255);
        rectangle(image, info.searchRect.tl(), info.searchRect.br(), color, 2);
    }
    return true;
}
    
bool ObjectTrackerDebugger::getValidationModuleDebugImage(cv::Mat& image, const TrackerDebugInfo& info, bool drawObjectRect, bool drawObjectKeyPoints, bool drawSceneKeyPoints, bool drawFilteredMatches, bool drawAllMatches)
{
    if (info.currentModuleType != ModuleType2String::convert(MODULE_TYPE_VALIDATION)
        || (info.objectImage.empty() || info.sceneImagePart.empty())) { return false; }

    Size imageSize = Size(info.sceneImageFull.cols, info.sceneImageFull.rows);
    Scalar matchColor        = Scalar(  0, 255, 255);
    Scalar keyPointColor     = Scalar(255,   0,   0);
    Scalar rectColorPositive = Scalar(  0, 255,   0);
    Scalar rectColorNegative = Scalar(  0,   0, 255);
    Mat objectImage; info.objectImage.copyTo(objectImage);
    Mat sceneImage; info.sceneImagePart.copyTo(sceneImage);
    if (objectImage.type() == CV_8UC1) { cvtColor(objectImage, objectImage, CV_GRAY2BGR); }
    if (sceneImage.type() == CV_8UC1) { cvtColor(sceneImage, sceneImage, CV_GRAY2BGR); }
    
    if (drawObjectKeyPoints) { drawKeyPoints(objectImage, info.objectKeyPoints, keyPointColor); }
    if (drawSceneKeyPoints) { drawKeyPoints(sceneImage, info.sceneKeyPoints, keyPointColor); }
    if (drawObjectRect) {
        Point2f offset = -info.imageOffset;
        Scalar color = info.badHomography ? rectColorNegative : rectColorPositive;
        drawRect(sceneImage, info.objectCornersTransformed, color, offset);
    }
    
    if (drawObjectKeyPoints && !drawSceneKeyPoints && !drawFilteredMatches && !drawAllMatches) {
        Point2f offset; float scale;
        scaleImageToSize(objectImage, imageSize, scale, offset);
        image = objectImage;
        return true;
    } else if (drawSceneKeyPoints && !drawObjectKeyPoints && !drawFilteredMatches && !drawAllMatches) {
        Point2f offset; float scale;
        scaleImageToSize(sceneImage, imageSize, scale, offset);
        image = sceneImage;
        return true;
    }
    
    // both object and scene image have to be drawn, so scale them down to fit into one
    Size imageSizeFlipped = Size(imageSize.height, imageSize.width);
    int objImgCh = objectImage.channels();
    int scnImgCh = sceneImage.channels();
    if (objImgCh != scnImgCh) {
        // conversion if images are not in same color space
        if (objImgCh == 4) { cvtColor(objectImage, objectImage, CV_BGRA2BGR); }
        if (scnImgCh == 4) { cvtColor(sceneImage, sceneImage, CV_BGRA2BGR); }
    }

    image = Mat(imageSizeFlipped.height, imageSizeFlipped.width, CV_8UC3, Scalar(0));
    float scaleObj, scaleScn;
    Point2f offsetObj, offsetScn;
    imageSizeFlipped.height *= 0.5f;
    // scale down to half image size and put them together
    scaleImageToSize(objectImage, imageSizeFlipped, scaleObj, offsetObj);
    scaleImageToSize(sceneImage, imageSizeFlipped, scaleScn, offsetScn);
    objectImage.copyTo(image(Rect(0, 0, objectImage.cols, objectImage.rows)));
    sceneImage.copyTo(image(Rect(0, objectImage.rows, sceneImage.cols, sceneImage.rows)));
    offsetScn.y += objectImage.rows;
    
    if (drawFilteredMatches && !drawAllMatches  && info.namedMatches.size() > 0) {
        drawMatches(image, info.namedMatches[info.namedMatches.size() - 1].second, info.objectKeyPoints, info.sceneKeyPoints, matchColor, offsetObj, offsetScn, scaleObj, scaleScn);
    }

    if (drawAllMatches) {
        int size = (int)info.namedMatches.size();
        for (int i = 0; i < size; i++) {
            if (i == 0 && size > 1) { continue; } // do not draw total matches
            int brightness = (int)(255 * (i + 1) / size);
            drawMatches(image, info.namedMatches[i].second, info.objectKeyPoints, info.sceneKeyPoints, Scalar(0, brightness, brightness), offsetObj, offsetScn, scaleObj, scaleScn);
        }
    }

    // rotate image
    flip(image, image, 1);
    transpose(image, image);
    return true;
}

bool ObjectTrackerDebugger::getTrackingModuleDebugImage(cv::Mat& image, const TrackerDebugInfo& info, bool drawObjectRect, bool drawFilteredPoints, bool drawAllPoints, bool drawSearchRect)
{
    if (info.currentModuleType != ModuleType2String::convert(MODULE_TYPE_TRACKING)) { return false; }
    
    Scalar pointsColor       = Scalar(  0, 255, 255);
    Scalar searchRectColor   = Scalar(255,   0, 255);
    Scalar rectColorPositive = Scalar(  0, 255,   0);
    Scalar rectColorNegative = Scalar(  0,   0, 255);

    info.sceneImageFull.copyTo(image);

    if (!drawAllPoints && drawFilteredPoints && info.namedPointSets.size() > 0) {
        drawPoints(image, info.namedPointSets[info.namedPointSets.size() - 1].second, pointsColor);
    }
    
    if (drawAllPoints) {
        int size = (int)info.namedPointSets.size();
        for (int i = 0; i < size; i++) {
            int brightness = (int)(255 * (i + 1) / size);
            drawPoints(image, info.namedPointSets[i].second, Scalar(0, brightness, brightness));
        }
    }
    
    if (drawSearchRect) {
        rectangle(image, info.searchRect.tl(), info.searchRect.br(), searchRectColor, 2);
    }
    
    if (drawObjectRect) {
        Scalar color = info.badHomography ? rectColorNegative : rectColorPositive;
        drawRect(image, info.objectCornersTransformed, color);
    }
    
    return true;
}

} // end of namespace
