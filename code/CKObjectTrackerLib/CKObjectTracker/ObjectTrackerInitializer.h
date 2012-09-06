//
//  ObjectTrackerInitializer.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 03.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_ObjectTrackerInitializer_h
#define CKObjectTrackerLib_ObjectTrackerInitializer_h

#include "ObjectTrackerImplementation.h"
#include "SettingsProjectScope.h"

#include "DetectionModule.h"
#include "ValidationModule.h"
#include "TrackingModule.h"

namespace ck {
    
    class ObjectTracker::Initializer {
        
    private:
        static void initDetectionSettings(DetectionModule* module, Settings& settings) {
            Settings detectionSettings = Settings("Detection Settings");
            settings.addCategory(detectionSettings);
        }
        
        static void initValidationSettings(ValidationModule* module, Settings& settings) {
            std::vector<std::string> estMethVals; estMethVals.push_back(EST_METHOD_RANSAC); estMethVals.push_back(EST_METHOD_LMEDS); estMethVals.push_back(EST_METHOD_DEFAULT);
            std::vector<std::string> detectorVals; detectorVals.push_back("FAST"); detectorVals.push_back("GFTT"); detectorVals.push_back("SIFT"); detectorVals.push_back("SURF"); detectorVals.push_back("ORB");
            std::vector<std::string> extractorVals; extractorVals.push_back("SIFT"); extractorVals.push_back("SURF"); extractorVals.push_back("ORB"); extractorVals.push_back("FREAK");
            std::vector<int> fastThresVals; fastThresVals.push_back(5); fastThresVals.push_back(10); fastThresVals.push_back(20); fastThresVals.push_back(40); fastThresVals.push_back(80);
            
            Settings filterSettings = Settings("Filter Parameters");
            filterSettings.registerBool("Sort Matches", module, &ValidationModule::setSortMatches, &ValidationModule::getSortMatches);
            filterSettings.registerBool("Crop Matches", module, &ValidationModule::setCropMatches, &ValidationModule::getCropMatches);
            filterSettings.registerInt("n Best Matches", module, &ValidationModule::setNBestMatches, &ValidationModule::getNBestMatches, 10, 300);
            filterSettings.registerBool("Symmetry Test", module, &ValidationModule::setSymmetryTestEnabled, &ValidationModule::getSymmetryTestEnabled);
            filterSettings.registerBool("Ratio Test", module, &ValidationModule::setRatioTestEnabled, &ValidationModule::getRatioTestEnabled);
            filterSettings.registerFloat("Ratio", module, &ValidationModule::setRatio, &ValidationModule::getRatio, 0.0f, 1.0f);

            Settings estimationSettings = Settings("Estimation Parameters");
            estimationSettings.registerString("Estimation Method", module, &ValidationModule::setEstimationMethod, &ValidationModule::getEstimationMethod, estMethVals);
            estimationSettings.registerInt("Ransac Threshold", module, &ValidationModule::setRansacThreshold, &ValidationModule::setRansacThreshold, 1, 9);
            estimationSettings.registerBool("Refine Homography", module, &ValidationModule::setRefineHomography, &ValidationModule::getRefineHomography);

            Settings validationSettings = Settings("Validation Settings");
            validationSettings.registerBool("Convert to Gray", module, &ValidationModule::setConvertToGray, &ValidationModule::getConvertToGray);
            validationSettings.registerString("Feature Detector", module, &ValidationModule::setDetector, &ValidationModule::getDetector, detectorVals);
            validationSettings.registerString("Descriptor Extractor", module, &ValidationModule::setExtractor, &ValidationModule::getExtractor, extractorVals);
            validationSettings.registerInt("Number of Features", module, &ValidationModule::setMaxFeatures, &ValidationModule::getMaxFeatures, 20, 1000);
            validationSettings.registerFloat("Hessian Threshold (SURF)", module, &ValidationModule::setHessianThreshold, &ValidationModule::getHessianThreshold, 300, 500);
            validationSettings.registerInt("Fast Threshold (FAST)", module, &ValidationModule::setFastThreshold, &ValidationModule::getFastThreshold, -1, -1, fastThresVals);
            
            validationSettings.addCategory(filterSettings);
            validationSettings.addCategory(estimationSettings);
            
            settings.addCategory(validationSettings);
        }
        
        static void initTrackingSettings(TrackingModule* module, Settings& settings) {
            Settings trackingSettings = Settings("Tracking Settings");
            settings.addCategory(trackingSettings);
        }
        
        
    public:
        static void initTracker(ObjectTracker::Implementation& tracker)
        {
            initDetectionSettings(dynamic_cast<DetectionModule*>(tracker._allModules[MODULE_TYPE_DETECTION]), tracker._settings);
            initValidationSettings(dynamic_cast<ValidationModule*>(tracker._allModules[MODULE_TYPE_VALIDATION]), tracker._settings);
            initTrackingSettings(dynamic_cast<TrackingModule*>(tracker._allModules[MODULE_TYPE_TRACKING]), tracker._settings);
        }
    };
    
} // end of namespace

#endif
