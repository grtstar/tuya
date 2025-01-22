/*********************************************************************************
 *Copyright(C),2015-2020, TUYA www.tuya.comm
 *FileName:    tuya_sdk_net_sync_demo
 *brief：1.This article introduces the synchronization of Wi-Fi status and local
           device time after the MQTT connection is established.
         2.Developers can directly build upon this demo for further development.
 **********************************************************************************/
#include "uni_log.h"
#include "uni_time.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "sys_timer.h"
#include "tuya_cloud_wifi_defs.h"
#include "tuya_sdk_common.h"
#include "tuya_sdk_dp_demo.h"

static BOOL_T s_mqtt_online_status = FALSE; //mqtt上线标识

/**
 * @brief  设备的wifi状态回调
 * @param  [BYTE_T*] stat
 * @return [*]
 */
VOID ty_sdk_net_status_change_cb(IN CONST BYTE_T stat)
{
    static GW_WIFI_NW_STAT_E last_stat = 0xFF;
    static char pair_flag = 0;

    PR_DEBUG("net change cb now status -[%d], last status -[%d]", stat, last_stat);

    if (last_stat == stat) { //相同的状态做过滤
        return;
    }

    last_stat = stat;
    switch (stat) {
    case STAT_UNPROVISION:
    case STAT_AP_STA_UNCFG:
        //这里是设备重置的状态，可以做灯效显示，比如未配网状态，显示黄灯
        pair_flag = TRUE;
        PR_DEBUG("goto wait pair wifi");
        break;
    case STAT_CLOUD_CONN: // wifi连上云端
    case STAT_AP_CLOUD_CONN: //开发者可以在wifi连云之后，做dp、语音、灯效等同步
        PR_DEBUG("mqtt is online\r\n");
        //其它比较耗时的业务逻辑不要在这边处理，可以通过事件的形式发送出去，到别的任务上出来。
        s_mqtt_online_status = true; //mqtt上线
        if (pair_flag) {
            PR_DEBUG("first pair sucess!");
            pair_flag = FALSE;
            //首次配网成功之后的业务处理可以在这里做，比如这边可以显示绿灯，语音播报如配网成功。
        } else {
            //设备正常上线状态，业务上可以在这里处理
        }
        ty_cmd_handle_sync_to_cloud(); // 上传支持的dp点信息（进行dp同步）
        break;

    case STAT_AP_STA_DISC:
    case STAT_STA_DISC:
        //尝试连接wifi，开发者一般不需要关心该过程。如有特殊业务，需要获取wifi的连接过程状态来做相对应的业务功能，可以在这里做。
        //其它比较耗时的业务逻辑不要在这边处理，可以通过事件的形式发送出去，到别的任务上出来。
        break;
    case STAT_STA_CONN: //连上路由器
        PR_NOTICE("device disconnect clouds");
        //这是设备离线状态，可以在这里做一些灯效显示，如显示黄灯
        //其它比较耗时的业务逻辑不要在这边处理，可以通过事件的形式发送出去，到别的任务上出来。
        break;
    case STAT_REG_FAIL: //激活失败的
        //可以在这里播放激活失败的语音
        //其它比较耗时的业务逻辑不要在这边处理，可以通过事件的形式发送出去，到别的任务上出来。
        break;
    default: {
        break;
    }
    }
    return;
    /**********该回调是1S一次，回调里面不要做复杂的事件，建议通过事件的形式发送到独立线程上处理***************/
}

/**
 * @brief 获取SDK的系统时间（该时间在mqtt上线时会与云端时间同步）
 * @param[out] time_utc: 时间
 * @param[out] time_zone: 时区
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_robot_get_service_time(OUT TIME_T* time_utc, OUT INT_T* time_zone)
{
    if (NULL == time_utc || NULL == time_zone) {
        PR_ERR("invalid param");
        return OPRT_INVALID_PARM;
    }
    *time_utc = uni_time_get_posix(); //获取系统时间戳
    uni_get_time_zone_seconds(time_zone); //获取时区
    PR_DEBUG("robot get service time %d %d", *time_utc, *time_zone);

    if ((OPRT_OK == uni_time_check_time_sync()) && (OPRT_OK == uni_time_check_time_zone_sync())) { //时间戳及时区校验
        return OPRT_OK;
    } else {
        return OPRT_COM_ERROR;
    }
}

/**
 * @brief  定时获取系统时间对时处理
 * @param  [TIMER_ID] timerID
 * @param  [PVOID_T] pTimerArg
 * @return [*]
 */
