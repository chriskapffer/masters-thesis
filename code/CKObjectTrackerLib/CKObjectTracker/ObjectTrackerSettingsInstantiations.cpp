//
//  ObjectTrackerSettingsInstantiations.cpp
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 04.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#include "DetectionModule.h"
#include "ValidationModule.h"
#include "TrackingModule.h"

#include "Utils/Settings.cpp"

namespace ck {
    
template void Settings::registerBool(const std::string& name, DetectionModule owner, void (DetectionModule::*setter)(const bool&), bool (DetectionModule::*getter)() const);
template void Settings::registerBool(const std::string& name, ValidationModule owner, void (ValidationModule::*setter)(const bool&), bool (ValidationModule::*getter)() const);
template void Settings::registerBool(const std::string& name, TrackingModule owner, void (TrackingModule::*setter)(const bool&), bool (TrackingModule::*getter)() const);
    
template void Settings::registerInt(const std::string& name, DetectionModule owner, void (DetectionModule::*setter)(const int&), int (DetectionModule::*getter)() const, int minValue, int maxValue, std::vector<int> values);
template void Settings::registerInt(const std::string& name, ValidationModule owner, void (ValidationModule::*setter)(const int&), int (ValidationModule::*getter)() const, int minValue, int maxValue, std::vector<int> values);
template void Settings::registerInt(const std::string& name, TrackingModule owner, void (TrackingModule::*setter)(const int&), int (TrackingModule::*getter)() const, int minValue, int maxValue, std::vector<int> values);

template void Settings::registerFloat(const std::string &name, DetectionModule owner, void (DetectionModule::*setter)(float), float (DetectionModule::*getter)() const, float minValue, float maxValue);
template void Settings::registerFloat(const std::string &name, ValidationModule owner, void (ValidationModule::*setter)(float), float (ValidationModule::*getter)() const, float minValue, float maxValue);
template void Settings::registerFloat(const std::string &name, TrackingModule owner, void (TrackingModule::*setter)(float), float (TrackingModule::*getter)() const, float minValue, float maxValue);
    
template void Settings::registerString(const std::string& name, DetectionModule owner, void (DetectionModule::*setter)(const std::string&), std::string (DetectionModule::*getter)() const, std::vector<std::string> values);
template void Settings::registerString(const std::string& name, ValidationModule owner, void (ValidationModule::*setter)(const std::string&), std::string (ValidationModule::*getter)() const, std::vector<std::string> values);
template void Settings::registerString(const std::string& name, TrackingModule owner, void (TrackingModule::*setter)(const std::string&), std::string (TrackingModule::*getter)() const, std::vector<std::string> values);
}

