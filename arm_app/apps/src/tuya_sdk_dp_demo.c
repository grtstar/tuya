/*********************************************************************************
 *Copyright(C),2015-2020, TUYA www.tuya.comm
 *FileName:    tuya_sdk_dp_demo
 *brief：1.This article introduces demos for receiving and sending various types
           of DP (Data Points) for the vacuum cleaner.
         2.Developers can use the following demos as a basis for directly
           developing DP points, adding DP that exist in the PID, and removing DP
           that are not present in the PID.
 *Note: 1.DP should be reported as needed.
        2.Do not report DPs that do not exist under the PID, do not report
          nonexistent DP values, and do not report DP values that exceed the
          defined range.
 **********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "tuya_iot_com_api.h"
#include "tuya_sdk_dp_demo.h"
#include "utilities/uni_log.h"
#if defined(TY_ROBOT_MEDIA_ENABLE) && (TY_ROBOT_MEDIA_ENABLE == 1)
#include "tuya_sdk_sd_card_demo.h"
#endif

static ROBOT_DP_S dp_set = { 0 };    //DP点资源全局变量

/**
 * @brief  设备端接口obj-dp类型
 */
typedef VOID (*TUYA_DP_HANDLER)(IN TY_OBJ_DP_S* p_obj_dp);
typedef struct
{
    BYTE_T dp_id; // dp点的id
    TUYA_DP_HANDLER handler; // 函数句柄
} TUYA_DP_INFO_S;

/**
 * @brief  设备端接口raw-dp类型
 */
typedef void (*TUYA_RAW_DP_HANDLER)(unsigned char* p_obj_dp, unsigned int len);
typedef struct
{
    BYTE_T dp_id; // dp点的id
    TUYA_RAW_DP_HANDLER handler; // 函数句柄
} TUYA_RAW_DP_INFO_S;

/***************以下是dp的接收与发送的函数声明*******************/
#ifdef TUYA_DP_SWITCH_GO
static void __dp_handle_switch_go(TY_OBJ_DP_S* p_obj_dp);
static void dp_handle_switch_go_response(bool switch_go);
#endif

#ifdef TUYA_DP_PAUSE
static void __dp_handle_pause(TY_OBJ_DP_S* p_obj_dp);
static void dp_handle_pause_response(bool pause);
#endif

#ifdef TUYA_DP_SWITCH_CHARGE
static void __dp_handle_charge(TY_OBJ_DP_S* p_obj_dp);
static void dp_handle_charge_response(bool switch_charge);
#endif

#ifdef TUYA_DP_WORK_MODE
static void __dp_handle_work_mode_set(TY_OBJ_DP_S* p_obj_dp);
static void dp_handle_work_mode_response(ROBOT_WORK_MODE_E mode);
#endif

#ifdef TUYA_DP_STATUS
static void dp_handle_status_response(ROBOT_STATUS_E status);
static void dp_handle_status_restart_response(void);
#endif

#ifdef TUYA_DP_CLEAN_MODE
static void __dp_handle_clean_mode_set(TY_OBJ_DP_S* p_obj_dp);
#endif

#ifdef TUYA_DP_SUCTION
static void __dp_handle_suction_set(TY_OBJ_DP_S* p_obj_dp);
#endif

#ifdef TUYA_DP_CISTERN
static void __dp_handle_cistern_set(TY_OBJ_DP_S* p_obj_dp);
#endif

#ifdef TUYA_DP_DIRECTION_CONTROL
static void __dp_handle_direction_control(TY_OBJ_DP_S* p_obj_dp);
#endif

#ifdef TUYA_DP_MAP_RESET
static void __dp_handle_map_reset(TY_OBJ_DP_S* p_obj_dp);
#endif

#if defined TUYA_DP_COMMAND_TRANS || defined TUYA_DP_DEVICE_TIMER || defined TUYA_DP_DISTURB_TIMER_SET
static void __dp_handle_command_trans(unsigned char* p_obj_dp, unsigned int len);
#endif

#ifdef TUYA_DP_EDGE_BRUSH_LIEF
static void __dp_handle_edge_brush_used_time_reset(TY_OBJ_DP_S* p_obj_dp);
#endif

#ifdef TUYA_DP_ROLL_BRUSH_LIEF
static void __dp_handle_roll_brush_used_time_reset(TY_OBJ_DP_S* p_obj_dp);
#endif

#ifdef TUYA_DP_FILTER_LIEF
static void __dp_handle_filter_used_time_reset(TY_OBJ_DP_S* p_obj_dp);
#endif

#ifdef TUYA_DP_RAG_LIEF
static void __dp_handle_rag_used_time_reset(TY_OBJ_DP_S* p_obj_dp);
#endif

#ifdef TUYA_DP_FAULT
// static void __dp_handle_fault(TY_OBJ_DP_S* p_obj_dp);
#endif

#ifdef TUYA_DP_SD_STATUS_ONLY_GET
static void __dp_handle_sd_status_get(TY_OBJ_DP_S* p_obj_dp);
#endif

#ifdef TUYA_DP_SD_STORAGE_ONLY_GET
static void __dp_handle_sd_storage_get(TY_OBJ_DP_S* p_obj_dp);
#endif

#ifdef TUYA_DP_SD_RECORD_ENABLE
static void __dp_handle_sd_record_enable(TY_OBJ_DP_S* p_obj_dp);
#endif

#ifdef TUYA_DP_SD_RECORD_MODE
static void __dp_handle_sd_record_mode(TY_OBJ_DP_S* p_obj_dp);
#endif

#ifdef TUYA_DP_SD_UMOUNT
static void __dp_handle_sd_umount(TY_OBJ_DP_S* p_obj_dp);
#endif

#ifdef TUYA_DP_SD_FORMAT
static void __dp_handle_sd_format(TY_OBJ_DP_S* p_obj_dp);
#endif

#ifdef TUYA_DP_SD_FORMAT_STATUS_ONLY_GET
static void __dp_handle_sd_format_status_get(TY_OBJ_DP_S* p_obj_dp);
#endif

/**
 * @brief  处理dp类型数据函数的集合，用于数据解析；dp点与数据处理函数一一对应
 */
STATIC TUYA_DP_INFO_S s_dp_table[] = {

#ifdef TUYA_DP_SWITCH_GO
    { TUYA_DP_SWITCH_GO, __dp_handle_switch_go },
#endif

#ifdef TUYA_DP_PAUSE
    { TUYA_DP_PAUSE, __dp_handle_pause },
#endif

#ifdef TUYA_DP_SWITCH_CHARGE
    { TUYA_DP_SWITCH_CHARGE, __dp_handle_charge },
#endif

#ifdef TUYA_DP_WORK_MODE
    { TUYA_DP_WORK_MODE, __dp_handle_work_mode_set },
#endif

#ifdef TUYA_DP_CLEAN_MODE
    { TUYA_DP_CLEAN_MODE, __dp_handle_clean_mode_set },
#endif

#ifdef TUYA_DP_SUCTION
    { TUYA_DP_SUCTION, __dp_handle_suction_set },
#endif

#ifdef TUYA_DP_CISTERN
    { TUYA_DP_CISTERN, __dp_handle_cistern_set },
#endif

#ifdef TUYA_DP_DIRECTION_CONTROL
    { TUYA_DP_DIRECTION_CONTROL, __dp_handle_direction_control },
#endif

#ifdef TUYA_DP_MAP_RESET
    { TUYA_DP_MAP_RESET, __dp_handle_map_reset },
#endif

#ifdef TUYA_DP_EDGE_BRUSH_LIEF
    { TUYA_DP_EDGE_BRUSH_LIEF, __dp_handle_edge_brush_used_time_reset },
#endif

#ifdef TUYA_DP_ROLL_BRUSH_LIEF
    { TUYA_DP_ROLL_BRUSH_LIEF, __dp_handle_roll_brush_used_time_reset },
#endif

#ifdef TUYA_DP_FILTER_LIEF
    { TUYA_DP_FILTER_LIEF, __dp_handle_filter_used_time_reset },
#endif

#ifdef TUYA_DP_RAG_LIEF
    { TUYA_DP_RAG_LIEF, __dp_handle_rag_used_time_reset },
#endif

#ifdef TUYA_DP_SD_STATUS_ONLY_GET
    { TUYA_DP_SD_STATUS_ONLY_GET, __dp_handle_sd_status_get },
#endif

#ifdef TUYA_DP_SD_STORAGE_ONLY_GET
    { TUYA_DP_SD_STORAGE_ONLY_GET, __dp_handle_sd_storage_get },
#endif

#ifdef TUYA_DP_SD_RECORD_ENABLE
    { TUYA_DP_SD_RECORD_ENABLE, __dp_handle_sd_record_enable },
#endif

#ifdef TUYA_DP_SD_RECORD_MODE
    { TUYA_DP_SD_RECORD_MODE, __dp_handle_sd_record_mode },
#endif

#ifdef TUYA_DP_SD_UMOUNT
    { TUYA_DP_SD_UMOUNT, __dp_handle_sd_umount },
#endif

#ifdef TUYA_DP_SD_FORMAT
    { TUYA_DP_SD_FORMAT, __dp_handle_sd_format },
#endif

#ifdef TUYA_DP_SD_FORMAT_STATUS_ONLY_GET
    { TUYA_DP_SD_FORMAT_STATUS_ONLY_GET, __dp_handle_sd_format_status_get },
#endif

};

