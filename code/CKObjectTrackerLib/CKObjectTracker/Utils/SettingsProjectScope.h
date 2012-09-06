//
//  SettingsProjectScope.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 05.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_SettingsProjectScope_h
#define CKObjectTrackerLib_SettingsProjectScope_h

#include "SettingsPublicScope.h"
#include "Callback.h"

namespace ck {

template<typename TParam>
struct Param {
    std::string name;
    CallBackSingleArg<TParam>* setter;
    CallBackReturn<TParam>* getter;
    std::vector<TParam> values;
    TParam min; TParam max;
    bool critical;
    
    Param() {
        critical = false;
        setter = NULL;
        getter = NULL;
        name = "";
    }
    
    Param(const Param& other) {
        setter = other.setter->clone();
        getter = other.getter->clone();
        critical = other.critical;
        values = other.values;
        name = other.name;
        min = other.min;
        max = other.max;
    }
    
    template <typename TClass>
    Param(const std::string& name, TClass* owner, void (TClass::*setter)(const TParam&), TParam (TClass::*getter)() const, bool critical = false) {
        this->setter = new CallBackToMemberSingleArg<TParam, TClass>(owner, setter);
        this->getter = new CallBackToMemberReturn<TParam, TClass>(owner, getter);
        this->critical = critical;
        this->name = name;
    }
    
    ~Param() {
        if (setter != NULL) { delete setter; }
        if (getter != NULL) { delete getter; }
    }
};

struct Settings::ParameterCollection {
    std::vector<Param<bool> > boolParams;
    std::vector<Param<int> > intParams;
    std::vector<Param<float> > floatParams;
    std::vector<Param<std::string> > stringParams;
    
    ParameterCollection() { }
    
    ParameterCollection(const ParameterCollection& other) {
        boolParams = other.boolParams;
        intParams = other.intParams;
        floatParams = other.floatParams;
        stringParams = other.stringParams;
    }
};
    
template<class TClass>
    inline void Settings::registerBool(const std::string& name, TClass* owner, void (TClass::*setter)(const bool&), bool (TClass::*getter)() const, bool critical)
{
    Param<bool> param = Param<bool>(name, owner, setter, getter, critical);
    _parameters->boolParams.push_back(param);
}

template<class TClass>
inline void Settings::registerInt(const std::string& name, TClass* owner, void (TClass::*setter)(const int&), int (TClass::*getter)() const, int minValue, int maxValue, bool critical)
{
    std::vector<int> dummy;
    registerInt(name, owner, setter, getter, minValue, maxValue, dummy, critical);
}
    
template<class TClass>
    inline void Settings::registerInt(const std::string& name, TClass* owner, void (TClass::*setter)(const int&), int (TClass::*getter)() const, int minValue, int maxValue, std::vector<int> values, bool critical)
{
    Param<int> param = Param<int>(name, owner, setter, getter, critical);
    param.min = minValue;
    param.max = maxValue;
    param.values = values;
    _parameters->intParams.push_back(param);
}

template<class TClass>
inline void Settings::registerFloat(const std::string& name, TClass* owner, void (TClass::*setter)(const float&), float (TClass::*getter)() const, float minValue, float maxValue, bool critical)
{
    Param<float> param = Param<float>(name, owner, setter, getter, critical);
    param.min = minValue;
    param.max = maxValue;
    _parameters->floatParams.push_back(param);
}

template<class TClass>
inline void Settings::registerString(const std::string& name, TClass* owner, void (TClass::*setter)(const std::string&), std::string (TClass::*getter)() const, std::vector<std::string> values, bool critical)
{
    Param<std::string> param = Param<std::string>(name, owner, setter, getter, critical);
    param.values = values;
    _parameters->stringParams.push_back(param);
}

} // end of namespace
    
#endif
