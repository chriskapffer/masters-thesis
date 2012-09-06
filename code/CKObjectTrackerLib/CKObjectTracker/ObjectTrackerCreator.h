//
//  ObjectTrackerCreator.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 03.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_ObjectTrackerCreator_h
#define CKObjectTrackerLib_ObjectTrackerCreator_h

#include "ObjectTrackerImpl.h"
#include "SettingsProjectScope.h"
#include "Profiler.h"

#include "AbstractModule.h"
#include "DetectionModule.h"
#include "ValidationModule.h"
#include "TrackingModule.h"
#include "EmptyModule.h"

namespace ck {
    
    class ObjectTracker::Creator {
    public:
        static void initializeTracker(ObjectTracker::Implementation& tracker)
        {
            tracker._allModules = createModuleCollection();
            tracker._currentModule = tracker._allModules[MODULE_TYPE_EMPTY];
            tracker._moduleParams.successor = MODULE_TYPE_EMPTY;
            
            initDetectionSettings(dynamic_cast<DetectionModule*>(tracker._allModules[MODULE_TYPE_DETECTION]), tracker._settings);
            initValidationSettings(dynamic_cast<ValidationModule*>(tracker._allModules[MODULE_TYPE_VALIDATION]), tracker._settings);
            initTrackingSettings(dynamic_cast<TrackingModule*>(tracker._allModules[MODULE_TYPE_TRACKING]), tracker._settings);
        }
        
        static void finalizeTracker(ObjectTracker::Implementation& tracker)
        {
            std::map<ModuleType, AbstractModule*>::iterator it;
            for (it = tracker._allModules.begin(); it != tracker._allModules.end(); it++) {
                delete (*it).second;
            }
            tracker._allModules.clear();
            
            Profiler::Finish();
        }
        
    private:
        static std::map<ModuleType, AbstractModule*> createModuleCollection() {
            std::vector<FilterFlag> flags;
            flags.push_back(FILTER_FLAG_RATIO);
            
            std::map<ModuleType, AbstractModule*> modules;
            modules[MODULE_TYPE_DETECTION] = new DetectionModule();
            modules[MODULE_TYPE_VALIDATION] = new ValidationModule(flags);
            modules[MODULE_TYPE_TRACKING] = new TrackingModule(500);
            modules[MODULE_TYPE_EMPTY] = new EmptyModule();
            return modules;
        }

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
            validationSettings.registerBool("Convert to Gray", module, &ValidationModule::setConvertToGray, &ValidationModule::getConvertToGray, true);
            validationSettings.registerString("Feature Detector", module, &ValidationModule::setDetector, &ValidationModule::getDetector, detectorVals, true);
            validationSettings.registerString("Descriptor Extractor", module, &ValidationModule::setExtractor, &ValidationModule::getExtractor, extractorVals, true);
            validationSettings.registerInt("Number of Features", module, &ValidationModule::setMaxFeatures, &ValidationModule::getMaxFeatures, 20, 1000);
            validationSettings.registerFloat("Hessian Threshold (SURF)", module, &ValidationModule::setHessianThreshold, &ValidationModule::getHessianThreshold, 300, 500);
            validationSettings.registerInt("Fast Threshold (FAST)", module, &ValidationModule::setFastThreshold, &ValidationModule::getFastThreshold, -1, -1, fastThresVals);
            
            validationSettings.addCategory(filterSettings);
            validationSettings.addCategory(estimationSettings);
            
            settings.addCategory(validationSettings);
        }
        
        static void initTrackingSettings(TrackingModule* module, Settings& settings) {
            std::vector<int> winSizeVals; winSizeVals.push_back(11); winSizeVals.push_back(21); winSizeVals.push_back(31); winSizeVals.push_back(41);
            
            Settings terminationSettings = Settings("Stop tracking after");
            terminationSettings.registerInt("n Points remaining", module, &TrackingModule::setMinPointsAbsolute, &TrackingModule::getMinPointsAbsolute, 4, 1000);
            terminationSettings.registerInt("% Points remaining", module, &TrackingModule::setMinPointsRelative, &TrackingModule::getMinPointsRelative, 1, 99);
            terminationSettings.registerInt("max successive Frames (-1 -> oo)", module, &TrackingModule::setMaxSuccessiveFrames, &TrackingModule::getMaxSuccessiveFrames, -1, 120);
            terminationSettings.registerInt("max inter-frame Movement (in px)", module, &TrackingModule::setMaxTransformationDelta, &TrackingModule::getMaxTransformationDelta, 10, 200);
            
            Settings flowSettings = Settings("Optical Flow Parameters");
            flowSettings.registerInt("max Iterations (until termination)", module, &TrackingModule::setLKTermIter, &TrackingModule::getLKTermIter, 10, 50);
            flowSettings.registerFloat("min Epsilon (until termination)", module, &TrackingModule::setLKTermEpsilon, &TrackingModule::getLKTermEpsilon, 0.001f, 0.1f);
            flowSettings.registerInt("Search Window Size", module, &TrackingModule::setLKWinSize, &TrackingModule::getLKWinSize, -1, -1, winSizeVals);
            flowSettings.registerInt("max Pyramid Levels", module, &TrackingModule::setLKMaxLevel, &TrackingModule::getLKMaxLevel, 0, 5);
            flowSettings.registerFloat("min Eigenvalue Threshold", module, &TrackingModule::setLKMinEigenThreshold, &TrackingModule::getLKMinEigenThreshold, 0.0001f, 0.01f);
            
            Settings trackingSettings = Settings("Tracking Settings");
            trackingSettings.registerBool("Tracking", module, &TrackingModule::setEnabled, &TrackingModule::getEnabled);
            trackingSettings.registerBool("Use Sub-Pixel Accuracy", module, &TrackingModule::setUseSubPixels, &TrackingModule::getUseSubPixels);
            trackingSettings.registerBool("calc Homography from frame to frame", module, &TrackingModule::setCalcHomRelToFrame, &TrackingModule::getCalcHomRelToFrame);
            trackingSettings.registerBool("Divergence Filter", module, &TrackingModule::setFilterDistortions, &TrackingModule::getFilterDistortions);
            trackingSettings.registerFloat("Divergence Threshold", module, &TrackingModule::setDistortionThreshold, &TrackingModule::getDistortionThreshold, 0.1f, 2.0f);
            
            trackingSettings.addCategory(terminationSettings);
            trackingSettings.addCategory(flowSettings);
            
            settings.addCategory(trackingSettings);
        }
    };
    
} // end of namespace

#endif
