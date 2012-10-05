//
//  TransformationBuilder.cpp
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 05.10.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#include "TransformationBuilder.h"
#include "CameraParams.h"
#include "SanityCheck.h"

using namespace std;
using namespace cv;

namespace ck {
    
    Mat TransformationBuilder::getRotation(const Mat& homography)
    {
        Mat invIntrinsics = CameraParams::Instance()->getInverseIntrinsics();
        
        // Column vectors of homography
        Mat h1 = homography.col(0);
        Mat h2 = homography.col(1);
        
        // inverse
        Mat invH1 = invIntrinsics * h1;
        Mat invH2 = invIntrinsics * h2;
        
        // Column vectors of rotation matrix
        Mat r1 = (1/norm(invH1)) * invH1;
        Mat r2 = (1/norm(invH2)) * invH2;
        Mat r3 = r1.cross(r2);
        
        // create a 4x4 matrix
        return Mat_<double>(3,3) <<
            r1.at<double>(0,0), r2.at<double>(0,0), r3.at<double>(0,0),
            r1.at<double>(1,0), r2.at<double>(1,0), r3.at<double>(1,0),
            r1.at<double>(2,0), r2.at<double>(2,0), r3.at<double>(2,0);
    }
    
    Point2f TransformationBuilder::getTranslation(const vector<Point2f>& objectCornersTransformed, const Size2f& imageSize)
    {
        Point2f translation;
        if (!SanityCheck::checkRectangle(objectCornersTransformed, translation)) {
            return Point2f();
        }
        // normalize with image dimensions
        translation.x /= imageSize.width;
        translation.y /= imageSize.height;
        return translation;
    }
    
    float TransformationBuilder::getScale(const vector<Point2f>& objectCornersTransformed, const vector<Point2f>& objectCorners)
    {
        return contourArea(objectCornersTransformed) / contourArea(objectCorners);
    }
    
    Transform TransformationBuilder::getTransform(const Mat& homography, const vector<Point2f>& objectCorners, const Size2f& imageSize)
    {
        vector<Point2f> objectCornersTransformed;
        perspectiveTransform(objectCorners, objectCornersTransformed, homography);
        
        Transform result;
        result.translation = getTranslation(objectCornersTransformed, imageSize);
        result.scale = getScale(objectCornersTransformed, objectCorners);
        result.rotation = getRotation(homography);
        return result;
    }
}