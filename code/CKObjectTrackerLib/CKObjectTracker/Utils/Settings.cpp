//
//  Settings.cpp
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 03.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#include "SettingsProjectScope.h"
#include "Callback.h"

using namespace std;

namespace ck {

    template<typename T>
    struct Param {
        CallBackSingleArg<T>* setter;
        CallBackReturn<T>* getter;
        std::vector<T> values;
        std::string name;
        T min; T max;
    };
    
    struct Settings::ParameterCollection {
        vector<Param<bool> > boolParams;
        vector<Param<int> > intParams;
        vector<Param<float> > floatParams;
        vector<Param<std::string> > stringParams;
    };
    
    Settings::Settings()
    {
        _parameters = new ParameterCollection();
    }
    
    Settings::~Settings()
    {
        delete _parameters;
    }
    
#pragma mark

    void Settings::setBoolValue(const std::string& name, bool value) const
    {
        
    }
    
    void Settings::setIntValue(const std::string& name, int value) const
    {
        
    }
    
    void Settings::setFloatValue(const std::string& name, float value) const
    {
        
    }
    
    void Settings::setStringValue(const std::string& name, std::string value) const
    {
        
    }
    
    bool Settings::getBoolValue(const std::string& name) const
    {
        return false;
    }
    
    int Settings::getIntValue(const std::string& name) const
    {
        return 0;
    }
    
    float Settings::getFloatValue(const std::string& name) const
    {
        return 0;
    }
    
    std::string Settings::getStringValue(const std::string& name) const
    {
        return string();
    }
    
#pragma mark
    
    void Settings::getIntInfo(const std::string& name, int& minValue, int& maxValue, vector<int>& values) const
    {
        
    }
    
    void Settings::getFloatInfo(const std::string& name, float& minValue, float& maxValue) const
    {
        
    }
    
    void Settings::getIntInfo(const std::string& name, std::vector<std::string>& values) const
    {
        
    }
    
#pragma mark
    
    Type Settings::getParameterType(const std::string& name) const
    {
        return TYPE_BOOL;
    }
    
    const std::vector<std::string> Settings::getParameterNames() const
    {
        return vector<string>();
    }
    
    void Settings::addCategory(const Settings& category)
    {
        
    }
    
    const Settings Settings::getSubCategory(std::string categoryName) const
    {
        return Settings();
    }
    
    const std::vector<Settings> Settings::getSubCategories() const
    {
        return vector<Settings>();
    }
    
} // end of namespace