/**
 * @brief  处理raw类型数据函数的集合，用于数据解析；dp点与数据处理函数一一对应
 */
STATIC TUYA_RAW_DP_INFO_S s_raw_dp_table[] = {

#ifdef TUYA_DP_COMMAND_TRANS
    { TUYA_DP_COMMAND_TRANS, __dp_handle_command_trans },
#endif

#ifdef TUYA_DP_DEVICE_TIMER
    { TUYA_DP_COMMAND_TRANS, __dp_handle_command_trans },
#endif

#ifdef TUYA_DP_DISTURB_TIMER_SET
    { TUYA_DP_COMMAND_TRANS, __dp_handle_command_trans },
#endif

};

/**
 * @brief  累加和的方式计算check sum
 * @param  [char*] buf
 * @param  [char] len
 * @return [char] 结果
 */
unsigned char calc_check_sum(unsigned char* buf, int len)
{
    unsigned char sum = 0;
    while (len-- > 0) {
        sum += *(buf++);
    }
    return sum;
}

/**
 * @brief  布尔量DP点数值check
 * @param  [TY_OBJ_DP_S] *p_obj_dp
 * @return [int] 结果
 */
int check_dp_bool_invalid(TY_OBJ_DP_S* p_obj_dp)
{
    if (p_obj_dp == NULL) { // 判断数据是否合法
        PR_ERR("error! input is null");
        return -1;
    }

    if (p_obj_dp->type != PROP_BOOL) { // 判断类型是否合法
        PR_ERR("error! input is not bool %d", p_obj_dp->type);
        return -2;
    }

    if (0 == p_obj_dp->value.dp_bool) {
        return FALSE;
    } else if (1 == p_obj_dp->value.dp_bool) {
        return TRUE;
    } else {
        PR_ERR("Error!! type invalid %d ", p_obj_dp->value.dp_bool);
        return -2;
    }
}

/**
 * @brief  接收obj dp函数
 * @param  [TY_RECV_OBJ_DP_S*] p_obj_dp
 * @return [*]
 */
VOID robotics_app_dp_cmd_objs(IN CONST TY_RECV_OBJ_DP_S* dp_rev)
{
    TY_OBJ_DP_S* dp_data = (TY_OBJ_DP_S*)(dp_rev->dps);
    UINT_T cnt = dp_rev->dps_cnt;
    INT_T table_idx = 0;
    INT_T table_count = (sizeof(s_dp_table) / sizeof(s_dp_table[0]));
    INT_T index = 0;
    for (index = 0; index < cnt; index++) {
        TY_OBJ_DP_S* p_dp_obj = dp_data + index;
        PR_DEBUG("recv dpid:[%d]\r\n", p_dp_obj->dpid);

        for (table_idx = 0; table_idx < table_count; table_idx++) {
            if (s_dp_table[table_idx].dp_id == p_dp_obj->dpid) {
                s_dp_table[table_idx].handler(p_dp_obj);
                /*开发者在此处接收到的数据，只能做一些简单解析处理，具体要执行的行为，请放到别的任务中去*/
                break;
            }
        }
    }
}

/**
 * @brief  接收raw dp函数
 * @param  [TY_RECV_RAW_DP_S*] p_obj_dp
 * @return [*]
 */
VOID robotics_app_dp_raw_objs(IN CONST TY_RECV_RAW_DP_S* dp_rev)
{
    unsigned char* dp_data = (unsigned char*)dp_rev->data;
    int table_idx = 0;
    int table_count = (sizeof(s_raw_dp_table) / sizeof(s_raw_dp_table[0]));

    PR_DEBUG("dpid:%d cmd_tp:%d", dp_rev->dpid, dp_rev->cmd_tp);
    for (table_idx = 0; table_idx < table_count; table_idx++) {
        if (s_raw_dp_table[table_idx].dp_id == dp_rev->dpid) {
            s_raw_dp_table[table_idx].handler(dp_data, dp_rev->len);
            /*收到的数据可参考涂鸦激光扫地机的协议*/
            /*开发者在此处接收到的数据，只能做一些简单解析处理，具体要执行的行为，请放到别的任务中去*/
            break;
        }
    }
}

/**
 * @brief  接收查询dp函数
 * @param  [TY_DP_QUERY_S*] p_obj_dp
 * @return [*]
 */
VOID robotics_app_dp_query_objs(IN CONST TY_DP_QUERY_S* dp_query)
{
    if (!dp_query->cnt) {
        PR_DEBUG("recv query objs cnt = 0");
        return;
    }
    dp_handle_sync_to_cloud(); // APP来查询时，同步设备所有的DP数据
}

/**
 * @brief  接收到sdk obj dp处理回调，注册到SDK中
 * @param  [TY_SDK_RECV_OBJ_DP_S] *dp_rev
 * @return [*]
 */
VOID ty_cmd_handle_dp_cmd_objs(IN CONST TY_RECV_OBJ_DP_S* dp_rev)
{
    PR_DEBUG("[user cmd]recv cmd dp");
    robotics_app_dp_cmd_objs(dp_rev);
}

/**
 * @brief  接收到sdk raw dp处理回调，注册到SDK中
 * @param  [TY_SDK_RECV_OBJ_DP_S] *dp_rev
 * @return [*]
 */
VOID ty_cmd_handle_dp_raw_objs(IN CONST TY_RECV_RAW_DP_S* dp_rev)
{
    PR_DEBUG("[user raw cmd]recv cmd dp");
    robotics_app_dp_raw_objs(dp_rev);
}

/**
 * @brief  接收到sdk query dp处理回调，注册到SDK中
 * @param  [TY_SDK_RECV_OBJ_DP_S] *dp_rev
 * @return [*]
 */
VOID ty_cmd_handle_dp_query_objs(IN CONST TY_DP_QUERY_S* dp_query)
{
    PR_DEBUG("[user cmd]recv query dp");
    robotics_app_dp_query_objs(dp_query);
}

/**
 * @brief  调用sdk中的接口进行数值型dp回复
 * @param  [BYTE_T] dp_id
 * @param  [INT_T] val
 * @return [*]
 */
VOID respone_dp_value(BYTE_T dp_id, INT_T val)
{
    TY_OBJ_DP_S dp_obj_data[1] = { { 0 } };
    dp_obj_data[0].dpid = dp_id;
    dp_obj_data[0].type = PROP_VALUE;
    dp_obj_data[0].value.dp_value = val;
    /*以下是dp异步上报，会过滤重复的dp值，当您不关心实际的上报次数，只需最后一次上报的状态和设备的实际状态保持一致时，就可以选择这种上报方式，如开关量、累计值等*/
    dev_report_dp_json_async(NULL, dp_obj_data, 1);
    /*以下是dp异步强制上报，不会过滤重复的dp值，如果您没有这方面的需求，请不要使用，以免dp上报受限制（云端最大一天3500条dp记录）*/
    // dev_report_dp_json_async_force(NULL, dp_obj_data, 1);
    /*注意：异步上报是SDK会缓存上报数据，等到SDK内的上报线程被调度时再将数据上报，不会堵塞应用线程。*/
}

/**
 * @brief  调用sdk中的接口进行布尔量dp回复
 * @param  [BYTE_T] dp_id
 * @param  [BOOL_T] true_false
 * @return [*]
 */
VOID respone_dp_bool(BYTE_T dp_id, BOOL_T true_false)
{
    TY_OBJ_DP_S dp_obj_data[1] = { { 0 } };
    dp_obj_data[0].dpid = dp_id;
    dp_obj_data[0].type = PROP_BOOL;
    dp_obj_data[0].value.dp_bool = true_false;

    dev_report_dp_json_async(NULL, dp_obj_data, 1); // dp异步上报
}

