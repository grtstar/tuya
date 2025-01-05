/*********************************************************************************
  *Copyright(C),2015-2020, TUYA www.tuya.comm
  *FileName:    tuya_sdk_simple_start
**********************************************************************************/
#ifndef INCLUDE_TY_SDK_SIMPLE_START_H_
#define INCLUDE_TY_SDK_SIMPLE_START_H_



#ifdef __cplusplus
extern "C" {
#endif

#include "tuya_cloud_types.h"
#include "tuya_cloud_com_defs.h"

/**
 * @brief  扫地机SDK初始化接口
 * @param  [int] mode
 * @param  [char *] token 该token只是测试当中使用，正常代码无效使用
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET ty_sys_start(GW_WF_START_MODE connect_mode, CHAR_T *p_token);

/**
 * @brief  ty media 本地存储处理
 * @param  [*]
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET ty_media_local_storage(void);

/**
 * @brief  云端存储初始化
 * @param  [INT_T] en_audio_mode 设置音频开关
 * @param  [INT_T] pre_recode_time  录制时长
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET ty_media_cloud_storage(INT_T en_audio_mode, INT_T pre_recode_time);

#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_TY_SDK_SIMPLE_START_H_ */
