/*********************************************************************************
  *Copyright(C),2015-2020, TUYA www.tuya.comm
  *FileName:    tuya_sdk_app_reset_demo
**********************************************************************************/

#ifndef INCLUDE_TUYA_SDK_APP_RESET_DEMO_H_
#define INCLUDE_TUYA_SDK_APP_RESET_DEMO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "tuya_cloud_com_defs.h"

/**
 * @brief  APP或者本地移除设备SDK结果回调
 * @param  [GW_RESET_TYPE_E] type -> 重置的原因
 * @return [*]
 */
VOID ty_sdk_app_reset_cb(GW_RESET_TYPE_E reboot_type);

/**
 * @brief  设备激活状态
 * @param  [in] GW_STATUS_E 激活状态
 * @return [*]
 */
VOID ty_sdk_dev_status_changed_cb(IN CONST GW_STATUS_E status);

#ifdef __cplusplus
}
#endif

#endif  /*INCLUDE_TUYA_SDK_APP_RESET_DEMO_H_*/
