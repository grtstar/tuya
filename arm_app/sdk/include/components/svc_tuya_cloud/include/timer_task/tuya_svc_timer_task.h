/**
* @file tuya_svc_timer_task.h
* @author liukang@tuya.com
* @brief Common process - Initialization
* @version 0.1
* @date 2020-11-09
*
* @copyright Copyright 2020-2021 Tuya Inc. All Rights Reserved.
*
*/
#ifndef __TUYA_SVC_TIME_TASK_H__
#define __TUYA_SVC_TIME_TASK_H__

#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * 隔天循环有定时开始时间：具体开始时间（具体到某一天）
 * 周循环和奇偶日、隔小时 只有开始时间点，不用确认开始日期
 */
typedef BYTE_T LOOP_TYPE_T;
#define LOOP_TYPE_WEEKLY   (0) // 每周循环
#define LOOP_TYPE_DAILY    (1) // 隔天循环, 1~31天
#define LOOP_TYPE_HOURLY   (2) // 小时间隔, 1-6小时
#define LOOP_TYPE_EVEN_ODD (3) // 奇偶循环, 单双号
#define LOOP_TYPE_INVALID (0xFE) // 无效循环类型


#pragma pack(1)

typedef struct {
    USHORT_T year;
    UCHAR_T  mon;
    UCHAR_T  day;
} TIMER_DATE_S;


/*
bit6 bit5 bit4 bit3 bit2 bit1 bit0
周日  周一 周二  周三  周四 周五  周六
64   32   16    8    4    2    1
*/
typedef struct {
    UINT_T weekday;
} TIMER_WEEK_S;


typedef struct {
    TIME_T last_time; /* 记录定时器上次执行时间，防止重复执行 */
    UCHAR_T valid; /* 0:无效，1: 有效 */
    UCHAR_T hour;
    UCHAR_T min;
    UCHAR_T loop;
    CHAR_T *timer_id;
    union {
        TIMER_DATE_S date;
        TIMER_WEEK_S weekday;
    } day;
    CHAR_T *dps_info;

#if defined(ENABLE_CALENDAR_TIMER) && (ENABLE_CALENDAR_TIMER==1)
    //日历定时
    LOOP_TYPE_T loop_type; // 0:周循环 1:日间隔，2:小时间隔 3:奇偶循环
    UCHAR_T loop_interval; // 隔天（不超过31天）、隔小时（1-6小时可选）
    UCHAR_T end_hour;      // 结束时间
    UCHAR_T end_min;       // 结束时间
    TIMER_DATE_S end_date; // 结束日期
#endif
} ALARM_TIMER_S;

#pragma pack()

/**
 * @brief Init a timer task.
 *
 * @param[in] increase_unit: increase unit
 *
 * @note This API is used for initializing the timer task.
 * when the task queue is full, it will expand the size with the unit user input.
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_svc_timer_task_init(UINT_T increase_unit);

/**
 * @brief Reset the timer task.
 *
 * @param VOID
 *
 * @note This API is used to reset the timer task. the resource of timer task will be released.
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_svc_timer_task_reset(VOID_T);

/**
 * @brief Clear up the timer task data in flash.
 *
 * @param VOID
 *
 * @note This API is used to clear up the timer task data in flash.
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_svc_timer_task_clear_db(VOID_T);

/**
 * @brief Get the count of timer task.
 *
 * @param VOID
 *
 * @note This API is used for getting the count of timer task.
 * @return the count of timer task.
 */
UINT_T tuya_svc_timer_get_task_count(VOID_T);

/**
 * @brief Get the num of timer task.
 *
 * @param VOID
 *
 * @note This API is used for getting the num of timer task.
 * @return the capacity of timer task.
 */
UINT_T tuya_svc_timer_get_num(VOID_T);

/**
 * @brief Get the capacity of timer task.
 *
 * @param VOID
 *
 * @note This API is used for getting the capacity of timer task.
 * @return the capacity of timer task.
 */
UINT_T tuya_svc_timer_get_capacity(VOID_T);

/**
 * @brief delete timer task.
 *
 * @param[in] timer_id: timer id
 * @param[in] update_flash: if need update timer to flash
 *
 * @note This API is used for deleting the timer task.
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_svc_timer_delete(CHAR_T* timer_id, BOOL_T update_flash);

/**
 * @brief get timer task data.
 *
 * @param[out] timer_data: timer id and crc32
 *
 * @note This API is used for getting timer task data.
 * @return void
 */
VOID_T tuya_svc_timer_get_data(uint8_t *timer_data);

/**
 * @brief add timer task.
 *
 * @param[in] pTimer: timer info
 * @param[in] update_flash: if need update timer to flash
 *
 * @note This API is used for adding timer task.
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_svc_timer_add(ALARM_TIMER_S *pTimer, BOOL_T update_flash);

/**
 * @brief Query from cloud immediately when mqtt recved.
 * 
 * @param[in] urgent Urgent or not
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_svc_timer_task_urgent(BOOL_T urgent);

/**
 * @brief Is timer will execute in next 120s
 * 
 * @return BOOL_T
 */
BOOL_T tuya_svc_timer_task_check_trigger(VOID);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /*__TUYA_SVC_TIME_TASK_H__ */

