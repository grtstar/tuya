/*********************************************************************************
  *Copyright(C),2015-2020, TUYA www.tuya.comm
  *FileName:    tuya_sdk_upgrade_demo
**********************************************************************************/

#ifndef INCLUDE_TUYA_UPGRADE_DEMO_H_
#define INCLUDE_TUYA_UPGRADE_DEMO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "tuya_cloud_types.h"
#include "tuya_cloud_com_defs.h"

/**
 * @brief  OTA前检查设备状态是否符合升级的回调
 * @param  [TY_SDK_FW_UG_T] *fw
 * @return [INT_T] TI_UPGRD_STAT_S
 */
INT_T ty_dev_upgrade_pre_check_cb(IN CONST FW_UG_S *fw);

/**
 * @brief  OTA SDK回调函数
 * @param  [TY_SDK_FW_UG_T] *fw
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
INT_T ty_user_upgrade_inform_cb(IN CONST FW_UG_S *fw);

#ifdef __cplusplus
}
#endif

#endif  /*INCLUDE_TUYA_UPGRADE_DEMO_H_*/
