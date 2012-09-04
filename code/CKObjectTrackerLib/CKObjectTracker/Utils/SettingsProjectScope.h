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

namespace ck {

template<class TClass>
inline void Settings::registerBool(const std::string& name, TClass owner, void (TClass::*setter)(bool), bool (TClass::*getter)() const)
{
    
}

template<class TClass>
    inline void Settings::registerInt(const std::string& name, TClass owner, void (TClass::*setter)(int), int (TClass::*getter)() const, int minValue, int maxValue, std::vector<int> values)
{
    
}

template<class TClass>
inline void Settings::registerFloat(const std::string& name, TClass owner, void (TClass::*setter)(float), float (TClass::*getter)() const, float minValue, float maxValue)
{
    
}

template<class TClass>
inline void Settings::registerString(const std::string& name, TClass owner, void (TClass::*setter)(const std::string&), std::string (TClass::*getter)() const, std::vector<std::string> values)
{
    
}

} // end of namespace
    
#endif
