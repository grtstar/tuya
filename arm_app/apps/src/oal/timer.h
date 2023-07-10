#pragma once

#include "time_tick.h"

class Timer
{
    int64_t delay;
    int64_t kickoff; // ms
    int repeat;

public:
    Timer(int ms)
    {
        Set(ms);
    }
    void Set(int ms)
    {
        kickoff = TimeTick::Ms();
        delay = ms;
    };

    void Reset()
    {
        kickoff = 0;
    }

    bool IsSet()
    {
        return kickoff != 0;
    }

    void Recall()
    {
        kickoff = TimeTick::Ms();
    }

    bool IsTimeOut()
    {
        if (kickoff == 0)
        {
            return false;
        }
        if ((int64_t)(TimeTick::Ms() - kickoff) > (int64_t)delay)
        {
            kickoff = 0;
            return true;
        }
        return false;
    }
};