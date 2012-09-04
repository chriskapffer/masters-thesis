//
//  Settings.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 03.09.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_SettingsPublicScope_h
#define CKObjectTrackerLib_SettingsPublicScope_h

namespace ck {
    
    enum Type {
        TYPE_BOOL,
        TYPE_INT,
        TYPE_FLOAT,
        TYPE_STRING,
    };
    
    // WHATCH OUT!! You need to include settingsTemplates.cpp somewhere and declare instantiations
    // for each consumer of the templates in order to avoid linker errors.
    class Settings {
    public:
        Settings();
        ~Settings();
        
        template<class TClass>
        void registerBool(const std::string& name, TClass owner,
                          void (TClass::*setter)(bool),
                          bool (TClass::*getter)() const);
        
        template<class TClass>
        void registerInt(const std::string& name, TClass owner,
                         void (TClass::*setter)(int),
                         int (TClass::*getter)() const,
                         int minValue, int maxValue,
                         std::vector<int> values);
        
        template<class TClass>
        void registerFloat(const std::string& name, TClass owner,
                           void (TClass::*setter)(float),
                           float (TClass::*getter)() const,
                           float minValue, float maxValue);
        
        template<class TClass>
        void registerString(const std::string& name, TClass owner,
                            void (TClass::*setter)(const std::string&),
                            std::string (TClass::*getter)() const,
                            std::vector<std::string> values);
        
        void setBoolValue(const std::string& name, bool value) const;
        void setIntValue(const std::string& name, int value) const;
        void setFloatValue(const std::string& name, float value) const;
        void setStringValue(const std::string& name, std::string value) const;
        
        bool getBoolValue(const std::string& name) const;
        int getIntValue(const std::string& name) const;
        float getFloatValue(const std::string& name) const;
        std::string getStringValue(const std::string& name) const;
        
        void getIntInfo(const std::string& name, int& minValue, int& maxValue, std::vector<int>& values) const;
        void getFloatInfo(const std::string& name, float& minValue, float& maxValue) const;
        void getIntInfo(const std::string& name, std::vector<std::string>& values) const;
        
        Type getParameterType(const std::string& name) const;
        const std::vector<std::string> getParameterNames() const;
        
        void addCategory(const Settings& category);
        const Settings getSubCategory(std::string categoryName) const;
        const std::vector<Settings> getSubCategories() const;
        
    private:
        struct ParameterCollection;
        ParameterCollection* _parameters;
        std::vector<Settings> _subCategories;
        std::string _name;
    };
    
} // end of namespace

#endif
