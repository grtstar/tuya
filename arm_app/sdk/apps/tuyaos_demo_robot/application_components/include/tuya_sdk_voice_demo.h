/*********************************************************************************
  *Copyright(C),2015-2020, TUYA www.tuya.comm
  *FileName:    tuya_sdk_voice_demo
**********************************************************************************/
#ifndef INCLUDE_TUYA_SDK_VOICE_DEMO_H_
#define INCLUDE_TUYA_SDK_VOICE_DEMO_H_

#ifdef __cplusplus
extern "C" {
#endif	

#include <stdio.h>
#include "uni_log.h"
#include "tuya_cloud_types.h"
#include "tuya_cloud_com_defs.h"

/**
 * @brief  voice_init处理初始化
 * @param  [*]
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET robotics_svc_init_voice(void);

/**
 * @brief  语音下载的功能测试入口
 * @param  [*]
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET voice_download_oper(VOID);

#ifdef __cplusplus
}
#endif
#endif /*INCLUDE_TUYA_SDK_VOICE_DEMO_H_*/
