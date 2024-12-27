/*********************************************************************************
  *Copyright(C),2015-2020, TUYA www.tuya.comm
  *FileName:    tuya_sdk_net_sync_demo
**********************************************************************************/
#ifndef INCLUDE_TUYA_SDK_NET_SYNC_H_
#define INCLUDE_TUYA_SDK_NET_SYNC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "tuya_cloud_com_defs.h"
#include "cJSON.h"
#include "tuya_os_adapter.h"

/**
 * @brief  设备的wifi状态回调
 * @param  [BYTE_T*] stat 
 * @return [*]
 */
VOID ty_sdk_net_status_change_cb(IN CONST BYTE_T stat);

/**
 * @brief  sdk中的mqtt在线后loop处理
 * @param  [*]
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_sdk_mqtt_online_sys_loop(void);

#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_TUYA_SDK_NET_SYNC_H_ */