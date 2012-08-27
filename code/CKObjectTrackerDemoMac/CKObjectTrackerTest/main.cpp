//
//  main.cpp
//  CKObjectTrackerTest
//
//  Created by Christoph Kapffer on 17.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#include <string>
#include <iostream>
#include <mach-o/dyld.h>
#include <opencv2/opencv.hpp>

#include "ObjectTracker.h"
#include "ObjectTrackerDebugger.h"

using namespace std;
using namespace cv;
using namespace ck;

bool isVideoFile(const string& file)
{
    string extension = file.substr(file.find_last_of(".") + 1);
    if (extension == "avi" || extension == "mov") {
        return true;
    }
    // if it doesen't match any of these, just guess that it must be video.
    return !(extension == ".bmp" || extension == ".dib" || extension == ".jpeg"
             || extension == ".jpg" || extension == ".jpe" || extension == ".jp2"
             || extension == ".png" || extension == ".pbm" || extension == ".pgm"
             || extension == ".ppm" || extension == ".sr" || extension == ".ras"
             || extension == ".tiff" || extension == ".tif");
}

string getFullpath(string file)
{
    char path[1024];
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) != 0) {
        cout << "The absolute path to the video file is too long."  << endl;
        exit(-1);
    }
    string pathString = string(path);
    pathString.erase(pathString.find_last_of('/') + 1);
    return pathString + file;
}

void trackObjectInVideo(const Mat& objectImage, VideoCapture sceneVideo)
{
    ObjectTracker tracker = ObjectTracker();
    tracker.setObject(objectImage);
    
    TrackerOutput output;
    TrackerDebugInfo debugInfo;
    
    Mat frame;
    bool firstRun = true;
    bool endCapture = false;
    while (!endCapture) {
        if (!sceneVideo.read(frame)) {
            cout << "Capture error." << endl;
            endCapture = true;
        }

        tracker.trackObjectInVideo(frame, output, debugInfo);
        vector<pair<string, Mat> > debugImages = ObjectTrackerDebugger::debugImages(debugInfo);
        for (int i = 0; i < debugImages.size(); i++) {
            pair<string, Mat> item = debugImages[i];
            if (firstRun) { namedWindow(item.first); }
            imshow(item.first, item.second);
        }
        firstRun = false;
        
        cout << debugInfo.currentModuleType << " ";
        cout << "took " <<  debugInfo.totalProcessingTime << " ms";
        cout << " which results in " << 1000 / debugInfo.totalProcessingTime << " FPS." << endl;
    
        if (waitKey(1000 / sceneVideo.get(CV_CAP_PROP_FPS)) >= 0) {
            cout << "User stopped playback." << endl;
            endCapture = true;
        }
        
        //waitKey();
        if (sceneVideo.get(CV_CAP_PROP_POS_FRAMES) == 0) {
            cout << "Done." << endl;
            endCapture = true;
        }
    }
}

void trackObjectInStillImage(const Mat& objectImage, const Mat& sceneImage)
{
    vector<TrackerOutput> output;
    vector<TrackerDebugInfo> debugInfo;
    
    ObjectTracker tracker = ObjectTracker();
    tracker.setObject(objectImage);
    tracker.trackObjectInStillImage(objectImage, output, debugInfo);
    if (!output.at(output.size() - 1).isObjectPresent) {
        cout << "Object not found" << endl;
    }
}

int main(int argc, const char * argv[])
{
    if(argc != 3) {
        cout << "This program is used for quality and performance testing of the CKObjectTracker library.\n"
             << "Usage: " << argv[0] << " -help | <object> <scene>\n"
             << "-help prints this message\n"
             << "<object> is a realtive path to an image containing the object to be tracked.\n"
             << "         Valid image formats are: .bmp, .dib, .jpeg, .jpg, .jpe, .jp2,\n"
             << "         .png, .pbm, .pgm, .ppm, .sr, .ras, .tiff and .tif\n"
             << "<scene> is a relative path to either an image or a video showing a scene,\n"
             << "        in which the object will be tracked.\n"
             << "        Valid video formats are: .avi, .mov and probably others.\n"
             << "        Just try if it works.\n"
             << endl;
        return -1;
    }
        
    Mat objectImage = imread(argv[1]);
    if (objectImage.empty()) {
        cout << "Could not open object image '" << argv[1] <<  "'.\n"
             << "See " << argv[0] << " -help for supported image formats." << endl;
        return -1;
    }
    
    string secondArgument = string(argv[2]);
    if (isVideoFile(secondArgument)) {
        VideoCapture sceneVideo(getFullpath(secondArgument));
        if (!sceneVideo.isOpened()) {
            cout << "Could not open scene video '" << secondArgument <<  "'.\n"
            << "See " << argv[0] << " -help for supported video formats." << endl;
            return -1;
        }
        trackObjectInVideo(objectImage, sceneVideo);
    } else {
        Mat sceneImage = imread(secondArgument);
        if (sceneImage.empty()) {
            cout << "Could not open scene image '" << secondArgument <<  "'.\n"
            << "See " << argv[0] << " -help for supported image formats." << endl;
            return -1;
        }
        trackObjectInStillImage(objectImage, sceneImage);
    }
    
    return 0;
}