/**
 * @brief  调用sdk中的接口进行枚举dp回复
 * @param  [BYTE_T] dp_id
 * @param  [INT_T] p_val_enum
 * @return [*]
 */
VOID respone_dp_enum(BYTE_T dp_id, INT_T p_val_enum)
{
    TY_OBJ_DP_S dp_obj_data[1] = { { 0 } };
    dp_obj_data[0].dpid = dp_id;
    dp_obj_data[0].type = PROP_ENUM;
    dp_obj_data[0].value.dp_enum = p_val_enum;

    dev_report_dp_json_async(NULL, dp_obj_data, 1); // dp异步上报
    // dev_report_dp_stat_sync(NULL, dp_obj_data, 1, 5); // dp同步上报
    /*注意：SDK立即上报数据，会堵塞应用线程，且不会过滤相同值的 DP 数据。
           对应用的堆栈深度要求也会比较高。堵塞时间可通过接口参数进行配置，一般设置 5S。
           有些场景对 DP 的上报次数需要精准控制，应用需要立即知道这次上报的结果，做出相应处理。*/
}

/**
 * @brief  调用sdk中的接口进行图位型dp回复
 * @param  [BYTE_T] dp_id
 * @param  [INT_T] p_val_str
 * @return [*]
 */
void respone_dp_bitmap(BYTE_T dp_id, INT_T p_val_str)
{
    TY_OBJ_DP_S dp_obj_data[1] = { { 0 } };
    dp_obj_data[0].dpid = dp_id;
    dp_obj_data[0].type = PROP_BITMAP;
    dp_obj_data[0].value.dp_bitmap = p_val_str;

    dev_report_dp_json_async(NULL, dp_obj_data, 1); // dp异步上报
}

/**
 * @brief  调用sdk中的接口进行字符串dp回复
 * @param  [BYTE_T] dp_id
 * @param  [CHAR_T] *p_val_str
 * @return [*]
 */
VOID respone_dp_str(BYTE_T dp_id, CHAR_T* p_val_str)
{
    TY_OBJ_DP_S dp_obj_data[1] = { { 0 } };
    dp_obj_data[0].dpid = dp_id;
    dp_obj_data[0].type = PROP_STR;
    dp_obj_data[0].value.dp_str = p_val_str;

    dev_report_dp_json_async(NULL, dp_obj_data, 1); // dp异步上报
}

/**
 * @brief  调用sdk中的接口进行raw dp回复
 * @param  [unsigned int] dp_id
 * @param  [unsigned char] *p_val_str
 * @return [*]
 */
void response_dp_raw(unsigned int dp_id, unsigned char* p_val_raw, unsigned int len)
{
    dev_report_dp_raw_sync(NULL, dp_id, p_val_raw, len, 5); // raw数据同步上报
    /*RAW类型 DP 只支持同步上报，且SDK不会对该 DP 数据进行缓存，即也不会有过滤和数据同步机制*/
}

/**
 * @brief  获取DP 处理核心参数设置
 * @return [*]
 */
ROBOT_DP_S* get_dp_handler_core_sets(void)
{
    return &dp_set;
}

/**
 * @brief  选区清扫信息回复
 * @return [*]
 */
void dp_handle_room_clean_info_response(void)
{
    /*****
     * 这里可以做相对应的业务处理
     * 此处demo程序使用了激光协议进行编写，具体数据打包方式根据自身协议决定
     * ******/

    unsigned char response_buf[RAW_DATA_MAX_LEN] = { 0 };
    int data_len = 0;

    /*****将原app下发数据打包复制出来*******/
    memcpy(response_buf, &dp_set.room_clean_sets.raw_data, dp_set.room_clean_sets.raw_data_len);

    /*****修改原有指令，将其转化为读取查询指令*******/
    response_buf[COMMAND_TRANS_HEAD_LEN] += 1; // cmd加1为上报
    data_len = dp_set.room_clean_sets.raw_data_len - COMMAND_TRANS_HEAD_LEN - 1; // 有效数据长度

    /*****更新校验码*******/
    response_buf[dp_set.room_clean_sets.raw_data_len - 1] = calc_check_sum(response_buf + COMMAND_TRANS_HEAD_LEN, data_len);

    /*****调用发送接口上报*******/
    response_dp_raw(TUYA_DP_COMMAND_TRANS, response_buf, dp_set.room_clean_sets.raw_data_len);
}

/**
 * @brief  定点清扫信息回复
 * @return [*]
 */
void dp_handle_pose_clean_info_response(void)
{
    /*****
     * 这里可以做相对应的业务处理
     * 此处demo程序使用了激光协议进行编写，具体数据打包方式根据自身协议决定
     * ******/

    unsigned char response_buf[RAW_DATA_MAX_LEN] = { 0 };
    int data_len = 0;

    /*****将原app下发数据打包复制出来*******/
    memcpy(response_buf, &dp_set.pose_sets.raw_data, dp_set.pose_sets.raw_data_len);

    /*****修改原有指令，将其转化为读取查询指令*******/
    response_buf[COMMAND_TRANS_HEAD_LEN] += 1; // cmd加1为上报
    data_len = dp_set.pose_sets.raw_data_len - COMMAND_TRANS_HEAD_LEN - 1; //有效数据长度

    /*****更新校验码*******/
    response_buf[dp_set.pose_sets.raw_data_len - 1] = calc_check_sum(response_buf + COMMAND_TRANS_HEAD_LEN, data_len);

    /*****调用发送接口上报*******/
    response_dp_raw(TUYA_DP_COMMAND_TRANS, response_buf, dp_set.pose_sets.raw_data_len);
}

/**
 * @brief  划区清扫信息回复
 * @return [*]
 */
void dp_handle_area_clean_info_response(void)
{
    /*****
     * 这里可以做相对应的业务处理
     * 此处demo程序使用了激光协议进行编写，具体数据打包方式根据自身协议决定
     * ******/

    unsigned char response_buf[RAW_DATA_MAX_LEN] = { 0 };
    int data_len = 0;

    /*****将原app下发数据打包复制出来*******/
    memcpy(response_buf, &dp_set.area_clean_sets.raw_data, dp_set.area_clean_sets.raw_data_len);

    /*****修改原有指令，将其转化为读取查询指令*******/
    response_buf[COMMAND_TRANS_HEAD_LEN] += 1; // cmd加1为上报
    data_len = dp_set.area_clean_sets.raw_data_len - COMMAND_TRANS_HEAD_LEN - 1; //有效数据长度

    /*****更新校验码*******/
    response_buf[dp_set.area_clean_sets.raw_data_len - 1] = calc_check_sum(response_buf + COMMAND_TRANS_HEAD_LEN, data_len);

    /*****调用发送接口上报*******/
    response_dp_raw(TUYA_DP_COMMAND_TRANS, response_buf, dp_set.area_clean_sets.raw_data_len);
}

#ifdef TUYA_DP_SWITCH_GO
/**
 * @brief  结合work_mode、trans raw数据, 根据switch go开启
 * @param  [ROBOT_DP_S*] dp_set
 * @return [*]
 */
static void __dp_handle_switch_go_deal(int switch_go)
{
    /***attention：
     *  1.当收到switch_go指令前，app必然会下发清扫模式、选区清扫等指令告知当前清扫目标；
     *  2.根据当前收到的switch_go状态及app下发的清扫模式控制机器执行相应的动作；
     * ***/

    ROBOT_DP_S* dp_handle = get_dp_handler_core_sets();

    if (switch_go) {
        switch (dp_handle->work_mode) {
        case ROBOT_MODE_SMART: {

            /** 当前执行的智能清扫任务，可将指令告知业务执行智能清扫； **/
        } break;

        case ROBOT_MODE_SELECT_ROOM: {

            /** 当前执行选区清扫任务，结合选区参数执行清扫任务**/
        } break;

        case ROBOT_MODE_ZONE: {

            /** 当前执行划区清扫任务，结合划区参数执行清扫任务**/
        } break;

        case ROBOT_MODE_POSE: {

            /** 当前执行定点清扫任务，结合当前点的地图坐标执行清扫任务**/
        } break;

        default:
            /** 其他清扫模式根据业务需求及协议实现 **/
            break;
        }

    } else {

        /** 清扫任务结束，可将指令告知业务结束清扫任务 **/
    }
}

/**
 * @brief  收到switch_go指令后，进行相应的回复app
 * @param  [int] switch_go
 * @return [*]
 */
