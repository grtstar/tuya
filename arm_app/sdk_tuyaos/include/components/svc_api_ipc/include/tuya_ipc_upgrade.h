/**
 * @file tuya_ipc_upgrade.h
 * @brief This is tuya ipc upgrade file
 * @version 1.0
 * @date 2021-11-17
 *
 * @copyright Copyright 2021-2031 Tuya Inc. All Rights Reserved.
 *
 */

#ifndef __TUYA_IPC_UPGRADE_H__
#define __TUYA_IPC_UPGRADE_H__

#include "tuya_cloud_com_defs.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef int (*TUYA_IPC_SDK_DEV_UPGRADE_INFORM_CB)(IN CONST FW_UG_S *fw);


typedef struct {
    TUYA_IPC_SDK_DEV_UPGRADE_INFORM_CB upgrade_cb;
} TUYA_IPC_SDK_UPGRADE_T;

typedef struct
{
    GW_ATTACH_ATTR_T *sub_dev_ota_info;
    UINT_T sub_dev_cnt;
}TUYA_IPC_SUB_DEV_OTA_INFO_T;


/**
 * @brief OTA, upgrade via TUYA SDK
 * 
 * @param[in] fw: firmware infomation
 * @param[in] get_file_cb: callback function during downloading fw
 * @param[in] upgrd_nofity_cb: callback function when downloading fw fininsh
 * @param[in] pri_data: data transferred between callback functions
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_upgrade_sdk(IN CONST FW_UG_S *fw,
                                 IN CONST GET_FILE_DATA_CB get_file_cb,
                                 IN CONST UPGRADE_NOTIFY_CB upgrd_nofity_cb,
                                 IN PVOID_T pri_data);

/**
 * @brief send a upgrade progress to tuya cloude and app
 * 
 * @param[in] percent: upgrade progress percent , valid value [0,100]
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_upgrade_progress_report(IN UINT_T percent);

/**
 * @brief send a upgrade progress to tuya cloude and app
 * 
 * @param[in] percent: upgrade progress percent , valid value [0,100]
 * @param[in] type: upgrade dev type , valid value [DEV_TYPE_T]
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_upgrade_progress_report_by_type(IN UINT_T percent, IN BYTE_T type);                                      

/**
 * @brief report upgrade status to app
 * 
 * @param[in] dev_id: ID of sub-device, or NULL for GW
 * @param[in] tp: OTA channel, see DEV_TYPE_T
 * @param[in] status: Upgrade status, see TI_UPGRD_STAT_S
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_upgrade_progress_status(IN CONST CHAR_T *dev_id, IN CONST BYTE_T tp, IN BYTE_T status);


#ifdef __cplusplus
}
#endif

#endif

