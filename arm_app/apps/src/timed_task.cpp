#include "timed_task.hpp"
#include "utils/log_.h"
#include "tuya_robot.h"
#include "tuya_dp_function.h"
#include "mars_message/Range.hpp"

#undef TAG
#define TAG "TK"

using namespace mars_message;

TimedTask::TimedTask()
{   
    not_disturb_time_.version = -1;
    timed_task_.verison = -1;
    start_time_ = TimeTick::Ms();

    check_thread_ = std::thread([this]() 
    {
        while (!b_exit_) 
        {
            
            int16_t dis_start_hour;
            int16_t dis_start_minute;
            int16_t dis_end_hour;
            int16_t dis_end_minute;

            {
                std::lock_guard<std::mutex> lock2(not_disturb_time_mutex_);
                if (not_disturb_time_.version >= 0)
                {
                    dis_start_hour = not_disturb_time_.starTime >> 8 & 0xFF;
                    dis_start_minute = not_disturb_time_.starTime & 0xFF;
                    dis_end_hour = not_disturb_time_.endTime >> 8 & 0xFF;
                    dis_end_minute = not_disturb_time_.endTime & 0xFF;

                    time_t timep;
                    time(&timep);
                    struct tm *local_time = localtime(&timep);
                    int16_t hour = local_time->tm_hour;
                    int16_t minute = local_time->tm_min;
                    bool in_not_disturb_time = false;
                    if (not_disturb_time_.version >= 0)
                    {
                        bool is_not_disturb = IsInNotDisturbTime(hour, minute, not_disturb_time_);
                        if (not_disturb_time_.onoff == 1 && is_not_disturb && !last_not_disturb_status_)
                        {
                            //开启勿扰模式,关闭声音
                            bool on = false;
                            Event msg;
                            msg.param = 0x01;
                            TuyaComm::Get()->Send("NotDisturbMode", &msg);
                            last_not_disturb_status_ = true;
                        }

                        if((!is_not_disturb || not_disturb_time_.onoff != 1) && last_not_disturb_status_)
                        {
                            //关闭勿扰模式,打开声音
                            Event msg;
                            msg.param = 0x00;
                            TuyaComm::Get()->Send("NotDisturbMode", &msg);
                            last_not_disturb_status_ = false;
                        }
                    }
                }

                std::lock_guard<std::mutex> lock1(timed_task_map_mutex_);
                if (timed_task_.verison >= 0)
                {
                    for (auto &task_info : timed_task_.timedTaskInfo) 
                    {
                        int16_t hour = (task_info.time >> 8 & 0xFF);
                        int16_t minute = task_info.time & 0xFF;
                        //判断定时任务是否在勿扰时间段
                        bool in_not_disturb_time = false;
                        if (not_disturb_time_.version >= 0 && not_disturb_time_.onoff == 1)
                        {
                            in_not_disturb_time = IsInNotDisturbTime(hour, minute, not_disturb_time_);
                        }
                        if (in_not_disturb_time || task_info.valid != 1)
                        {   
                            //在勿扰时间段或者任务没开启, 跳过本次任务
                            // LOGD(TAG, "跳过本次任务");
                            continue;
                        }

                        //判断任务是否到触发时间
                        bool is_trigger = IsTimeToTrigger(task_info.weekDayMask, hour, minute);
                        if(is_trigger)
                        {
                            //通知执行任务
                            LOGD(TAG, "定时任务触发");
                            if (!b_run_in_one_minute_)
                            {   
                                TuyaComm::Get()->Send("START_TASK", &task_info);
                                b_run_in_one_minute_ = true;
                                start_time_ = TimeTick::Ms();
                            }
                            else
                            {
                              LOGD(TAG, "一分钟内已经执行过一次任务");
                            }
                        }

                        //单次任务执行完需要手动失效掉
                        if(is_trigger && task_info.weekDayMask == 0 && task_info.valid == 1)
                        {
                            task_info.valid = 0;
                            if (TuyaComm::Get()->Send("AC_LocalAlert", &timed_task_))
                            {
                                TuyaReportLocalAlert(32, timed_task_.verison == 0 ? 0x31 : 0x45, &timed_task_);
                            }
                        }
                    }
                }
            }
            sleep(10);
            if (TimeTick::Ms() - start_time_ > 60000)
            {
                b_run_in_one_minute_ = false;
                start_time_ = TimeTick::Ms();
            }
        }
    });
}

TimedTask::~TimedTask() 
{
    b_exit_ = true;
    if (check_thread_.joinable()) 
    {
        check_thread_.join();
    }
}

