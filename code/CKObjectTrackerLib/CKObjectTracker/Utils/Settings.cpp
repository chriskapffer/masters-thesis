//
//  Settings.cpp
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 03.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#include "SettingsProjectScope.h"

using namespace std;

namespace ck {

    template<typename T>
    static std::vector<std::string> getParameterNamesFromCollection(const vector<Param<T> >& params)
    {
        vector<string> result;
        typename vector<Param<T> >::const_iterator iter;
        for (iter = params.begin(); iter != params.end(); iter++) {
            result.push_back((*iter).name);
        }
        return result;
    }
    
    template<typename T>
    static bool getParamterWithName(const string& name, const vector<Param<T> >& params, Param<T>& parameter)
    {
        typename vector<Param<T> >::const_iterator iter;
        for (iter = params.begin(); iter != params.end(); iter++) {
            if ((*iter).name == name) {
                parameter.setter = (*iter).setter->clone();
                parameter.getter = (*iter).getter->clone();
                parameter.values = (*iter).values;
                parameter.name = (*iter).name;
                parameter.min = (*iter).min;
                parameter.max = (*iter).max;
                return true;
            }
        }
        return false;
    }
    
    template<typename T>
    static bool containsParamterWithName(const string& name, const vector<Param<T> >& params)
    {
        typename vector<Param<T> >::const_iterator iter;
        for (iter = params.begin(); iter != params.end(); iter++) {
            if ((*iter).name == name)
                return true;
        }
        return false;
    }
    
    Settings::Settings(const string& name)
    {
        _parameters = new ParameterCollection();
        _name = name;
    }
    
    Settings::Settings(const Settings& other)
    {
        _name = other._name;
        _subCategories = other._subCategories;
        _parameters = new ParameterCollection(*other._parameters);
    }
    
    Settings::~Settings()
    {
        delete _parameters;
    }
    
#pragma mark

    bool Settings::setBoolValue(const std::string& name, const bool& value) const
    {
        Param<bool> param;
        if (getParamterWithName(name, _parameters->boolParams, param)) {
            param.setter->call(value);
            return true;
        }
        // search sub categories if not found
        vector<Settings>::const_iterator iter;
        for (iter = _subCategories.begin(); iter != _subCategories.end(); iter++) {
            if ((*iter).setBoolValue(name, value)) {
                return true;
            }
        }
        return false;
    }
    
    bool Settings::setIntValue(const std::string& name, const int& value) const
    {
        Param<int> param;
        if (getParamterWithName(name, _parameters->intParams, param)) {
            param.setter->call(value);
            return true;
        }
        // search sub categories if not found
        vector<Settings>::const_iterator iter;
        for (iter = _subCategories.begin(); iter != _subCategories.end(); iter++) {
            if ((*iter).setIntValue(name, value)) {
                return true;
            }
        }
        return false;
    }
    
    bool Settings::setFloatValue(const std::string& name, const float& value) const
    {
        Param<float> param;
        if (getParamterWithName(name, _parameters->floatParams, param)) {
            param.setter->call(value);
            return true;
        }
        // search sub categories if not found
        vector<Settings>::const_iterator iter;
        for (iter = _subCategories.begin(); iter != _subCategories.end(); iter++) {
            if ((*iter).setFloatValue(name, value)) {
                return true;
            }
        }
        return false;
    }
    
    bool Settings::setStringValue(const std::string& name, const std::string& value) const
    {
        Param<string> param;
        if (getParamterWithName(name, _parameters->stringParams, param)) {
            param.setter->call(value);
            return true;
        }
        // search sub categories if not found
        vector<Settings>::const_iterator iter;
        for (iter = _subCategories.begin(); iter != _subCategories.end(); iter++) {
            if ((*iter).setStringValue(name, value)) {
                return true;
            }
        }
        return false;
    }
    
    bool Settings::getBoolValue(const std::string& name, bool& value) const
    {
        Param<bool> param;
        if (getParamterWithName(name, _parameters->boolParams, param)) {
            value = param.getter->call();
            return true;
        }
        // search sub categories if not found
        vector<Settings>::const_iterator iter;
        for (iter = _subCategories.begin(); iter != _subCategories.end(); iter++) {
            if ((*iter).getBoolValue(name, value)) {
                return true;
            }
        }
        return false;
    }
    
    bool Settings::getIntValue(const std::string& name, int& value) const
    {
        Param<int> param;
        if (getParamterWithName(name, _parameters->intParams, param)) {
            value = param.getter->call();
            return true;
        }
        // search sub categories if not found
        vector<Settings>::const_iterator iter;
        for (iter = _subCategories.begin(); iter != _subCategories.end(); iter++) {
            if ((*iter).getIntValue(name, value)) {
                return true;
            }
        }
        return false;
    }
    
    bool Settings::getFloatValue(const std::string& name, float& value) const
    {
        Param<float> param;
        if (getParamterWithName(name, _parameters->floatParams, param)) {
            value = param.getter->call();
            return true;
        }
        // search sub categories if not found
        vector<Settings>::const_iterator iter;
        for (iter = _subCategories.begin(); iter != _subCategories.end(); iter++) {
            if ((*iter).getFloatValue(name, value)) {
                return true;
            }
        }
        return false;
    }
    
