//
//  Profiler.cpp
//  CKObjectTrackerLib
//
//  Created by Christoph Kapffer on 22.08.12.
//  Copyright (c) 2012 HTW Berlin. All rights reserved.
//

#include "Profiler.h"

using namespace std;

namespace ck {

DEFINE_SINGLETON(Profiler)

Profiler::Profiler()
{
    _timers = new map<string, Timer>();
    _enabled = true;
}

Profiler::~Profiler()
{
    delete _timers;
}

void Profiler::startTimer(std::string timerName)
{
    if (!_enabled)
        return;
    
    map<string, Timer>::iterator it = _timers->find(timerName);
    if (it == _timers->end()) {
        _timers->insert(map<string, Timer>::value_type(timerName, Timer()));
    }
    else {
        it->second.start();
    }
}

void Profiler::stopTimer(std::string timerName)
{
    if (!_enabled)
        return;
    
    map<string, Timer>::iterator it = _timers->find(timerName);
    if (it != _timers->end()) {
        it->second.stop();
    }
}

void Profiler::clearTimer(std::string timerName)
{
    if (!_enabled)
        return;
    
    map<string, Timer>::iterator it = _timers->find(timerName);
    if (it != _timers->end()) {
        _timers->erase(it);
    }
}

double Profiler::elapsedTime(std::string timerName) const
{
    map<string, Timer>::iterator it = _timers->find(timerName);
    if (it != _timers->end()) {
        return it->second.elapsedTotal();
    }
    return 0;
}

map<string, double> Profiler::getCurrentTimerValues() const
{
    map<string, double> result;
    map<string, Timer>::const_iterator iter;
    for (iter = _timers->begin(); iter != _timers->end(); iter++) {
        result[(*iter).first] = (*iter).second.elapsedTotal();
    }
    return result;
}
    
//---------------------------------------------------------------

Profiler::Timer::Timer()
{
    _elapsedTotal = 0;
    _isRunning = false;
    start();
}

void Profiler::Timer::start()
{
    if (_isRunning)
        return;
    
    _lastStart = (double)cv::getTickCount();
    _isRunning = true;
}

void Profiler::Timer::stop()
{
    if (!_isRunning)
        return;
    
    _elapsedTotal += elapsed();
    _isRunning = false;
}

double Profiler::Timer::elapsed() const
{
    return ((double)cv::getTickCount() - _lastStart) / cv::getTickFrequency() * 1000;
}

double Profiler::Timer::elapsedTotal() const
{
    float result = _elapsedTotal;
    if (_isRunning) {
        result += elapsed();
    }
    return result;
}
    
} // end of namespace