void TimedTask::AddTask(const AppLocalAlert &alert)
{
    std::lock_guard<std::mutex> lock(timed_task_map_mutex_);
    timed_task_ = alert;
    LOGD(TAG, "-----------------------定时任务信息---------------------------");
    LOGD(TAG, "版本: {}", timed_task_.verison == 0 ? "V1.0.0" : "V1.1.0");
    LOGD(TAG, "手机系统时区(补码) : {}", timed_task_.timeZone);
    LOGD(TAG, "定时条数: {}", timed_task_.number);
    for (auto &task_info : timed_task_.timedTaskInfo) 
    {
        LOGD(TAG, "定时是否有效: {}", task_info.valid == 1 ? "生效" : "失效");
        std::string days;
        if (task_info.weekDayMask & 0x01) 
        {
            days += " 周一 ";
        }
        if (task_info.weekDayMask & 0x02) 
        {
            days += " 周二 ";
        }
        if (task_info.weekDayMask & 0x04) 
        {
            days += " 周三 ";
        }
        if (task_info.weekDayMask & 0x08) 
        {
            days += " 周四 ";
        }
        if (task_info.weekDayMask & 0x10) 
        {
            days += " 周五 ";
        }
        if (task_info.weekDayMask & 0x20) 
        {
            days += " 周六 ";
        }
        if (task_info.weekDayMask & 0x40) 
        {
            days += " 周日 ";
        }
        if (task_info.weekDayMask == 0) 
        {
            days += " 仅执行一次 ";
        }
        LOGD(TAG, "执行星期: {}", days);

        int16_t hour = (task_info.time >> 8 & 0xFF);
        int16_t minute = task_info.time & 0xFF;
        LOGD(TAG, "执行时间: {}:{}", hour, minute);
        LOGD(TAG, "清扫位置: {}",task_info.roomN == 0 ? "全屋清扫": "选择房间清扫");
        LOGD(TAG, "房间个数: {}", task_info.roomN);
        for (auto &id : task_info.roomId) 
        {
            LOGD(TAG, "房间标识: {}", id);
        }

        if (task_info.cleanMode == 0x00) 
        {
            LOGD(TAG, "工作模式: 扫拖");
        } else if (task_info.cleanMode == 0x01) 
        {
            LOGD(TAG, "工作模式: 仅扫");
        } else if (task_info.cleanMode == 0x02) 
        {
            LOGD(TAG, "工作模式: 仅拖");
        }
        LOGD(TAG, "风机档位: {}", task_info.fan);
        LOGD(TAG, "水箱档位: {}", task_info.water);
        LOGD(TAG, "扫地清扫次数: {}", task_info.cleanRepeat);
        LOGD(TAG, "-------------------------------------------------------");
    }
}

void TimedTask::DeleteTask(const AppLocalAlert &alert)
{
    timed_task_.verison = -1;
}

void TimedTask::SetNotDisturbTime(const AppNotDisturbTime &not_disturb_time)
{
    std::lock_guard<std::mutex> lock(not_disturb_time_mutex_);
    not_disturb_time_ = not_disturb_time;
    int16_t dis_start_hour;
    int16_t dis_start_minute;
    int16_t dis_end_hour;
    int16_t dis_end_minute;
    if (not_disturb_time_.version >= 0)
    {
        LOGD(TAG, "-----------------------勿扰时间信息---------------------------");
        LOGD(TAG, "版本: {}", not_disturb_time_.version == 0 ? "V1.0.0" : "V1.1.0");
        LOGD(TAG, "勿扰开关: {}", not_disturb_time_.onoff == 1 ? "开启" : "关闭");
        LOGD(TAG, "手机系统时区(补码) : {}", not_disturb_time_.timeZone);
        dis_start_hour = not_disturb_time_.starTime >> 8 & 0xFF;
        dis_start_minute = not_disturb_time_.starTime & 0xFF;
        LOGD(TAG, "开始时间: {}:{}", dis_start_hour, dis_start_minute);
        dis_end_hour = not_disturb_time_.endTime >> 8 & 0xFF;
        dis_end_minute = not_disturb_time_.endTime & 0xFF;
        LOGD(TAG, "结束时间: {}:{}", dis_end_hour, dis_end_minute);
        LOGD(TAG, "开始时间为当天?: {}", not_disturb_time_.startDay == 0x00 ? "当天" : "第二天");
        LOGD(TAG, "结束时间为当天?: {}", not_disturb_time_.endDay == 0x00 ? "当天" : "第二天");
    }
}

