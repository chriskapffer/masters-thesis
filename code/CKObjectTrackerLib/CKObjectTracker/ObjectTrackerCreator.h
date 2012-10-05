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
#include "ObjectTrackerTypesPublic.h"
#include "ObjectTrackerTypesProject.h"
#include "SettingsProjectScope.h"

#include "AbstractModule.h"
#include "DetectionModule.h"
#include "ValidationModule.h"
#include "TrackingModule.h"
#include "EmptyModule.h"
#include "Profiler.h"

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

        static void initDetectionSettings(DetectionModule* module, Settings& settings)
        {
            Settings camShiftSettings = Settings("Cam Shift Parameters");
            
            camShiftSettings.registerInt(OT_SETTING_DETEC_SEARCH_RECT_MIN, module, &DetectionModule::setMinSearchRectSizeRelative, &DetectionModule::getMinSearchRectSizeRelative, 1, 99);
            camShiftSettings.registerInt(OT_SETTING_DETEC_SEARCH_RECT_MAX, module, &DetectionModule::setMaxSearchRectSizeRelative, &DetectionModule::getMaxSearchRectSizeRelative, 1, 99);
            camShiftSettings.registerInt(OT_SETTING_DETEC_CAMSHIFT_TERM_MAX_ITER, module, &DetectionModule::setCamShiftTermIter, &DetectionModule::getCamShiftTermIter, 10, 50);
            camShiftSettings.registerFloat(OT_SETTING_DETEC_CAMSHIFT_TERM_MIN_EPS, module, &DetectionModule::setCamShiftTermEpsilon, &DetectionModule::getCamShiftTermEpsilon, 0.001f, 0.1f);
            
            Settings detectionSettings = Settings("Detection Settings");
            detectionSettings.registerBool(OT_SETTING_DETEC_ENABLED, module, &DetectionModule::setEnabled, &DetectionModule::getEnabled);
            
            detectionSettings.addCategory(camShiftSettings);
            
            settings.addCategory(detectionSettings);
        }
        
        static void initValidationSettings(ValidationModule* module, Settings& settings)
        {
            std::vector<std::string> estMethVals; estMethVals.push_back(EST_METHOD_RANSAC); estMethVals.push_back(EST_METHOD_LMEDS); estMethVals.push_back(EST_METHOD_DEFAULT);
            std::vector<std::string> detectorVals; detectorVals.push_back("FAST"); detectorVals.push_back("GFTT"); detectorVals.push_back("SIFT"); detectorVals.push_back("SURF"); detectorVals.push_back("ORB");
            std::vector<std::string> extractorVals; extractorVals.push_back("SIFT"); extractorVals.push_back("SURF"); extractorVals.push_back("ORB"); extractorVals.push_back("FREAK");
            std::vector<int> fastThresVals; fastThresVals.push_back(10); fastThresVals.push_back(20); fastThresVals.push_back(40); fastThresVals.push_back(60); fastThresVals.push_back(80);
            
            Settings filterSettings = Settings("Filter Parameters");
            filterSettings.registerBool(OT_SETTING_VALID_MATCHES_SORT, module, &ValidationModule::setSortMatches, &ValidationModule::getSortMatches);
            filterSettings.registerBool(OT_SETTING_VALID_MATCHES_CROP, module, &ValidationModule::setCropMatches, &ValidationModule::getCropMatches);
            filterSettings.registerInt(OT_SETTING_VALID_N_BEST_MATCHES, module, &ValidationModule::setNBestMatches, &ValidationModule::getNBestMatches, 10, 300);
            filterSettings.registerBool(OT_SETTING_VALID_FILTER_SYMM, module, &ValidationModule::setSymmetryTestEnabled, &ValidationModule::getSymmetryTestEnabled);
            filterSettings.registerBool(OT_SETTING_VALID_FILTER_RATIO, module, &ValidationModule::setRatioTestEnabled, &ValidationModule::getRatioTestEnabled);
            filterSettings.registerFloat(OT_SETTING_VALID_RATIO_THRESHLD, module, &ValidationModule::setRatio, &ValidationModule::getRatio, 0.0f, 1.0f);

            Settings estimationSettings = Settings("Estimation Parameters");
            estimationSettings.registerString(OT_SETTING_VALID_EST_METHOD, module, &ValidationModule::setEstimationMethod, &ValidationModule::getEstimationMethod, estMethVals);
            estimationSettings.registerInt(OT_SETTING_VALID_RANSAC_THRESHLD, module, &ValidationModule::setRansacThreshold, &ValidationModule::setRansacThreshold, 1, 9);
            estimationSettings.registerBool(OT_SETTING_VALID_REFINE_HOMOGRAPHY, module, &ValidationModule::setRefineHomography, &ValidationModule::getRefineHomography);

            Settings validationSettings = Settings("Validation Settings");
            validationSettings.registerBool(OT_SETTING_VALID_CVT_TO_GRAY, module, &ValidationModule::setConvertToGray, &ValidationModule::getConvertToGray, true);
            validationSettings.registerBool(OT_SETTING_VALID_ALL_POINTS_OUT, module, &ValidationModule::setUseAllKeyPointsForOutput, &ValidationModule::getUseAllKeyPointsForOutput);
            validationSettings.registerString(OT_SETTING_VALID_DETECTOR, module, &ValidationModule::setDetector, &ValidationModule::getDetector, detectorVals, true);
            validationSettings.registerString(OT_SETTING_VALID_EXTRACTOR, module, &ValidationModule::setExtractor, &ValidationModule::getExtractor, extractorVals, true);
            validationSettings.registerInt(OT_SETTING_VALID_N_FEATURES, module, &ValidationModule::setMaxFeatures, &ValidationModule::getMaxFeatures, 20, 1000);
            validationSettings.registerInt(OT_SETTING_VALID_HESS_THRESHLD, module, &ValidationModule::setHessianThreshold, &ValidationModule::getHessianThreshold, 300, 500);
            validationSettings.registerInt(OT_SETTING_VALID_FAST_THRESHLD, module, &ValidationModule::setFastThreshold, &ValidationModule::getFastThreshold, -1, -1, fastThresVals);
            
            validationSettings.addCategory(filterSettings);
            validationSettings.addCategory(estimationSettings);
            
            settings.addCategory(validationSettings);
        }
        
        static void initTrackingSettings(TrackingModule* module, Settings& settings)
        {
            std::vector<int> winSizeVals; winSizeVals.push_back(11); winSizeVals.push_back(21); winSizeVals.push_back(31); winSizeVals.push_back(41);
            
            Settings terminationSettings = Settings("Stop tracking after");
            terminationSettings.registerInt(OT_SETTING_TRACK_MIN_PTS_ABS, module, &TrackingModule::setMinPointsAbsolute, &TrackingModule::getMinPointsAbsolute, MIN_HOMOGRAPHY_INPUT, 1000);
            terminationSettings.registerInt(OT_SETTING_TRACK_MIN_PTS_REL, module, &TrackingModule::setMinPointsRelative, &TrackingModule::getMinPointsRelative, 1, 99);
            terminationSettings.registerInt(OT_SETTING_TRACK_MAX_SUCC_FRAMES, module, &TrackingModule::setMaxSuccessiveFrames, &TrackingModule::getMaxSuccessiveFrames, -1, 120);
            terminationSettings.registerInt(OT_SETTING_TRACK_MAX_DELTA_TRANS, module, &TrackingModule::setMaxTransformationDelta, &TrackingModule::getMaxTransformationDelta, 10, 200);
            
            Settings flowSettings = Settings("Optical Flow Parameters");
            flowSettings.registerInt(OT_SETTING_TRACK_TERM_MAX_ITER, module, &TrackingModule::setLKTermIter, &TrackingModule::getLKTermIter, 10, 50);
            flowSettings.registerFloat(OT_SETTING_TRACK_TERM_MIN_EPS, module, &TrackingModule::setLKTermEpsilon, &TrackingModule::getLKTermEpsilon, 0.001f, 0.1f);
            flowSettings.registerInt(OT_SETTING_TRACK_LK_SEARCH_WIN, module, &TrackingModule::setLKWinSize, &TrackingModule::getLKWinSize, -1, -1, winSizeVals);
            flowSettings.registerInt(OT_SETTING_TRACK_LK_MAX_PYR_LVS, module, &TrackingModule::setLKMaxLevel, &TrackingModule::getLKMaxLevel, 0, 5);
            flowSettings.registerFloat(OT_SETTING_TRACK_LK_MIN_EIG_THRESHLD, module, &TrackingModule::setLKMinEigenThreshold, &TrackingModule::getLKMinEigenThreshold, 0.0001f, 0.01f);
            
            Settings trackingSettings = Settings("Tracking Settings");
            trackingSettings.registerBool(OT_SETTING_TRACK_ENABLED, module, &TrackingModule::setEnabled, &TrackingModule::getEnabled);
            trackingSettings.registerInt(OT_SETTING_TRACK_MAX_PTS, module, &TrackingModule::setMaxPointsAbsolute, &TrackingModule::getMaxPointsAbsolute, 20, 1000);
            trackingSettings.registerBool(OT_SETTING_TRACK_USE_SUB_PX, module, &TrackingModule::setUseSubPixels, &TrackingModule::getUseSubPixels);
            trackingSettings.registerBool(OT_SETTING_TRACK_INTERFRAME_HOMOGRAPHY, module, &TrackingModule::setCalcHomRelToFrame, &TrackingModule::getCalcHomRelToFrame);
            trackingSettings.registerBool(OT_SETTING_TRACK_FILTER_DIVERGE, module, &TrackingModule::setFilterDistortions, &TrackingModule::getFilterDistortions);
            trackingSettings.registerFloat(OT_SETTING_TRACK_DIVERGE_THRESHLD, module, &TrackingModule::setDistortionThreshold, &TrackingModule::getDistortionThreshold, 0.1f, 2.0f);
            
            trackingSettings.addCategory(terminationSettings);
            trackingSettings.addCategory(flowSettings);
            
            settings.addCategory(trackingSettings);
        }
    };
    
} // end of namespace

#endif