static void __dp_handle_switch_go_response(int switch_go)
{
    /***attention：
     *  1.switch_go这个dp点下发前会下发模式、清扫模式等；因此在收到siwitch_go这个dp点时需要对之前的信息进行回复；
     *  2.每个模式需要回复的内容根据协议确定；
     * ***/

    ROBOT_DP_S* dp_handle = get_dp_handler_core_sets();

    if (switch_go) { // 当前siwtch_go == ture , 执行清扫任务,以下数据是设备当前状态同步给面板展示。

        dp_handle_pause_response(FALSE); //回复暂停及回充开关为FALSE
        dp_handle_charge_response(FALSE);
        switch (dp_handle->work_mode) {
        case ROBOT_MODE_SMART: {
            dp_handle_switch_go_response(TRUE); // 回复siwitch_go=TRUE开始清扫;
            dp_handle_work_mode_response(ROBOT_MODE_SMART); // 回复work_mode;
            dp_handle_status_response(DP_ST_SMART); // 回复当前机器状态;
        } break;

        case ROBOT_MODE_SELECT_ROOM: {
            dp_handle_room_clean_info_response(); // 回复高级指令，选区清扫;
            dp_handle_switch_go_response(TRUE); // 回复siwitch_go=TRUE开始清扫;
            dp_handle_work_mode_response(ROBOT_MODE_SELECT_ROOM); // 回复work_mode;
            dp_handle_status_response(DP_ST_SELECT_ROOM); // 回复当前机器状态;
        } break;

        case ROBOT_MODE_ZONE: {
            dp_handle_area_clean_info_response(); // 回复高级指令: 划区清扫;
            dp_handle_switch_go_response(TRUE); // 回复siwitch_go=TRUE开始清扫;
            dp_handle_work_mode_response(ROBOT_MODE_ZONE); // 回复work_mode;
            dp_handle_status_response(DP_ST_ZONE_CLEAN); // 回复当前机器状态;
        } break;

        case ROBOT_MODE_POSE: {
            dp_handle_pose_clean_info_response(); // 回复高级指令: 定点清扫;
            dp_handle_switch_go_response(TRUE); // 回复siwitch_go=TRUE开始清扫;
            dp_handle_work_mode_response(ROBOT_MODE_POSE); // 回复work_mode;
            dp_handle_status_response(DP_ST_GOTO_POS); // 回复当前机器状态;
        } break;

        default:
            break;
        }
    } else {
        dp_handle_switch_go_response(FALSE); //当前switch_go == false ， 结束任务
        dp_handle_status_response(DP_ST_STANDBY); // 回复当前机器状态;
    }
}

/**
 * @brief  switch_go dp接收处理并上报
 * @param  [TY_SDK_OBJ_DP_S*] p_obj_dp
 * @return [*]
 */
static void __dp_handle_switch_go(TY_OBJ_DP_S* p_obj_dp)
{
    int switch_go;

    switch_go = check_dp_bool_invalid(p_obj_dp); //验证数据内容及类型
    if (switch_go < 0) {
        PR_ERR("switch dp set err!");
        return;
    }

    dp_set.switch_go = switch_go;

    PR_DEBUG("switch go goto deal %d!!!", switch_go);
    /*****这里可以做相对应的业务处理******/
    __dp_handle_switch_go_deal(switch_go);

    /*****这里对数据进行相应的回复：可以在本文件中执行，也可告知业务功能，在业务中进行回复*****/
    __dp_handle_switch_go_response(switch_go);

    return;
}

/**
 * @brief  switch_go dp 回复云端
 * @return [*]
 */
void dp_handle_switch_go_response(bool switch_go)
{
    PR_DEBUG("goto set switch_go = %d", switch_go);

    respone_dp_bool(TUYA_DP_SWITCH_GO, switch_go);
    return;
}
#endif

#ifdef TUYA_DP_PAUSE
/**
 * @brief  结合work_mode、trans raw数据, 根据switch go开启
 * @param  [ROBOT_DP_S*] dp_set
 * @return [*]
 */
static void __dp_handle_pause_deal(int pause)
{
    /***attention：
     *  收到pause == true ，表明需要暂停的任务；pause == false 表明继续当前的任务
     * ***/

    // ROBOT_DP_S* dp_handle = get_dp_handler_core_sets();

    if (pause) {
        /**
         * 这里可以做相对应的业务处理:
         * 收到app下发暂停任务指令，发布指令给业务将当前执行的任务暂停；
         * ***/

    } else {
        /**
         * 这里可以做相对应的业务处理:
         * 收到app下发继续任务指令，发布指令给业务继续执行原有任务
         * ***/
    }
}

/**
 * @brief  收到switch_go指令后，进行相应的回复app
 * @param  [int] switch_go
 * @return [*]
 */
static void __dp_handle_pause_response(int pause)
{
    /***attention：
     *  对app进行回复的时候，需要包含暂停指令回复、当前状态回复
     * ***/

    if (pause) {
        dp_handle_pause_response(TRUE); // 回复app，当前收到继续
        dp_handle_status_response(DP_ST_PAUSED); // 当前为暂停状态
    } else {
        dp_handle_pause_response(FALSE); // 回复app，当前收到继续
        dp_handle_status_restart_response(); // 回复当前状态，当前状态根据之前的状态进行上报；
    }
}

/**
 * @brief  暂停开始DP点处理
 * @param  TY_OBJ_DP_S* p_obj_dp
 * @return [TY_OBJ_DP_S*] p_obj_dp
 */
static void __dp_handle_pause(TY_OBJ_DP_S* p_obj_dp)
{
    int pause_onoff = check_dp_bool_invalid(p_obj_dp); // 验证数据内容及类型
    if (pause_onoff < 0) {
        PR_ERR("pause dp set err!");
        return;
    }

    dp_set.pause = pause_onoff;

    PR_DEBUG("pause goto deal %d!!!", pause_onoff);
    /*****这里可以做相对应的业务处理******/
    __dp_handle_pause_deal(pause_onoff);

    /*****这里对数据进行相应的回复******/
    __dp_handle_pause_response(pause_onoff);

    return;
}

/**
 * @brief  暂停DP设置 & 上报
 * @return [*]
 */
static void dp_handle_pause_response(bool pause)
{
    PR_DEBUG("goto set pause %d", pause);

    respone_dp_bool(TUYA_DP_PAUSE, pause);
    return;
}
#endif

#ifdef TUYA_DP_SWITCH_CHARGE
/**
 * @brief  设置回充
 * @param  TY_OBJ_DP_S* p_obj_dp
 * @return [*]
 */
static void __dp_handle_charge(TY_OBJ_DP_S* p_obj_dp)
{
    int switch_charge = check_dp_bool_invalid(p_obj_dp); // 验证数据内容及类型

    if (switch_charge < 0) {
        PR_ERR("pause dp set err!");
        return;
    }
    PR_DEBUG("switch charge goto deal %d!!!", switch_charge);
    /**
     * 这里可以做相对应的业务处理:
     * 此处收到app下发回充指令，发布给业务进行相应启动回充任务
     * ***/

    /*****这里需要对数据进行的回复******/
    // attention:回复回充信息不单单只回复回充指令，还需将当前状态切换成回充状态发送给app
    dp_handle_charge_response(TRUE);
    dp_handle_status_response(DP_ST_GOTO_CHARGE);

    return;
}

/**
 * @brief  回充DP上报回复
 * @return [*]
 */
static void dp_handle_charge_response(bool switch_charge)
{
    PR_DEBUG("goto set charge switch dp %d", switch_charge);

    respone_dp_bool(TUYA_DP_SWITCH_CHARGE, switch_charge); // 调用接口回复云端

    return;
}
#endif

#ifdef TUYA_DP_WORK_MODE
/**
 * @brief  运行模式设置（自动、定点、区域、房间等）
 * @param  [TY_OBJ_DP_S*] p_obj_dp
 * @return [*]
 */
static void __dp_handle_work_mode_set(TY_OBJ_DP_S* p_obj_dp)
{
    if ((NULL == p_obj_dp) || (p_obj_dp->type != PROP_ENUM)) { // 验证数据内容及类型
        PR_ERR("Error!! type invalid ");
        return;
    }

    int set_mode = p_obj_dp->value.dp_enum;

    PR_DEBUG(">>>>>>>>>>>>>>>>>>>>>>>>set mode is %d %d", set_mode, p_obj_dp->value.dp_enum);
    /**
     * 这里可以做相对应的业务处理:
     * 工作模式需要保存，提供给switch_go指令使用；保存形式根据自身业务而定；
     * ***/

    dp_set.work_mode = set_mode;
    if ((dp_set.work_mode == ROBOT_MODE_ZONE) || (dp_set.work_mode == ROBOT_MODE_SELECT_ROOM) || (dp_set.work_mode == ROBOT_MODE_POSE)) {
        __dp_handle_switch_go_response(true); //清扫任务都需要回复清扫开关状态
    }
}

