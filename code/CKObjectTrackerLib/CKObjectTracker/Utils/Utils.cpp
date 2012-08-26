//
//  Utils.cpp
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 22.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#include "Utils.h"

using namespace cv;
using namespace std;

namespace ck {

vector<MatND> Utils::calcHistograms(const Mat& image, int numOfBins)
{
    vector<Mat> planes;
    vector<MatND> histograms;
    split(image, planes);
    
    float range[] = {0.0f, 255.0f};
    const float* pixelRange = {range};

    for (int i = 0; i < planes.size(); i++) {
        MatND histogram;
        calcHist(&planes[i],
                 1,             // num of images
                 0,             // channels
                 Mat(),         // mask
                 histogram,     // result
                 1,             // dimensions
                 &numOfBins,    //num of bins
                 &pixelRange    // max and min range
            );
        histograms.push_back(histogram);
    }

    return histograms;
}

Mat Utils::imageFromHistograms(const vector<MatND>& histgrams, int width, int height, int bins)
{
    vector<Scalar> colors;
    int numOfHists = (int)histgrams.size();
    if (numOfHists == 1) {
        colors.push_back(Scalar(255, 255, 255));
    }
    else if (numOfHists == 3 || numOfHists == 4) {
        colors.push_back(Scalar(255, 0, 0));
        colors.push_back(Scalar(0, 255, 0));
        colors.push_back(Scalar(0, 0, 255));
        if (numOfHists == 4)
            colors.push_back(Scalar(255, 255, 255));
    }
    else {
        for (int i = 0; i < numOfHists; i++) {
            colors.push_back(Scalar(rand()&255, rand()&255, rand()&255));
        }
    }
    
    int binWidth = cvRound(width/(float)bins);
    Mat image(height, width, CV_8UC3, Scalar(0,0,0));
    for (int i = 0; i < histgrams.size(); i++) {
        MatND hist = MatND(histgrams[i]);
        normalize(hist, hist, 0, image.rows, NORM_MINMAX, -1, Mat());
        for (int j = 1; j < bins; j++) {
            line(image,
                 Point(binWidth * (j-1), height - cvRound(hist.at<float>(j-1))),
                 Point(binWidth * (j  ), height - cvRound(hist.at<float>(j  ))),
                 colors[i], 2);
        }
    }

    return image;
}

void Utils::matchSimple(const Ptr<DescriptorMatcher>& matcher, const Mat& descriptors1, const Mat& descriptors2, vector<DMatch>& matches, bool crossCheck)
{
    // find matches;
    vector<DMatch> forwardMatches;
    matcher->match(descriptors1, descriptors2, forwardMatches);
    if (crossCheck) {
        vector<DMatch> backwardMatches;
        matcher->match(descriptors2, descriptors1, backwardMatches);
        for(int i = 0; i < backwardMatches.size(); i++) {
            DMatch forward = forwardMatches[i];
            if (forward.trainIdx < backwardMatches.size()) {
                DMatch backward = backwardMatches[forward.trainIdx];
                if(backward.trainIdx == forward.queryIdx) {
                    matches.push_back(forward);
                }
            }
        }
    }
    else {
        matches = forwardMatches;
    }
}

void Utils::matchAdvanced(const Ptr<DescriptorMatcher>& matcher, const Mat& descriptors1, const Mat& descriptors2, vector<DMatch>& matches, int knn)
{
    // find matches
    vector<vector<DMatch> > forwardMatches, backwardMatches;
    matcher->knnMatch(descriptors1, descriptors2, forwardMatches, knn);
    matcher->knnMatch(descriptors2, descriptors1, backwardMatches, knn);
    for (size_t m = 0; m < forwardMatches.size(); m++)
    {
        bool findCrossCheck = false;
        for (size_t fk = 0; fk < forwardMatches[m].size(); fk++)
        {
            DMatch forward = forwardMatches[m][fk];
            
            for (size_t bk = 0; bk < backwardMatches[forward.trainIdx].size(); bk++)
            {
                DMatch backward = backwardMatches[forward.trainIdx][bk];
                if (backward.trainIdx == forward.queryIdx)
                {
                    matches.push_back(forward);
                    findCrossCheck = true;
                    break;
                }
            }
            if (findCrossCheck)
                break;
        }
    }
}

void Utils::get2DCoordinatesOfMatches(const vector<DMatch>& matches, const vector<KeyPoint>& keypoints1, const vector<KeyPoint>& keypoints2, vector<Point2f>& coordinates1, vector<Point2f>& coordinates2)
{
    // get indices from matches for keypoint look up
    vector<int> indexSet1(matches.size());
    vector<int> indexSet2(matches.size());
    for (int i = 0; i < matches.size(); i++) {
        indexSet1[i] = matches[i].queryIdx;
        indexSet2[i] = matches[i].trainIdx;
    }
    
    // convert keypoints to x and y coordinates
    KeyPoint::convert(keypoints1, coordinates1, indexSet1);
    KeyPoint::convert(keypoints2, coordinates2, indexSet2);
}

void Utils::calcHomography(const vector<KeyPoint>& keypoints1, const vector<KeyPoint>& keypoints2, const vector<DMatch>& matches, Mat& H, int method, double ransacThreshold)
{
    vector<Point2f> coordinates1;
    vector<Point2f> coordinates2;
    get2DCoordinatesOfMatches(matches, keypoints1, keypoints2, coordinates1, coordinates2);
    calcHomography(coordinates1, coordinates2, H, method, ransacThreshold);
}

void Utils::calcHomography(const vector<Point2f>& coordinates1, const vector<Point2f>& coordinates2, Mat& H, int method, double ransacThreshold)
{
    if (MIN(coordinates1.size(), coordinates2.size()) < 4) {
        cout << "WARNING: Not enough matches to find homography!" << endl;
        H = Mat::eye(3, 3, CV_32FC1);
        return;
    }
    
    // find homography transformation matrix
    H = findHomography(coordinates1, coordinates2, method, ransacThreshold);
}

vector<char> Utils::createMaskWithOriginalPointSet(const vector<Point2f>& coordinates1, const vector<Point2f>& coordinates2, const Mat& H, double ransacThreshold)
{
    vector<Point2f> coordinates1Transformed;
    perspectiveTransform(coordinates1, coordinates1Transformed, H);
    return createMaskWithTransformedPointSet(coordinates1Transformed, coordinates2, ransacThreshold);
}

vector<char> Utils::createMaskWithTransformedPointSet(const vector<Point2f>& coordinates1, const vector<Point2f>& coordinates2, double ransacThreshold)
{
    int count = (int)coordinates1.size();
    vector<char> mask(count, 0);
    
    for (int i = 0; i < count; i++) {
        double distance = norm(coordinates2[i] - coordinates1[i]);
        if (distance <= ransacThreshold)
            mask[i] = 1;
    }
    return mask;
}

vector<char> Utils::invertedMask(const vector<char>& original)
{
    vector<char> inverted(original.size());
    for (int i = 0; i < inverted.size(); i++) {
        inverted[i] = !original[i];
    }
    return inverted;
}

vector<DMatch> Utils::filteredMatchesWithMask(const vector<DMatch>& matches, const vector<char>& mask)
{
    vector<DMatch> filtered;
    for (int i = 0; i < mask.size(); i++) {
        if (mask[i] == 1)
            filtered.push_back(matches[i]);
    }
    return filtered;
}

vector<DMatch> Utils::nBestMatches(const vector<DMatch>& matches, int numberOfMatchesToKeep)
{
    vector<DMatch> bestMatches = matches;
    if (matches.size() > numberOfMatchesToKeep) {
        nth_element(bestMatches.begin(), bestMatches.begin() + numberOfMatchesToKeep - 1, bestMatches.end());
        bestMatches.erase(bestMatches.begin() + numberOfMatchesToKeep, bestMatches.end());
    }
    return bestMatches;
}

int Utils::positiveMatches(const vector<char>& mask)
{
    int count = 0;
    for (int i = 0; i < mask.size(); i++) {
        if (mask[i] == 1)
            count++;
    }
    return count;
}

void Utils::maxMinDistance(const vector<DMatch>& matches, float& maxDistance, float& minDistance)
{
    maxDistance = 0;
    minDistance = MAXFLOAT;
    for (int i = 0; i < matches.size(); i++) {
        float distance = matches[i].distance;
        if (distance > maxDistance) {
            maxDistance = distance;
        }
        if (distance < minDistance) {
            minDistance = distance;
        }
    }
}

double Utils::distanceBetween(const Point2f& v1, const Point2f& v2)
{
    Point2f delta = v2 - v1;
    return sqrt(delta.x * delta.x + delta.y * delta.y);
}

double Utils::cumulatedDistance(const vector<Point2f>& c1, const vector<Point2f>& c2)
{
    double distance = 0;
    for (int i = 0; i < c1.size(); i++) {
        distance += distanceBetween(c1[i], c2[i]);
    }
    return distance;
}

#ifndef __ARM_NEON__

Mat Utils::bgra2Gray(Mat imgIn, int method)
{
    Mat imgOut;
    cvtColor(imgIn, imgOut, CV_BGRA2GRAY);
    return imgOut;
}

#else

static void neon_convert (uint8_t * __restrict dest, uint8_t * __restrict src, int numPixels);
static void neon_asm_convert(uint8_t * __restrict dest, uint8_t * __restrict src, int numPixels);

Mat Utils::bgra2Gray(Mat imgIn, int method)
{
    Mat imgOut;
    int size = imgIn.rows * imgIn.cols;
    
    switch (method) {
        case BGRA2GRAY_CV:
            cvtColor(imgIn, imgOut, CV_BGRA2GRAY);
            break;
        case BGRA2GRAY_NEON:
            imgOut = Mat(imgIn.rows, imgIn.cols, CV_8UC1);
            neon_convert(imgOut.data, imgIn.data, size);
            //uint8_t* baseAddressGray = (uint8_t*) malloc(size);
            break;
        case BGRA2GRAY_NEON_ASM:
            imgOut = Mat(imgIn.rows, imgIn.cols, CV_8UC1);
            neon_asm_convert(imgOut.data, imgIn.data, size);
            //uint8_t* baseAddressGray = (uint8_t*) malloc(size);
            break;
        default:
            imgOut = imgIn;
            break;
    }
    
    return imgOut;
}

#include <arm_neon.h>

// code taken from: http://computer-vision-talks.com/2011/02/a-very-fast-bgra-to-grayscale-conversion-on-iphone/
static void neon_convert (uint8_t * __restrict dest, uint8_t * __restrict src, int numPixels)
{
    int i;
    // Y = 0.299 R + 0.587 G + 0.114 B http://en.wikipedia.org/wiki/YCbCr
    uint8x8_t rfac = vdup_n_u8 (77);
    uint8x8_t gfac = vdup_n_u8 (150);
    uint8x8_t bfac = vdup_n_u8 (29);
    int n = numPixels / 8;
    
    // Convert per eight pixels
    for (i=0; i < n; ++i)
    {
        uint16x8_t  temp;
        uint8x8x4_t rgb  = vld4_u8 (src);
        uint8x8_t result;
        
        temp = vmull_u8 (rgb.val[0],      bfac);
        temp = vmlal_u8 (temp,rgb.val[1], gfac);
        temp = vmlal_u8 (temp,rgb.val[2], rfac);
        
        result = vshrn_n_u16 (temp, 8);
        vst1_u8 (dest, result);
        src  += 8*4;
        dest += 8;
    }
}

static void neon_asm_convert(uint8_t * __restrict dest, uint8_t * __restrict src, int numPixels)
{
	__asm__ volatile("lsr          %2, %2, #3      \n"
					 "# build the three constants: \n"
					 "mov         r4, #29          \n" // Blue channel multiplier
					 "mov         r5, #150         \n" // Green channel multiplier
					 "mov         r6, #77          \n" // Red channel multiplier
					 "vdup.8      d4, r4           \n"
					 "vdup.8      d5, r5           \n"
					 "vdup.8      d6, r6           \n"
					 "0:						   \n"
					 "# load 8 pixels:             \n"
					 "vld4.8      {d0-d3}, [%1]!   \n"
					 "# do the weight average:     \n"
					 "vmull.u8    q7, d0, d4       \n"
					 "vmlal.u8    q7, d1, d5       \n"
					 "vmlal.u8    q7, d2, d6       \n"
					 "# shift and store:           \n"
					 "vshrn.u16   d7, q7, #8       \n" // Divide q3 by 256 and store in the d7
					 "vst1.8      {d7}, [%0]!      \n"
					 "subs        %2, %2, #1       \n" // Decrement iteration count
					 "bne         0b            \n" // Repeat unil iteration count is not zero
					 :
					 : "r"(dest), "r"(src), "r"(numPixels)
					 : "r4", "r5", "r6"
					 );
}

#endif

} // end of namespace
