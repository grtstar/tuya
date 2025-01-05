/*********************************************************************************
  *Copyright(C),2015-2020, TUYA www.tuya.comm
  *FileName:    tuya_sdk_storage_demo
**********************************************************************************/

#ifndef INCLUDE_TUYA_STORAGE_DEMO_H_
#define INCLUDE_TUYA_STORAGE_DEMO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "tuya_cloud_types.h"

/**
 * @brief  存储的功能测试入口
 * @param  [*]
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_storage_enable_oper(VOID);

/**
 * @brief  创建存储任务
 * @param [*]
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_enable_storage_deal(VOID);

#ifdef __cplusplus
}
#endif
#endif  /*INCLUDE_TUYA_STORAGE_DEMO_H_*/