/**
 * @brief  运行模式 DP点回复 APP
 * @param  [ROBOT_WORK_MODE_E] mode
 * @return [*]
 */
static void dp_handle_work_mode_response(ROBOT_WORK_MODE_E mode)
{
    PR_DEBUG("goto set mode %d", mode);

    if (mode >= ROBOT_DP_SET_MAX) {
        mode = 0;
    } else {
        ;
    }

    /*****
     * 这里可以做相对应的业务处理
     * 1.将该数据加载到业务中，当开始清扫的时候使用该参数；保存形式根据自身业务而定
     * ******/
    dp_set.work_mode = mode;

    if (0 <= mode && mode < ROBOT_DP_SET_MAX) { // 枚举值上报需要做判断，超限值不要上报，可能会导致设备异常。
        /****** 需要对数据进行回复*******/
        respone_dp_enum(TUYA_DP_WORK_MODE, mode);
    } else {
        PR_ERR("dp %d response value out of range, value: %d.", TUYA_DP_WORK_MODE, mode);
    }
}
#endif

#ifdef TUYA_DP_STATUS
/**
 * @brief  暂停恢复任务任务状态自动获取 & 恢复
 * @return [*]
 */
static void dp_handle_status_restart_response(void)
{

    /*****attention
     * 当前机器从断点需扫或暂停重新开始的情况下，需要上报当前的机器状态；
     * 因此需要从当前的工作模式中获取当前的工作状态，并进行上报；
     * 此处使用全局变量保存工作模式，此方式根据自身业务功能自行决定；
     * ******/

    switch (dp_set.work_mode) {
    case ROBOT_MODE_SMART:
        dp_set.status = DP_ST_SMART;
        break;

    case ROBOT_MODE_GOTO_CHARGE:
        dp_set.status = DP_ST_GOTO_CHARGE;
        break;

    case ROBOT_MODE_ZONE:
        dp_set.status = DP_ST_ZONE_CLEAN;
        break;

    case ROBOT_MODE_POSE:
        dp_set.status = DP_ST_GOTO_POS;
        break;

    case ROBOT_MODE_SELECT_ROOM:
        dp_set.status = DP_ST_SELECT_ROOM;
        break;

    default:
        PR_ERR("Unknown work mode %d", dp_set.work_mode);
        return;
    }

    if (0 <= dp_set.status && dp_set.status < DP_ST_STATUS_MAX) { // attention:枚举值上报需要做判断，超限值不要上报，可能会导致设备异常。
        /****** 上报当前的状态 *******/
        respone_dp_enum(TUYA_DP_STATUS, dp_set.status);
    } else {
        PR_ERR("dp %d response value out of range, value: %d.", TUYA_DP_STATUS, dp_set.status);
    }

    return;
}

/**
 * @brief  status DP 设置 & 回复
 * @param  [ROBOT_STATUS_E] status
 * @return [*]
 */
static void dp_handle_status_response(ROBOT_STATUS_E status)
{
    PR_DEBUG("goto set status %d", status);

    /****机器状态需要配合其他清扫任务进行进行回复 ******/
    dp_set.status = status;

    if (0 <= dp_set.status && dp_set.status < DP_ST_STATUS_MAX) {
        /****** 需要对数据进行回复*******/
        respone_dp_enum(TUYA_DP_STATUS, dp_set.status);
    } else {
        PR_ERR("dp %d response value out of range, value: %d.", TUYA_DP_STATUS, dp_set.status);
    }
}
#endif

#ifdef TUYA_DP_CLEAN_MODE
/**
 * @brief  清扫工作模式DP点设置（只扫、只拖、扫拖）
 * @param  [TY_SDK_OBJ_DP_S*] p_obj_dp
 * @return [*]
 */
static void __dp_handle_clean_mode_set(TY_OBJ_DP_S* p_obj_dp)
{
    if ((NULL == p_obj_dp) || (p_obj_dp->type != PROP_ENUM)) {
        PR_ERR("Error!! type invalid ");
        return;
    }

    int set_clean_mode = p_obj_dp->value.dp_enum;

    PR_DEBUG(">>>>>>>>>>>>>>>>>>>>>>>>set clean mode is %d", set_clean_mode);

    if (dp_set.clean_mode != set_clean_mode) {

        /*****
         * 这里可以做相对应的业务处理
         * 1.参数进行更新存储，因为重启后该数据仍需有效；
         * 2.将该数据加载到业务中，当开始清扫的时候使用该参数
         * ******/
        dp_set.clean_mode = set_clean_mode;
    }

    if (0 <= dp_set.clean_mode && dp_set.clean_mode < CLEAN_MODE_MAX) {
        /****** 需要对数据进行回复*******/
        respone_dp_enum(TUYA_DP_CLEAN_MODE, dp_set.clean_mode);
    } else {
        PR_ERR("dp %d response value out of range, value: %d.", TUYA_DP_CLEAN_MODE, dp_set.clean_mode);
    }
}
#endif

#ifdef TUYA_DP_DIRECTION_CONTROL
/**
 * @brief  遥控控制
 * @param  [TY_SDK_OBJ_DP_S*] p_obj_dp
 * @return [*]
 */
static void __dp_handle_direction_control(TY_OBJ_DP_S* p_obj_dp)
{
    if ((NULL == p_obj_dp) || (p_obj_dp->type != PROP_ENUM)) {
        PR_ERR("Error!! type invalid ");
        return;
    }

    if (0 <= p_obj_dp->value.dp_enum && p_obj_dp->value.dp_enum < CTRL_STATE_MAX) {
        /****** 需要对数据进行回复* ******/
        respone_dp_enum(TUYA_DP_DIRECTION_CONTROL, p_obj_dp->value.dp_enum);
    } else {
        PR_ERR("dp %d response value out of range, value: %d.", TUYA_DP_DIRECTION_CONTROL, p_obj_dp->value.dp_enum);
    }
    /*****
     * 这里可以做相对应的业务处理
     * 需要将收到的按键信息发布给业务，让业务控制扫地机器按照数据方向进行运动
     * ******/
}
#endif

#ifdef TUYA_DP_MAP_RESET
/**
 * @brief  地图重置
 * @param  [TY_SDK_OBJ_DP_S*] p_obj_dp
 * @return [*]
 */
static void __dp_handle_map_reset(TY_OBJ_DP_S* p_obj_dp)
{
    int map_reset = check_dp_bool_invalid(p_obj_dp); // 验证数据内容及类型

    if (map_reset < 0) {
        PR_ERR("switch dp set err!");
        return;
    }

    map_reset = p_obj_dp->value.dp_enum;

    /*****
     * 需要对数据进行回复
     * ******/
    respone_dp_bool(TUYA_DP_MAP_RESET, map_reset);

    /*****
     * 这里可以做相对应的业务处理
     * 需要将收到的重置地图信息发布给业务，让业务将原有地图删除
     * ******/
}
#endif

#ifdef TUYA_DP_SUCTION
/**
 * @brief  设置吸力
 * @param  [TY_OBJ_DP_S*] p_obj_dp
 * @return [*]
 */
static void __dp_handle_suction_set(TY_OBJ_DP_S* p_obj_dp)
{
    if ((NULL == p_obj_dp) || (p_obj_dp->type != PROP_ENUM)) { // 验证数据内容及类型
        PR_ERR("Error!! type invalid ");
        return;
    }
    int set_suction = p_obj_dp->value.dp_enum;

    PR_DEBUG(">>>>>>>>>>>>>>>>>>>>>>>>set suction is %d", set_suction);

    if (dp_set.suction_select != set_suction) {
        /*****
         * 这里可以做相对应的业务处理
         * 1.参数进行更新存储，因为重启后该数据仍需有效；
         * 2.将该数据加载到业务中，当开始清扫的时候使用该参数
         * ******/
        dp_set.suction_select = set_suction;
    }

    if (0 <= set_suction && set_suction < E_MOTOR_VACCUM_STATE_MAX) { // 枚举值上报需要做判断，超限值不要上报，可能会导致设备异常。
        /****** 需要对数据进行回复*******/
        respone_dp_enum(TUYA_DP_SUCTION, set_suction);
    } else {
        PR_ERR("dp %d response value out of range, value: %d.", TUYA_DP_SUCTION, set_suction);
    }

    return;
}
#endif

