//
//  main.cpp
//  CKObjectTrackerDemo
//
//  Created by Christoph Kapffer on 17.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include "ObjectTracker.h"
#include "ObjectTrackerDebugger.h"

using namespace std;
using namespace cv;
using namespace ck;

int main(int argc, const char * argv[])
{
    if(argc != 1) {
        cout << "This program demonstrates the CKObjectTracker library.\n"
        << "Usage: " << argv[0] << " [-help]\n"
        << "You don't need to provide any parameters. The default capture device is used for tracking\n"
        << "-help prints this message\n."
        << "While tracking you can press following keys:\n"
        << "k - produces a key frame, which will represents the object to be tracked.\n"
        << "q - stops playback and quits the application.\n"
        << endl;
        return -1;
    }
    
    VideoCapture capture(0); // open the default camera
    if(!capture.isOpened()) {
        cout << "Could not find a capture device." << endl;
        return -1;
    }
    
    ObjectTracker tracker = ObjectTracker();
    //tracker.getSettings().setBoolValue(OT_SETTING_TRACK_ENABLED, false);
    
    TrackerOutput output;
    TrackerDebugInfo debugInfo;
    vector<TrackerDebugInfoStripped> infoCollection;
    Mat frame, detectionImage, validationImage, trackingImage;
    namedWindow("RawVideo");
    namedWindow("Detection");
    namedWindow("Validation");
    namedWindow("Tracking");
    
    bool endCapture = false;    
    while (!endCapture) {
        if (!capture.read(frame)) {
            cout << "Capture error." << endl;
            endCapture = true;
        }
        
        imshow("RawVideo", frame);
        
        tracker.trackObjectInVideo(frame, output, debugInfo);

        if (ObjectTrackerDebugger::getDetectionModuleDebugImage(detectionImage, debugInfo)) {
            imshow("Detection", detectionImage);
        }
        //drawObjectRect, drawObjectKeyPoints, drawSceneKeyPoints, drawFilteredMatches, drawAllMatches
        if (ObjectTrackerDebugger::getValidationModuleDebugImage(validationImage, debugInfo)) {
            imshow("Validation", validationImage);
        }
        if (ObjectTrackerDebugger::getTrackingModuleDebugImage(trackingImage, debugInfo)) {
            imshow("Tracking", trackingImage);
        }
        
        TrackerDebugInfoStripped stripped = TrackerDebugInfoStripped(debugInfo);
        cout << ObjectTrackerDebugger::getDebugString(stripped) << endl;
        infoCollection.push_back(stripped);
        
        char key = (char)waitKey(30);
        switch(key)
        {
            case 'k':
                cout << "Took keyframe." << endl;
                tracker.setObject(frame);
                break;
            case 'q':
                cout << "Done." << endl;
                endCapture = true;
                break;
            default:
                break;
        }
    }
    
    cout << "\n" << ObjectTrackerDebugger::getDebugString(infoCollection) << endl;
    
    return 0;
}

