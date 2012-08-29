//
//  Profiler.h
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 22.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#ifndef CKObjectTrackerLib_Profiler_h
#define CKObjectTrackerLib_Profiler_h

#include "Singleton.h"

namespace ck {

class Profiler {

DECLARE_SINGLETON(Profiler)

public:
    void startTimer(std::string timerName);
    void stopTimer(std::string timerName);
    void clearTimer(std::string timerName);
    double elapsedTime(std::string timerName) const;

    inline bool isEnabled() const { return _enabled; }
    inline void setEnabled(bool value) { _enabled = value; }
    inline void clearAll() { _timers->clear(); }
    
    std::map<std::string, double> getCurrentTimerValues() const;
    
private:
    class Timer {
    public:
        Timer();
        void start();
        void stop();
        double elapsedTotal() const;
        
    private:
        double elapsed() const;
        
        double _lastStart;
        double _elapsedTotal;
        bool _isRunning;
    };
    
    std::map<std::string, Timer>* _timers;
    bool _enabled;
};

} // end of namespace
    
#endif