#ifdef TUYA_DP_CISTERN
/**
 * @brief  设置水量
 * @param  [TY_OBJ_DP_S*] p_obj_dp
 * @return [*]
 */
static void __dp_handle_cistern_set(TY_OBJ_DP_S* p_obj_dp)
{
    if ((NULL == p_obj_dp) || (p_obj_dp->type != PROP_ENUM)) { // 验证数据内容及类型
        PR_ERR("Error!! type invalid ");
        return;
    }

    int set_cistern = p_obj_dp->value.dp_enum;

    PR_DEBUG(">>>>>>>>>>>>>>>>>>>>>>>>set cistern is %d", set_cistern);

    if (dp_set.cistern_select != set_cistern) {
        /*****
         * 这里可以做相对应的业务处理
         * 1.参数进行更新存储，因为重启后该数据仍需有效；
         * 2.将该数据加载到业务中，当开始清扫的时候使用该参数
         * ******/
        dp_set.cistern_select = set_cistern;
    }

    if (0 <= set_cistern && set_cistern < E_MOTOR_PUMPER_STATE_MAX) { // 枚举值上报需要做判断，超限值不要上报，可能会导致设备异常。
        /****** 需要对数据进行回复*******/
        respone_dp_enum(TUYA_DP_CISTERN, set_cistern);
    } else {
        PR_ERR("dp %d response value out of range, value: %d.", TUYA_DP_CISTERN, set_cistern);
    }
}
#endif

#ifdef TUYA_DP_EDGE_BRUSH_LIEF
/**
 * @brief  边刷使用时长重置处理
 * @param  [TY_OBJ_DP_S*] p_obj_dp
 * @return [*]
 */
static void __dp_handle_edge_brush_used_time_reset(TY_OBJ_DP_S* p_obj_dp)
{
    if ((p_obj_dp == NULL) || (p_obj_dp->type != PROP_VALUE)) {
        PR_ERR("Error!! type invalid ");
        return;
    }

    if (p_obj_dp->value.dp_value != 0) {
        PR_ERR("Error!! edge brush used time set value invalid %d", p_obj_dp->value.dp_value);
        return;
    }
    /*****
     * 这里可以做相对应的业务处理
     * 1.重置边刷使用时长需要对该参数进行更新，并发布到业务中进行使用；
     * 2.更新后的使用时长信息进行保存，以便重启后数据正确使用；
     * ******/
    dp_set.edge_brush_used_time = 9999; // 9999是设置的最长边刷时长，此参数根据扫地机边刷使用情况自定

    /****** 需要对数据进行回复*******/
    respone_dp_value(TUYA_DP_EDGE_BRUSH_LIEF, dp_set.edge_brush_used_time);
}
#endif

#ifdef TUYA_DP_ROLL_BRUSH_LIEF
/**
 * @brief  滚刷使用时长重置处理
 * @param  [TY_SDK_OBJ_DP_S*] p_obj_dp
 * @return [*]
 */
static void __dp_handle_roll_brush_used_time_reset(TY_OBJ_DP_S* p_obj_dp)
{
    if ((p_obj_dp == NULL) || (p_obj_dp->type != PROP_VALUE)) {
        PR_ERR("Error!! type invalid ");
        return;
    }

    if (p_obj_dp->value.dp_value != 0) {
        PR_ERR("Error!! roll brush used time set value invalid %d", p_obj_dp->value.dp_value);
        return;
    }

    /*****
     * 这里可以做相对应的业务处理
     * 1.重置滚刷使用时长需要对该参数进行更新，并发布到业务中进行使用；
     * 2.更新后的使用时长信息进行保存，以便重启后数据正确使用；
     * ******/
    dp_set.roll_brush_used_time = 9999; // 9999是设置的最长滚刷时长，此参数根据扫地机滚刷使用情况自定

    /****** 需要对数据进行回复*******/
    respone_dp_value(TUYA_DP_ROLL_BRUSH_LIEF, dp_set.roll_brush_used_time);
}
#endif

#ifdef TUYA_DP_FILTER_LIEF
/**
 * @brief  过滤滤芯使用时长重置
 * @param  [TY_OBJ_DP_S*] p_obj_dp
 * @return [*]
 */
static void __dp_handle_filter_used_time_reset(TY_OBJ_DP_S* p_obj_dp)
{
    if ((p_obj_dp == NULL) || (p_obj_dp->type != PROP_VALUE)) {
        PR_ERR("Error!! type invalid ");
        return;
    }

    if (p_obj_dp->value.dp_value != 0) {
        PR_ERR("Error!! filter used time set value invalid %d", p_obj_dp->value.dp_value);
        return;
    }

    /*****
     * 这里可以做相对应的业务处理
     * 1.重置滤芯使用时长需要对该参数进行更新，并发布到业务中进行使用；
     * 2.更新后的使用时长信息进行保存，以便重启后数据正确使用；
     * ******/
    dp_set.filter_used_time = 9999; // 9999是设置的最长滤芯使用时长，此参数根据扫地机滤芯使用情况自定

    /****** 需要对数据进行回复*******/
    respone_dp_value(TUYA_DP_FILTER_LIEF, dp_set.filter_used_time);
}
#endif

#ifdef TUYA_DP_RAG_LIEF
/**
 * @brief  拖布使用时长重置
 * @param  [TY_OBJ_DP_S*] p_obj_dp
 * @return [*]
 */
static void __dp_handle_rag_used_time_reset(TY_OBJ_DP_S* p_obj_dp)
{

    if ((p_obj_dp == NULL) || (p_obj_dp->type != PROP_VALUE)) {
        PR_ERR("Error!! type invalid ");
        return;
    }

    if (p_obj_dp->value.dp_value != 0) {
        PR_ERR("Error!! rag used time set value invalid %d", p_obj_dp->value.dp_value);
        return;
    }

    /*****
     * 这里可以做相对应的业务处理
     * 1.重置拖布使用时长需要对该参数进行更新，并发布到业务中进行使用；
     * 2.更新后的使用时长信息进行保存，以便重启后数据正确使用；
     * ******/
    dp_set.rag_used_time = 9999; // 9999是设置的最长拖布使用时长，此参数根据扫地机拖布使用情况自定

    /****** 需要对数据进行回复*******/
    respone_dp_value(TUYA_DP_RAG_LIEF, dp_set.rag_used_time);
}
#endif

#ifdef TUYA_DP_SD_STATUS_ONLY_GET
/**
 * @brief  获取SD卡状态
 * @param  [TY_OBJ_DP_S*] p_obj_dp
 * @return [*]
 */
static void __dp_handle_sd_status_get(TY_OBJ_DP_S* p_obj_dp)
{
    ROBOT_SD_STATUS_E sd_status = DP_SD_ST_NULL;
    // todo:这里由开发者根据SD的真实状态填写
    sd_status = DP_SD_ST_NORMAL;
    respone_dp_value(TUYA_DP_SD_STATUS_ONLY_GET, sd_status);
}

/**
 * @brief  SD status DP 设置 & 回复
 * @param  [ROBOT_SD_STATUS_E] sd_status
 * @return [*]
 */
void dp_handle_sd_status_response(ROBOT_SD_STATUS_E sd_status)
{
    if ((sd_status < DP_SD_ST_NULL) || (sd_status >= DP_SD_ST_MAX)) {
        return;
    }
    respone_dp_value(TUYA_DP_SD_STATUS_ONLY_GET, sd_status);
}
#endif

#ifdef TUYA_DP_SD_STORAGE_ONLY_GET
/**
 * @brief  获取SD卡存储容量
 * @param  [TY_OBJ_DP_S*] p_obj_dp
 * @return [*]
 */
static void __dp_handle_sd_storage_get(TY_OBJ_DP_S* p_obj_dp)
{
    char tmp_str[100] = { 0 };

    int total = 100;
    int used = 0;
    int empty = 100;
    tuya_robot_get_sd_storage(&total, &used, &empty); //获取SD容量

    //"total capacity|Current usage|remaining capacity"
    snprintf(tmp_str, 100, "%u|%u|%u", total, used, empty);
    respone_dp_str(TUYA_DP_SD_STORAGE_ONLY_GET, tmp_str); //上报云端
}

/**
 * @brief  SD卡存储容量上报
 * @param  []
 * @return [*]
 */
void dp_handle_sd_storage_response(void)
{
    char tmp_str[100] = { 0 };

    int total = 100;
    int used = 0;
    int empty = 100;
    tuya_robot_get_sd_storage(&total, &used, &empty); //获取SD容量

    //"total capacity|Current usage|remaining capacity"
    snprintf(tmp_str, 100, "%u|%u|%u", total, used, empty);
    respone_dp_str(TUYA_DP_SD_STORAGE_ONLY_GET, tmp_str);
}
#endif

