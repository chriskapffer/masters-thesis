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

template<typename T>
struct Param {
    CallBackSingleArg<T>* setter;
    CallBackReturn<T>* getter;
    std::vector<T> values;
    std::string name;
    T min; T max;
    Param(const std::string& name, CallBackSingleArg<T>* setter, CallBackReturn<T>* getter)
        : name(name), setter(setter), getter(getter) {}
};

struct Settings::ParameterCollection {
    std::vector<Param<bool> > boolParams;
    std::vector<Param<int> > intParams;
    std::vector<Param<float> > floatParams;
    std::vector<Param<std::string> > stringParams;
};
    
template<class TClass>
inline void Settings::registerBool(const std::string& name, TClass owner, void (TClass::*setter)(bool), bool (TClass::*getter)() const)
{
    CallBackToMemberSingleArg<bool, TClass>* setterCallback = new CallBackToMemberSingleArg<bool, TClass>(owner, setter);
    CallBackToMemberReturn<bool, TClass>* getterCallback = new CallBackToMemberReturn<bool, TClass>(owner, getter);
    Param<bool> param = Param<bool>(name, setterCallback, getterCallback);
    _parameters->boolParams.push_back(param);
}

template<class TClass>
    inline void Settings::registerInt(const std::string& name, TClass owner, void (TClass::*setter)(int), int (TClass::*getter)() const, int minValue, int maxValue, std::vector<int> values)
{
    CallBackToMemberSingleArg<int, TClass>* setterCallback = new CallBackToMemberSingleArg<int, TClass>(owner, setter);
    CallBackToMemberReturn<int, TClass>* getterCallback = new CallBackToMemberReturn<int, TClass>(owner, getter);
    Param<int> param = Param<int>(name, setterCallback, getterCallback);
    param.min = minValue;
    param.max = maxValue;
    param.values = values;
    _parameters->intParams.push_back(param);
}

template<class TClass>
inline void Settings::registerFloat(const std::string& name, TClass owner, void (TClass::*setter)(float), float (TClass::*getter)() const, float minValue, float maxValue)
{
    CallBackToMemberSingleArg<float, TClass>* setterCallback = new CallBackToMemberSingleArg<float, TClass>(owner, setter);
    CallBackToMemberReturn<float, TClass>* getterCallback = new CallBackToMemberReturn<float, TClass>(owner, getter);
    Param<float> param = Param<float>(name, setterCallback, getterCallback);
    param.min = minValue;
    param.max = maxValue;
    _parameters->floatParams.push_back(param);
}

template<class TClass>
inline void Settings::registerString(const std::string& name, TClass owner, void (TClass::*setter)(const std::string&), std::string (TClass::*getter)() const, std::vector<std::string> values)
{
    CallBackToMemberSingleArg<std::string, TClass>* setterCallback = new CallBackToMemberSingleArg<std::string, TClass>(owner, setter);
    CallBackToMemberReturn<std::string, TClass>* getterCallback = new CallBackToMemberReturn<std::string, TClass>(owner, getter);
    Param<std::string> param = Param<std::string>(name, setterCallback, getterCallback);
    param.values = values;
    _parameters->stringParams.push_back(param);
}

} // end of namespace
    
#endif
