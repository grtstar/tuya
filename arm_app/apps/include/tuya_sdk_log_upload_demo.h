/*********************************************************************************
  *Copyright(C),2015-2020, TUYA www.tuya.comm
  *FileName:    tuya_sdk_log_upload_demo
**********************************************************************************/
#ifndef INCLUDE_TUYA_SDK_LOG_UPLOAD_DEMO_H_
#define INCLUDE_TUYA_SDK_LOG_UPLOAD_DEMO_H_

#ifdef __cplusplus
extern "C" {
#endif	

#include <stdio.h>
#include "uni_log.h"
#include "tuya_cloud_types.h"
#include "tuya_cloud_com_defs.h"

/**
 * @brief  设备本地日志上报初始化
 * @param  [*]
 * @return [*]
 */
void ty_log_upload_init(void);

#ifdef __cplusplus
}
#endif
#endif /*INCLUDE_TUYA_SDK_LOG_UPLOAD_DEMO_H_*/