#ifdef TUYA_DP_SD_RECORD_MODE
/**
 * @brief  设置录像模式
 * @param  [int]  sd_record_mode 1为事件录像，2为连续录像
 * @return [*]
 */
void dp_handle_set_sd_record_mode(int sd_record_mode)
{
    tuya_robot_set_sd_record_mode(sd_record_mode);
    dp_set.sd_record_mode = sd_record_mode;
}

/**
 * @brief  获取录像模式
 * @param  [int]  sd_record_mode 1为事件录像，2为连续录像
 * @return [*]
 */
int dp_handle_get_sd_record_mode(void)
{
    return dp_set.sd_record_mode;
}

/**
 * @brief  SD卡录像模式
 * @param  [TY_OBJ_DP_S*] p_obj_dp
 * @return [*]
 */
static void __dp_handle_sd_record_mode(TY_OBJ_DP_S* p_obj_dp)
{
    if ((p_obj_dp == NULL) || (p_obj_dp->type != PROP_ENUM)) {
        PR_ERR("Error!! type invalid %d \r\n", p_obj_dp->type);
        return;
    }

    dp_handle_set_sd_record_mode(p_obj_dp->value.dp_enum);
    int mode = dp_handle_get_sd_record_mode();
    respone_dp_enum(TUYA_DP_SD_RECORD_MODE, mode);
}
#endif

#ifdef TUYA_DP_SD_RECORD_ENABLE
/**
 * @brief  SD卡录像开关状态
 * @param  [TY_OBJ_DP_S*] p_obj_dp
 * @return [*]
 */
static void __dp_handle_sd_record_enable(TY_OBJ_DP_S* p_obj_dp)
{
    BOOL_T sd_record_on_off = check_dp_bool_invalid(p_obj_dp);

    /* 注意：开发者自行保存开关状态 */
#ifdef TUYA_DP_SD_RECORD_MODE
    if (sd_record_on_off == TRUE) {
        dp_handle_set_sd_record_mode(dp_handle_get_sd_record_mode());
    } else {
        tuya_robot_set_sd_record_mode(0);
    }
#endif
    dp_set.sd_record_on_off = sd_record_on_off;
    tuya_robot_set_sd_record_onoff(dp_set.sd_record_on_off); //设置录像开关状态

    respone_dp_bool(TUYA_DP_SD_RECORD_ENABLE, sd_record_on_off);
}

#endif

#ifdef TUYA_DP_SD_UMOUNT
/**
 * @brief  卸载SD存储卡
 * @param  [TY_OBJ_DP_S*] p_obj_dp
 * @return [*]
 */
static void __dp_handle_sd_umount(TY_OBJ_DP_S* p_obj_dp)
{
    BOOL_T umount_ok = TRUE;
    /*开发者自行卸载SD卡动作，并将结果上报*/

    respone_dp_bool(TUYA_DP_SD_UMOUNT, umount_ok);
}
#endif

#ifdef TUYA_DP_SD_FORMAT
/**
 * @brief  SD存储卡格式化
 * @param  [TY_OBJ_DP_S*] p_obj_dp
 * @return [*]
 */
static void __dp_handle_sd_format(TY_OBJ_DP_S* p_obj_dp)
{
    tuya_robot_format_sd_card();
    respone_dp_bool(TUYA_DP_SD_FORMAT, TRUE);
}
#endif

#ifdef TUYA_DP_SD_FORMAT_STATUS_ONLY_GET
/**
 * @brief  格式化状态获取
 * @param  [TY_OBJ_DP_S*] p_obj_dp
 * @return [*]
 */
static void __dp_handle_sd_format_status_get(TY_OBJ_DP_S* p_obj_dp)
{
    int progress = dp_set.s_sd_format_progress;

    respone_dp_value(TUYA_DP_SD_FORMAT_STATUS_ONLY_GET, progress);
}

/**
 * @brief  SD卡式化状态上报
 * @param  [int] status 状态
 * @return [*]
 */
void dp_handle_report_sd_format_status(int status)
{
    respone_dp_value(TUYA_DP_SD_FORMAT_STATUS_ONLY_GET, status);
}

#endif

/**
 * @brief  command raw数据校验
 * @param  [unsigned char*] p_obj_dp
 * @param  [unsigned int] len
 * @return [*]
 */
static OPERATE_RET __command_trans_pre_check(unsigned char* p_obj_dp, unsigned int len)
{
    unsigned char check_sum = 0;
    unsigned int data_len = 0;

    /*****校验数据长度是否合规 *******/
    if (len < COMMAND_TRANS_HEAD_LEN) {
        PR_ERR("command trans len is err");
        return OPRT_INVALID_PARM;
    }

    /***** 校验数据头是否合规 *******/
    if ((p_obj_dp[0] != COMMAND_TRANS_HEAD) && (p_obj_dp[0] != COMMAND_TRANS_MAP_HEAD)) {
        PR_ERR("command trans head is err");
        return OPRT_INVALID_PARM;
    }

    data_len = (p_obj_dp[2] << 24 | p_obj_dp[3] << 16 | p_obj_dp[4] << 8 | p_obj_dp[5]);
    PR_DEBUG("recv data len %d", data_len);

    /***** 校验协议钟数据长度是否与实际长度相同 *******/
    if ((data_len + COMMAND_TRANS_HEAD_LEN) >= len) { //接收到的
        PR_ERR("command trans len is err %d", data_len);
        return OPRT_INVALID_PARM;
    }

    /***** 校验码检查，和校验 *******/
    check_sum = calc_check_sum(p_obj_dp + COMMAND_TRANS_HEAD_LEN, data_len);

    if (check_sum != p_obj_dp[len - 1]) {
        PR_ERR("command trans check sum err %x-%x", check_sum, p_obj_dp[len - 1]);
        return OPRT_INVALID_PARM;
    }

    return OPRT_OK;
}

/**
 * @brief  本地定时指令回复
 * @return [*]
 */
void dp_handle_local_time_info_response(char* data_buf, int len)
{

    /*****
     * 这里可以做相对应的业务处理
     * 此处demo程序使用了激光协议进行编写，具体数据打包方式根据自身协议决定
     * ******/

    unsigned char response_buf[MSG_LOCAL_RAW_DATA_MAX_LEN] = { 0 };
    int data_len = 0;

    /*****将原app下发数据打包复制出来*******/
    memcpy(response_buf, data_buf, len);

    /*****修改原有指令，将其转化为读取查询指令*******/
    response_buf[COMMAND_TRANS_HEAD_LEN] += 1; // cmd加1为上报
    data_len = len - COMMAND_TRANS_HEAD_LEN - 1; //有效数据长度

    /*****更新校验码*******/
    response_buf[len - 1] = calc_check_sum(response_buf + COMMAND_TRANS_HEAD_LEN, data_len);

    /*****调用发送接口上报*******/
    response_dp_raw(TUYA_DP_DEVICE_TIMER, response_buf, len);

    memcpy(dp_set.local_timer_sets.raw_data, data_buf, len);
    dp_set.local_timer_sets.raw_data_len = len;
}

/**
 * @brief  勿扰时间设置指令回复
 * @return [*]
 */
void dp_handle_disturb_time_info_response(char* data_buf, int len)
{
    /*****
     * 这里可以做相对应的业务处理
     * 此处demo程序使用了激光协议进行编写，具体数据打包方式根据自身协议决定
     * ******/
    unsigned char response_buf[RAW_DATA_MAX_LEN] = { 0 };
    int data_len = 0;

    /*****将原app下发数据打包复制出来*******/
    memcpy(response_buf, data_buf, len);
    response_buf[COMMAND_TRANS_HEAD_LEN] = CMD_DONT_DISTURB_SET_REQ;
    data_len = len - COMMAND_TRANS_HEAD_LEN - 1; //有效数据长度

    /*****修改原有指令，将其转化为读取查询指令*******/
    response_buf[len - 1] = calc_check_sum(response_buf + COMMAND_TRANS_HEAD_LEN, data_len);

    /*****调用发送接口上报*******/
    response_dp_raw(TUYA_DP_DISTURB_TIMER_SET, response_buf, len);
}

/**
 * @brief  command raw 数据查询判断&回复
 * @param  [char] cmd_type
 * @return [*]
 */
