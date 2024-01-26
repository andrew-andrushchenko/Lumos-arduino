#include "timer.h"

Timer::Timer(Timer::TimeUnit timeUnit) : _timer(0), _interval(0), _timeUnit(timeUnit), _mode(Timer::Mode::AUTO), _state(Timer::State::RUNNING)
{
    _timestampFn = _timeUnit == Timer::TimeUnit::MILLIS ? millis : micros;
}

Timer::Timer(uint32_t interval, Timer::TimeUnit timeUnit)
{
    _timeUnit = timeUnit;
    _interval = interval;
    _timestampFn = _timeUnit == Timer::TimeUnit::MILLIS ? millis : micros;
    _timer = _timestampFn();
}

void Timer::setInterval(uint32_t interval)
{
    _interval = interval;
    Timer::reset();
}

void Timer::setMode(Timer::Mode mode)
{
    _mode = mode;
}

void Timer::start()
{
    _state = Timer::State::RUNNING;
}

void Timer::stop()
{
    _state = Timer::State::STOPPED;
}

bool Timer::isReady()
{
    if (_state == Timer::State::STOPPED)
        return false;

    if ((long)_timestampFn() - _timer >= _interval)
    {
        if (_mode == Timer::Mode::AUTO)
            _timer = _timestampFn();
        return true;
    }
    else
    {
        return false;
    }
}

void Timer::reset()
{
    _timer = _timestampFn();
}