void TimedTask::GetNotDisturbTime(AppNotDisturbTime &not_disturb_time) 
{
    std::lock_guard<std::mutex> lock(not_disturb_time_mutex_);
    not_disturb_time = not_disturb_time_;
}

void TimedTask::SetNotDisturbSwitch(bool onoff) 
{
    std::lock_guard<std::mutex> lock(not_disturb_time_mutex_);
    if (not_disturb_time_.version >= 0)
    {
        not_disturb_time_.onoff = (onoff ? 0x01 : 0x00);
    }
}

bool TimedTask::IsInNotDisturbTime(int16_t alert_hour, int16_t alert_minute, const AppNotDisturbTime &not_disturb_time)
{
    int16_t dis_start_hour = not_disturb_time_.starTime >> 8 & 0xFF;
    int16_t dis_start_minute = not_disturb_time_.starTime & 0xFF;
    int16_t dis_end_hour = not_disturb_time_.endTime >> 8 & 0xFF;
    int16_t dis_end_minute = not_disturb_time_.endTime & 0xFF;

    if (not_disturb_time.endDay == 0x01)
    {
        //勿扰时间段跨天
        if ((alert_hour > dis_start_hour ||
             (alert_hour == dis_start_hour &&
              alert_minute >= dis_start_minute)) ||
            (alert_hour < dis_end_hour ||
             (alert_hour == dis_end_hour && alert_minute <= dis_end_minute))) 
        {
            // LOGD(TAG, "定时任务{}:{} 在勿扰时间{}:{} ~ {}:{}内", alert_hour,
            //     alert_minute, dis_start_hour, dis_start_minute, dis_end_hour,
            //     dis_end_minute);
            return true;
        }
    }
    else
    {
        //勿扰时间段非跨天
        if ((alert_hour > dis_start_hour && alert_hour < dis_end_hour) ||
            (alert_hour == dis_start_hour &&
             alert_minute >= dis_start_minute) ||
            (alert_hour == dis_end_hour && alert_minute <= dis_end_minute)) 
        {
            // LOGD(TAG, "定时任务{}:{} 在勿扰时间{}:{} ~ {}:{}内", alert_hour,
            //     alert_minute, dis_start_hour, dis_start_minute, dis_end_hour,
            //     dis_end_minute);
            return true;
        }
    }

    // LOGD(TAG, "定时任务{}:{} 不在勿扰时间{}:{} ~ {}:{}内", alert_hour,
    //     alert_minute, dis_start_hour, dis_start_minute, dis_end_hour,
    //     dis_end_minute);
    return false;
}

bool TimedTask::IsTimeToTrigger(int16_t week_day_mask, int16_t alert_hour, int16_t alert_minute)
{
    time_t timep;
    time(&timep);
    struct tm *local_time = localtime(&timep);
    // tm_wday 一周中的第几天，范围从 0 到 6
    // tm_hour 小时，范围从 0 到 23
    // tm_min  分，范围从 0 到 59
    // LOGD(TAG, "当地时间:{}日 {}:{}", local_time->tm_wday, local_time->tm_hour, local_time->tm_min);

    std::string days;
    bool is_same_day = false;
    bool is_once = false;
    if (((week_day_mask & 0x01) && local_time->tm_wday == 1) ||
        ((week_day_mask & 0x02) && local_time->tm_wday == 2) ||
        ((week_day_mask & 0x04) && local_time->tm_wday == 3) ||
        ((week_day_mask & 0x08) && local_time->tm_wday == 4) ||
        ((week_day_mask & 0x10) && local_time->tm_wday == 5) ||
        ((week_day_mask & 0x20) && local_time->tm_wday == 6) ||
        ((week_day_mask & 0x40) && local_time->tm_wday == 0))
    {
        is_same_day = true;
    }
    
    if (week_day_mask == 0) 
    {
        is_once = true;
    }
   
    if (is_same_day && alert_hour == local_time->tm_hour && alert_minute == local_time->tm_min)
    {
        // LOGD(TAG, "到点{}:{}执行", alert_hour, alert_minute);
        return true;
    }
    else if(is_once && alert_hour == local_time->tm_hour && alert_minute == local_time->tm_min)
    {
        // LOGD(TAG, "单次到点{}:{}执行", alert_hour, alert_minute, local_time->tm_hour, local_time->tm_min);
        return true;
    }
    else
    {
        // LOGD(TAG, "没有到点, 预计时间 {}:{}, 当地时间{}:{} ,week_day_mask: {}, tm_wday: {}",
        //     alert_hour, alert_minute, local_time->tm_hour, local_time->tm_min,
        //     week_day_mask, local_time->tm_wday);
        return false;
    }
}