static OPERATE_RET __command_trans_query_judge_proc(char cmd_type)
{
    /*****
     * 这里可以做相对应的业务处理
     * 根据协议中的命令指令进行相应回复；
     * ******/
    switch (cmd_type) {

    case CMD_ROOM_CLEAN_REQ: { //选取清扫查询

        dp_handle_room_clean_info_response();
    } break;

    case CMD_POSE_CLEAN_REQ: { //定点清扫查询

        dp_handle_pose_clean_info_response();

    } break;

    case CMD_AREA_CLEAN_REQ: { //划区清扫查询

        dp_handle_area_clean_info_response();

    } break;

    case CMD_LOCAL_TIME_SET_REQ: { //本地定时查询回复

        dp_handle_local_time_info_response(dp_set.local_timer_sets.raw_data, dp_set.local_timer_sets.raw_data_len);
    } break;

    case CMD_DONT_DISTURB_SET_REQ: { //勿扰模式查询

        dp_handle_disturb_time_info_response(dp_set.disturb_timer_sets.raw_data, dp_set.disturb_timer_sets.raw_data_len);
    } break;

    default:
        return OPRT_INVALID_PARM;
    }

    return OPRT_OK;
}

/**
 * @brief  command指令下发解析 & 处理
 * @param  [unsigned char*] p_obj_dp
 * @param  [unsigned int] len
 * @return [*]
 */
static OPERATE_RET __comman_trans_deal(unsigned char* p_obj_dp, unsigned int len)
{
    switch (p_obj_dp[COMMAND_TRANS_HEAD_LEN]) {

    case CMD_ROOM_CLEAN: { //选区清扫
        /*****
         * 这里可以做相对应的业务处理
         * 根据协议将下发的raw数据进行校验数据并解析收到的数据
         * ******/

    } break;

    case CMD_POSE_CLEAN_V1: { //定点清扫V1
        /*****
         * 这里可以做相对应的业务处理
         * 根据协议将下发的raw数据进行校验数据并解析收到的数据
         * ******/

    } break;

    case CMD_AREA_CLEAN_V2_CUSTOMSIZE: { //划区清扫
        /*****
         * 这里可以做相对应的业务处理
         * 根据协议将下发的raw数据进行校验数据并解析收到的数据
         * ******/

    } break;

    case CMD_LOCAL_TIME_SET: { //本地定时设置
        /*****
         * 这里可以做相对应的业务处理
         * 根据协议将下发的raw数据进行校验数据并解析收到的数据
         * ******/

    } break;

    case CMD_DONT_DISTURB_SET_V1: { //勿扰模式设置
        /*****
         * 这里可以做相对应的业务处理
         * 根据协议将下发的raw数据进行校验数据并解析收到的数据
         * ******/

    } break;
    default:
        return OPRT_INVALID_PARM;
    }

    return OPRT_OK;
}

/**
 * @brief  raw数据进行解析
 * @param  [unsigned char*] p_obj_dp
 * @param  [unsigned int] len
 * @return [*]
 */
static void __dp_handle_command_trans(unsigned char* p_obj_dp, unsigned int len)
{
    /*****
     * 这里可以做相对应的业务处理
     * 1.高级指令dp点中包含着众多raw数据，该dp中可以通过协议中cmd指令来区分每条raw处理的信息,如选区指令为0x14,而定时设置指令为0x30
     * 2.协议中的cmd指令还可以区分当前是设置功能还是查询功能，同一个功能的设置查询命令值差1，如定时设置0x30,查询指令为0x31
     * ******/

    if (NULL == p_obj_dp) {
        PR_ERR("Error!! type invalid ");
        return;
    }

    PR_DEBUG("reciv raw command DP, len %d", len);

    // raw数据校验
    if (OPRT_OK != __command_trans_pre_check(p_obj_dp, len)) {
        PR_ERR("command_raw_trans_head_check error");
        return;
    }
    // raw数据query
    if (__command_trans_query_judge_proc(p_obj_dp[COMMAND_TRANS_HEAD_LEN]) == OPRT_OK) {
        PR_DEBUG("command_raw is query, just response");
        return;
    }

    //不是query 就是控制指令 解析处理
    if (OPRT_OK != __comman_trans_deal(p_obj_dp, len)) {
        PR_ERR("command_tans cmd set error");
        return;
    }
}

/**
 * @brief  保存在flash或内存中的数据加载
 * @return [*]
 */
void dp_handle_all_dp_load(void)
{
    memset(&dp_set, 0, sizeof(ROBOT_DP_S));
    /*****
     * 这里可以做相对应的业务处理
     * 暂存在内存的数据在启动的时候需要重新加载，如吸力选择，水量选择等；
     * ******/
}

/**
 * @brief  上报机器电池电量
 * @param  [int] percent 0~100
 * @return [*]
 */
void ty_cmd_handle_battery_capacity_response(int percent)
{
    /***attention：
     * 当电池电量发生变化的时候，将数据上报给app
     * ***/
#ifdef TUYA_DP_BATTERY_PERCENTAGE
    respone_dp_value(TUYA_DP_BATTERY_PERCENTAGE, percent);
#else
    PR_NOTICE("battert upload dp not define, please check!");
#endif
    return;
}

/**
 * @brief  上报机器异常错误
 * @param  [FAULT_TYPE_E] errno_type
 * @return [*]
 */
void ty_cmd_handle_fault_response(FAULT_TYPE_E errno_type)
{
    /***attention：
     * 外部发生异常情况的时候，调用该接口进行上报
     * ***/
#ifdef TUYA_DP_FAULT
    if (errno_type > E_MSG_FF_MAX) { // 枚举值上报需要做判断，超限值不要上报，可能会导致设备异常。
        PR_ERR("errno type upload value set err %d", errno_type);
        return;
    }
    int errno_bit = 0;
    if (errno_type > E_MSG_FT_NO_ERR) {
        errno_bit = 1 << (errno_type - 1);
    } else {
        ; // do nothing
    }
    respone_dp_bitmap(TUYA_DP_FAULT, errno_bit);
#else
    PR_ERR("default errno  DP not set, please check!");
#endif
    return;
}

/**
 * @brief  设备上线同步DP
 * @param  [unsigned char*] p_obj_dp
 * @param  [unsigned int] len
 * @return [*]
 */
OPERATE_RET dp_handle_sync_to_cloud(void)
{
    /*****
     * 这里可以做相对应的业务处理
     * 1.云端有个限制：一天不要超过3500条，10分钟内上报的dp点数量不要超过1200条，这个是云端对dp上报数量的限流。
     * 因此，将其组合上报，可解决上述问题
     * ******/
    // 上报状态
   
    // ty_cmd_handle_fault_response(E_MSG_FT_BUMER_ERR);   //异常上报接口
    return ret;
}

#ifdef TUYA_DP_DEVICE_INFO
/**
 * @brief  上报机器设备信息
 * @param  [*]
 * @return [*]
 */
OPERATE_RET dp_handle_device_info_respone(void)
{
    /*****
     * 这里可以做相对应的业务处理
     * 获取机器当前的mac、sn 、uuid等参数进行上报
     * ******/

    ty_cJSON* device_info = ty_cJSON_CreateObject(); //创建cjon
    if (NULL == device_info) {
        PR_ERR("json err");
        return OPRT_CR_CJSON_ERR;
    }
    //获取mac信息

    //获取版本信息

    //获取sn信息

    //获取uuid
    char* device_info_buf = ty_cJSON_Print(device_info); //获取cjon为字符串
    PR_DEBUG("device info :%s", device_info_buf);
    PR_DEBUG("device info len:%d", strlen(device_info_buf));
    ty_cJSON_Delete(device_info); //删除cjson
    response_dp_raw(TUYA_DP_DEVICE_INFO, (unsigned char*)device_info_buf, strlen(device_info_buf)); // raw dp的方式上报

    if (device_info_buf != NULL) {
        ty_cJSON_FreeBuffer(device_info_buf); //释放buff
        device_info_buf = NULL;
    }
    return OPRT_OK;
}

#endif

/**
 * @brief  DP点存储值初始化
 * @return [*]
 */
void ty_cmd_dp_init(void)
{
    dp_handle_all_dp_load(); // DP点数据装载
}

/**
 * @brief  设备上线上报相关信息
 * @param  [*]
 * @return [*]
 */
void ty_cmd_handle_sync_to_cloud(void)
{
    /***attention：
     * 当网络连接成功后，需要将机器内的数据进行上传同步
     * ***/
    dp_handle_all_dp_load(); // DP点数据装载
    dp_handle_sync_to_cloud(); //数据打包上报
    dp_handle_device_info_respone(); //机器信息上报
}
