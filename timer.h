#pragma once

#include <Arduino.h>

class Timer
{
public:
    enum class TimeUnit
    {
        MILLIS = 0,
        MICROS
    };

    enum class Mode
    {
        AUTO = 0,
        MANUAL
    };

    enum class State
    {
        RUNNING = 0,
        STOPPED
    };

    Timer(Timer::TimeUnit timeUnit);
    Timer(uint32_t interval, Timer::TimeUnit timeUnit);

    void setInterval(uint32_t interval);
    void setMode(Timer::Mode mode);
    bool isReady();
    void start();
    void stop();
    void reset();

private:
    uint32_t _timer;
    uint32_t _interval;
    uint32_t (*_timestampFn)();
    Timer::TimeUnit _timeUnit;
    Timer::Mode _mode;
    Timer::State _state;
};