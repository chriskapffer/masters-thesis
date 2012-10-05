//
//  SanityCheck.cpp
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 27.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#define _USE_MATH_DEFINES

#include <math.h>
#include "SanityCheck.h"

using namespace std;
using namespace cv;

namespace ck {

struct Line2f
{
    float sX; // startX
    float sY; // startY
    float eX; // endX
    float eY; // endY
    
    Line2f(float sX, float sY, float eX, float eY) : sX(sX), sY(sY), eX(eX), eY(eY) { }
};

bool SanityCheck::checkRectangle(const vector<Point2f>& cornersTransformed, Point2f& intersectionPoint)
{
    // The rectangle is convex and not twisted, if the lines
    // between opposing points intersect with each other.
    
    // intersection test based on: http://paulbourke.net/geometry/lineline2d/
    Line2f l1 = Line2f(cornersTransformed[0].x, cornersTransformed[0].y, cornersTransformed[2].x, cornersTransformed[2].y);
    Line2f l2 = Line2f(cornersTransformed[1].x, cornersTransformed[1].y, cornersTransformed[3].x, cornersTransformed[3].y);
    
    // equal denominator for ua and ub
    float d = (l2.eY - l2.sY) * (l1.eX - l1.sX) - (l2.eX - l2.sX) * (l1.eY - l1.sY);
    if (d == 0.0f) return false; // lines are parallel (or coincidental if numerators are also 0)
    
    // seperate numerators for ua and ub
    float n_a = (l2.eX - l2.sX) * (l1.sY - l2.sY) - (l2.eY - l2.sY) * (l1.sX - l2.sX);
    float n_b = (l1.eX - l1.sX) * (l1.sY - l2.sY) - (l1.eY - l1.sY) * (l1.sX - l2.sX);
    
    // intermediate fractional point that the lines potentially intersect
    float ua = n_a / d;
    float ub = n_b / d;
    
    // The fractional point will be between 0 and 1 inclusive if the lines
    // intersect. If the fractional calculation is larger than 1 or smaller
    // than 0 the lines would need to be longer to intersect.
    if ((ua != 0.0f && ub != 0.0f) && (ua >= 0.0f && ua <= 1.0f) && (ub >= 0.0f && ub <= 1.0f)) {
        intersectionPoint.x = l1.sX + (ua * (l1.eX - l1.sX));
        intersectionPoint.y = l1.sY + (ua * (l1.eY - l1.sY));
        return true;
    }
    return false;
}

bool SanityCheck::checkBoundaries(const vector<Point2f>& cornersTransformed, int width, int height)
{
    // check if all transformed corners are within specified dimensions
    for (vector<Point2f>::const_iterator iter = cornersTransformed.begin(); iter != cornersTransformed.end(); iter++) {
        if ((*iter).x < 0 || (*iter).x >= width || (*iter).y < 0 || (*iter).y >= height) {
            return false;
        }
    }
    return true;
}
    
static float angleBetween(const Point2f& v1, const Point2f& v2)
{
    float length1 = sqrt(v1.x * v1.x + v1.y * v1.y);
    float length2 = sqrt(v2.x * v2.x + v2.y * v2.y);
    float dot = v1.x * v2.x + v1.y * v2.y;
    return acos(dot / (length1 * length2));
}
    
    // TODO: check min Angle
    // TODO: check Area
    
    
bool SanityCheck::checkAngleSimilarity(const vector<Point2f>& cornersTransformed, float maxAngleOffsetInDeg)
{
    Point2f tlr = cornersTransformed[3] - cornersTransformed[0];
    Point2f tll = cornersTransformed[1] - cornersTransformed[0];
    Point2f trr = cornersTransformed[0] - cornersTransformed[1];
    Point2f trl = cornersTransformed[2] - cornersTransformed[1];
    Point2f brr = cornersTransformed[1] - cornersTransformed[2];
    Point2f brl = cornersTransformed[3] - cornersTransformed[2];
    Point2f blr = cornersTransformed[2] - cornersTransformed[3];
    Point2f bll = cornersTransformed[0] - cornersTransformed[3];

    float atl = angleBetween(tlr, tll);
    float atr = angleBetween(trr, trl);
    float abr = angleBetween(brr, brl);
    float abl = angleBetween(blr, bll);
    
    float maxOffset = maxAngleOffsetInDeg * (M_PI / 180.0f);
    
    return (fabs(atl - atr) <= maxOffset || fabs(atl - abl) <= maxOffset)
        && (fabs(abr - abl) <= maxOffset || fabs(abr - atr) <= maxOffset);
}

bool SanityCheck::validate(const cv::Mat& homography, const cv::Size& imageSize, const std::vector<cv::Point2f>& corners, std::vector<cv::Point2f>& cornersTransformed)
{
    Rect dummy;
    return validate(homography, imageSize, corners, cornersTransformed, dummy, false);
}
    
bool SanityCheck::validate(const cv::Mat& homography, const cv::Size& imageSize, const std::vector<cv::Point2f>& corners, std::vector<cv::Point2f>& cornersTransformed, cv::Rect& boundingRect, bool cropBoundingRectToImageSize)
{
    perspectiveTransform(corners, cornersTransformed, homography);
    
    Point2f dummy;
    bool isValid = true
        //&& checkBoundaries(cornersTransformed, imageSize.width, imageSize.height)
        //&& checkAngleSimilarity(cornersTransformed, MAX_ANGLE_OFFSET_DEG)
        && checkRectangle(cornersTransformed, dummy);

    boundingRect = cv::boundingRect(cornersTransformed);
    if (cropBoundingRectToImageSize) {
        boundingRect.x = MAX(MIN(boundingRect.x, imageSize.width), 0);
        boundingRect.y = MAX(MIN(boundingRect.y, imageSize.height), 0);
        boundingRect.width = MAX(MIN(boundingRect.width, imageSize.width - boundingRect.x), 0);
        boundingRect.height = MAX(MIN(boundingRect.height, imageSize.height - boundingRect.y), 0);
    }
    
    return isValid;
}
    
} // end of namepsace
