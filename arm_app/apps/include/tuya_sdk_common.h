/*********************************************************************************
  *Copyright(C),2015-2020, TUYA www.tuya.comm
  *FileName:    tuya_sdk_common
**********************************************************************************/
#ifndef INCLUDE_TUYA_SDK_COMMON_H_
#define INCLUDE_TUYA_SDK_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "uni_log.h"
#include "tuya_cloud_types.h"
#include "tuya_cloud_com_defs.h"
#include "tuya_robot_sdk_init.h"

#define TY_APP_STORAGE_PATH    "/tmp/"     //虚拟存储路径
#define TY_SDK_ONLINE_LOG_PATH "/tmp/"     //虚拟存储路径
#define TY_APP_VERSION         "1.0.1"     //主机上报的版本
#define TY_MCU_VERSION         "1.0.0"     //mcu 上报的版本
#define TY_SDK_P2P_NUM_MAX     5           //最大支持5个客户端
#define TY_ROBOT_MEDIA_ENABLE  1           //打开音视频开关


#ifdef __cplusplus
}
#endif
#endif /*INCLUDE_TUYA_SDK_COMMON_H_*/
