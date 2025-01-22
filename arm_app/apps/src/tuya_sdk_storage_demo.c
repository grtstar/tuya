/*********************************************************************************
 *Copyright(C),2015-2020, TUYA www.tuya.comm
 *FileName:    tuya_sdk_storage_demo
 *brief：1.This article introduces the implementation demo for cloud storage
           and local SD card storage of audio and video data. Generally, vacuum
           cleaners do not require this functionality.
         2.however, if developers have special needs, they can refer to the
           following logic for implementation.
         3.If developers want to run the following functionality on Ubuntu,
           they can refer to the user_main file to manually operate the cloud
           storage feature as required. Note that the SD card storage functionality
           cannot be tested on the demo.
         4.developers need to verify it on their own devices.
 **********************************************************************************/
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "tuya_ipc_api.h"
#include "tuya_ipc_cloud_storage.h"
#include "tuya_ipc_event.h"
#include "tuya_ipc_stream_storage.h"
#include "tuya_sdk_common.h"
#include "tuya_sdk_media_demo.h"

#define MAX_SNAPSHOT_BUFFER_SIZE (100 * 1024) //最长的一帧数据,视频的封面快照
static char storage_enable_oper_type = FALSE; //存储操作使能，测试用，开发者根据实际的业务逻辑来实现，如dp点来控制
/**
 * @brief  存储功能提供选择
 * @param  [*]
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
STATIC VOID printf_storage_enable_oper_type_help(VOID)
{
    PR_DEBUG("operate type:\r\n");
    PR_DEBUG("1: open storage switch\r\n"); //打开存储开关
    PR_DEBUG("0: close storage switch\r\n"); //关闭存储开关
}

/**
 * @brief  存储的功能测试入口
 * @param  [*]
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_storage_enable_oper(VOID)
{
    int oper_type = 0;

    printf_storage_enable_oper_type_help();
    scanf("%d", &oper_type);
    if (oper_type == TRUE) {
        storage_enable_oper_type = TRUE;
    } else {
        storage_enable_oper_type = FALSE;
    }
    return OPRT_OK;
}

/**
 * @brief  启动存储任务
 * @param
 * @return [*]
 */
VOID* thread_md_proc(VOID* arg)
{
    int alarm_is_triggerd = FALSE;
    char snap_addr[MAX_SNAPSHOT_BUFFER_SIZE] = { 0 };
    int snap_size = MAX_SNAPSHOT_BUFFER_SIZE;
    while (1) {
        usleep(100 * 1000);
        if (storage_enable_oper_type) { //什么时候开启存储的逻辑，开发者自己做，如可以DP控制，也可以条件触发
            if (!alarm_is_triggerd) {
                alarm_is_triggerd = TRUE;
                //启动“本地SD卡事件存储”和“云存储事件”
                tuya_ipc_start_storage(E_ALARM_SD_STORAGE); //启动本地SD卡存储,注意：SD卡存储，demo无法测试，需要硬件支持，开发者自行在硬件上验证。
                tuya_ipc_start_storage(E_ALARM_CLOUD_STORAGE); //启动云存储
                sweeper_app_get_snapshot(snap_addr, &snap_size); //获取一帧数据，视频的快照
                if (snap_size > 0) {
                    tuya_ipc_notify_alarm(snap_addr, snap_size, NOTIFICATION_NAME_MOTION, TRUE); //云存储的一个封面推送，查看云端视频时，不至于是空白封面。
                }
            } else {
                /*
                事件存储开启后，超过SDK中事件的最大时间会自动停止（默认最大600秒，可以通过tuya_ipc_ss_set_max_event_duration接口设置）。
                如果你需要维护存储长时间而不丢失，你可以使用以下接口，监控SDK中是否有停止事件视频，之后选择重新启动新事件。
                */
                if (SS_WRITE_MODE_EVENT == tuya_ipc_ss_get_write_mode() && E_STORAGE_STOP == tuya_ipc_ss_get_status()) { //有本地存储事件，但是处于暂停状态
                    tuya_ipc_start_storage(E_ALARM_SD_STORAGE); //重新开启
                }

                if (ClOUD_STORAGE_TYPE_EVENT == tuya_ipc_cloud_storage_get_store_mode() //有云存储事件，但是处于暂停状态
                    && FALSE == tuya_ipc_cloud_storage_get_status()) {
                    tuya_ipc_start_storage(E_ALARM_CLOUD_STORAGE); //重新开启
                }
            }
        } else {
            if (alarm_is_triggerd) { //已经开始有存储事件
                tuya_ipc_stop_storage(E_ALARM_SD_STORAGE); //暂停SD存储
                tuya_ipc_stop_storage(E_ALARM_CLOUD_STORAGE); //暂停云存储
                alarm_is_triggerd = FALSE;
            }
        }
    }
}

/**
 * @brief  创建存储任务
 * @param [*]
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_enable_storage_deal(VOID)
{

    pthread_t enable_storage_thread;
    pthread_create(&enable_storage_thread, NULL, thread_md_proc, NULL);
    pthread_detach(enable_storage_thread);

    return OPRT_OK;
}
