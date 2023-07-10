#pragma once

#include <chrono>
#include <functional>
#include <map>
#include <mutex>
#include <string>
#include <thread>

#include "mars_message/AppLocalAlert.hpp"
#include "mars_message/AppNotDisturbTime.hpp"

using namespace mars_message;
class TimedTask {
public:
    static TimedTask *GetTimedTask() {
        static TimedTask timed_task;
        return &timed_task;
    }

    ~TimedTask();
    void AddTask(const AppLocalAlert &alert);
    void DeleteTask(const AppLocalAlert &alert);

    void SetNotDisturbTime(const AppNotDisturbTime &not_disturb_time);
    void GetNotDisturbTime(AppNotDisturbTime &not_disturb_time);
    void SetNotDisturbSwitch(bool onoff);

private:
    TimedTask();
    bool IsInNotDisturbTime(int16_t alert_hour, int16_t alert_minute, const AppNotDisturbTime &not_disturb_time);
    bool IsTimeToTrigger(int16_t alert_day, int16_t alert_hour, int16_t alert_minute);

private:
    std::thread check_thread_;
    bool b_exit_ = false;
    std::mutex timed_task_map_mutex_;
    AppLocalAlert timed_task_;
    std::mutex not_disturb_time_mutex_;
    AppNotDisturbTime not_disturb_time_;
    bool last_not_disturb_status_ = false;
    int64_t start_time_;
    bool b_run_in_one_minute_ = false;
};