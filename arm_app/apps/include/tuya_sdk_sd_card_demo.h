/*********************************************************************************
  *Copyright(C),2015-2020, TUYA www.tuya.comm
  *FileName:    tuya_sdk_sd_card_demo
**********************************************************************************/
#ifndef INCLUDE_TUYA_SDK_SD_CARD_DEMO_H_
#define INCLUDE_TUYA_SDK_SD_CARD_DEMO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "tuya_cloud_types.h"
#include "tuya_cloud_com_defs.h"

/**
 * @brief  获取SD状态回调
 * @param  [int] status  
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
int tuya_robot_sd_status_upload(int status);

/**
 * @brief  SD卡录像开关状态
 * @param  [BOOL_T]  sd_record_on_off true为打开，false为关闭
 * @return [*]
 */
void tuya_robot_set_sd_record_onoff(BOOL_T sd_record_on_off);

/**
 * @brief  获取SD卡存储容量
 * @param  [int*] p_total 总容量
 * @param  [int*] p_used 使用容量
 * @param  [int*] p_empty 空闲容量
 * @return [*]
 */
void tuya_robot_get_sd_storage(int *p_total, int *p_used, int *p_empty);

/**
 * @brief  设置录像模式
 * @param  [int]  sd_record_mode 1为事件录像，2为连续录像
 * @return [*]
 */
void tuya_robot_set_sd_record_mode(int sd_record_mode);

/**
 * @brief  创建SD格式化任务
 * @param  
 * @return [*]
 */
void tuya_robot_format_sd_card(void);

#ifdef __cplusplus
}
#endif
#endif /*INCLUDE_TUYA_SDK_SD_CARD_DEMO_H_*/