    bool Settings::getStringValue(const std::string& name, std::string& value) const
    {
        Param<string> param;
        if (getParamterWithName(name, _parameters->stringParams, param)) {
            value = param.getter->call();
            return true;
        }
        // search sub categories if not found
        vector<Settings>::const_iterator iter;
        for (iter = _subCategories.begin(); iter != _subCategories.end(); iter++) {
            if ((*iter).getStringValue(name, value)) {
                return true;
            }
        }
        return false;
    }
    
#pragma mark
    
    bool Settings::getBoolInfo(const std::string &name, bool &isCritical) const
    {
        Param<string> param;
        if (getParamterWithName(name, _parameters->stringParams, param)) {
            isCritical = param.critical;
            return true;
        }
        // search sub categories if not found
        vector<Settings>::const_iterator iter;
        for (iter = _subCategories.begin(); iter != _subCategories.end(); iter++) {
            if ((*iter).getBoolInfo(name, isCritical)) {
                return true;
            }
        }
        return false;
    }
    
    bool Settings::getIntInfo(const std::string& name, int& minValue, int& maxValue, vector<int>& values, bool& isCritical) const
    {
        Param<int> param;
        if (getParamterWithName(name, _parameters->intParams, param)) {
            isCritical = param.critical;
            minValue = param.min;
            maxValue = param.max;
            values = param.values;
            return true;
        }
        // search sub categories if not found
        vector<Settings>::const_iterator iter;
        for (iter = _subCategories.begin(); iter != _subCategories.end(); iter++) {
            if ((*iter).getIntInfo(name, minValue, maxValue, values, isCritical)) {
                return true;
            }
        }
        return false;
    }

    bool Settings::getFloatInfo(const std::string& name, float& minValue, float& maxValue, bool& isCritical) const
    {
        Param<float> param;
        if (getParamterWithName(name, _parameters->floatParams, param)) {
            isCritical = param.critical;
            minValue = param.min;
            maxValue = param.max;
            return true;
        }
        // search sub categories if not found
        vector<Settings>::const_iterator iter;
        for (iter = _subCategories.begin(); iter != _subCategories.end(); iter++) {
            if ((*iter).getFloatInfo(name, minValue, maxValue, isCritical)) {
                return true;
            }
        }
        return false;
    }
    
    bool Settings::getStringInfo(const std::string& name, std::vector<std::string>& values, bool& isCritical) const
    {
        Param<string> param;
        if (getParamterWithName(name, _parameters->stringParams, param)) {
            isCritical = param.critical;
            values = param.values;
            return true;
        }
        // search sub categories if not found
        vector<Settings>::const_iterator iter;
        for (iter = _subCategories.begin(); iter != _subCategories.end(); iter++) {
            if ((*iter).getStringInfo(name, values, isCritical)) {
                return true;
            }
        }
        return false;
    }
    
#pragma mark
    
    void Settings::addCategory(const Settings& category)
    {
        _subCategories.push_back(category);
    }
    
    const std::vector<Settings> Settings::getSubCategories() const
    {
        return _subCategories;
    }
        
    const std::vector<std::string> Settings::getParameterNames() const
    {
        vector<string> tmp;
        vector<string> result;
        tmp = getParameterNamesFromCollection(_parameters->boolParams);
        result.insert(result.begin(), tmp.begin(), tmp.end());
        tmp = getParameterNamesFromCollection(_parameters->intParams);
        result.insert(result.begin(), tmp.begin(), tmp.end());
        tmp = getParameterNamesFromCollection(_parameters->floatParams);
        result.insert(result.begin(), tmp.begin(), tmp.end());
        tmp = getParameterNamesFromCollection(_parameters->stringParams);
        result.insert(result.begin(), tmp.begin(), tmp.end());
        return result;
    }
        
    bool Settings::getParameterType(const std::string& parameterName, Type& type) const
    {
        if (containsParamterWithName(parameterName, _parameters->boolParams)) {
            type = TYPE_BOOL;
        } else if (containsParamterWithName(parameterName, _parameters->intParams)) {
            type = TYPE_INT;
        } else if (containsParamterWithName(parameterName, _parameters->floatParams)) {
            type = TYPE_FLOAT;
        } else if (containsParamterWithName(parameterName, _parameters->stringParams)) {
            type = TYPE_STRING;
        } else {
            return false;
        }
        return true;
    }
    
    bool Settings::getSubCategory(const std::string& categoryName, Settings& subCategory) const
    {
        // breadth first search
        vector<Settings>::const_iterator iter;
        for (iter = _subCategories.begin(); iter != _subCategories.end(); iter++) {
            if ((*iter).getName() == categoryName) {
                subCategory = (*iter);
                return true;
            }
        }
        // not found yet, look into sub categories
        for (iter = _subCategories.begin(); iter != _subCategories.end(); iter++) {
            if ((*iter).getSubCategory(categoryName, subCategory)) {
                return true;
            }
        }
        return false;
    }
    
} // end of namespace