static void __robot_get_service_timer_cb(TIMER_ID timerID, PVOID_T pTimerArg)
{
    TIME_T time_utc;
    INT_T time_zone;
    int ret = 0;
    char index = 0;

    while (1) {
        ret = tuya_robot_get_service_time(&time_utc, &time_zone); // mqtt在线时，循环获取系统时间，5S后还是获取失败，就不再获取
        if (0 != ret) {
            usleep(1000 * 1000);
            index++;
            // 5s后，获取不到sdk时间，不做对时
            if (index >= 5) {
                return;
            }
        } else {
            break;
        }
    }
    UINT_T time_diff = 0; // sdk同步下来的utc时间和本地时间的差值
    //这里需要开发者自行获取本地时间与云端的time_utc做差值比较
    // 只有云端时间和本地时间差大于5s 进行同步本地时间和sdk时间，开发者可以根据自身的精度要求，来修改差值
    if (time_diff > 5) {
        PR_DEBUG("tuya_get_service_time-time_diff:%d", time_diff);
        //在此处将获取的到服务器时间同步到设备本地
    }
}

/**
 * @brief  sdk中的mqtt在线后loop处理
 * @param  [*]
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_sdk_mqtt_online_sys_loop(void)
{
    while (s_mqtt_online_status == FALSE) { //等待MQTT在线
        usleep(100 * 1000); // 100ms
    }
    PR_DEBUG("tuya_sdk_mqtt_online_proc is start run\n");
    int ret;
    //同步服务器时间
    TIME_T time_utc;
    INT_T time_zone;
    while (1) {
        //设备上电会同步一次时间，对于平台系统时间不是很准的情况下，开发者可以根据自身的需求，每隔一段时间调用该接口，同步一次云端系统时间。
        ret = tuya_robot_get_service_time(&time_utc, &time_zone);
        if (OPRT_OK != ret) {
            //开发者可以在这里做些内存检查的事情，按照自身需求来
            usleep(1000 * 1000);
        } else {
            break;
        }
    }
    PR_DEBUG("get service time [%u-%d]", time_utc, time_zone);
    //开发者在此处将获取的到服务器时间同步到设备本地时间
    //注意：如果开发者需要支持夏/冬令时，请使用以下接口获取时间
    /*
    POSIX_TM_S CurrentTm = { 0 };
    ret =  uni_local_time_get(&CurrentTm);    //该获取本地时间（带时区和夏令时）。如果要获取utc时间（不带时区和夏令时），可以替换成uni_time_get接口
    if(ret != OPRT_OK){
        PR_ERR("uni_local_time_get->CurrentTm ret:%d\n", ret);
        return ret;
    }
    CurrentTm.tm_year = CurrentTm.tm_year + 1900;
    CurrentTm.tm_mon = CurrentTm.tm_mon + 1;
    PR_DEBUG("CurrentTm:%d-%d-%d %d:%d:%d",CurrentTm.tm_year,CurrentTm.tm_mon+1,CurrentTm.tm_mday,\
                                            CurrentTm.tm_hour,CurrentTm.tm_min,CurrentTm.tm_sec);
    */
    PR_DEBUG("tuya_sdk_mqtt_online_proc is end run\n");

    // 定时器ID 初始化
    STATIC TIMER_ID get_service_timer_id = 0;

    // 添加获取时间的定时器
    sys_add_timer(__robot_get_service_timer_cb, NULL, &get_service_timer_id);

    // 开启定时器，6小时机器对时一次
    sys_start_timer(get_service_timer_id, (3600 * 6000), TIMER_CYCLE);

    return 0;
}
