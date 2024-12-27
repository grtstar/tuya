/**
 * @file tuya_ipc_dp.h
 * @brief This is tuya ipc dp file
 * @version 1.0
 * @date 2021-11-17
 *
 * @copyright Copyright 2021-2031 Tuya Inc. All Rights Reserved.
 *
 */

#ifndef __TUYA_IPC_DP_H__
#define __TUYA_IPC_DP_H__

#include <time.h>
#include <stdio.h>
#include "tuya_iot_config.h"
#include "tuya_cloud_wifi_defs.h"
#include "tuya_cloud_types.h"
#include "tuya_error_code.h"
#include "tuya_cloud_com_defs.h"
#include "cJSON.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief report dp(data point) to tuya cloud asynchronous, this interface will report your dp everytime whatever it's value is
 * 
 * @param[in] dev_id: device id, cab be NULL if not a sub device
 * @param[in] dp_id: data point number
 * @param[in] type: data point type
 * @param[in] p_val: data point value
 * @param[in] cnt: data point count
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_dp_report(IN CONST CHAR_T *dev_id, IN BYTE_T dp_id, IN DP_PROP_TP_E type, IN VOID *p_val, IN CONST UINT_T cnt);

/**
 * @brief report dp(data point) to tuya cloud asynchronous, this interface only report your dp when it's value is different between last time
 *
 * @param[in] dev_id: device id, cab be NULL if not a sub device
 * @param[in] dp_id: data point number
 * @param[in] type: data point type
 * @param[in] p_val: data point value
 * @param[in] cnt: data point count
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_ipc_dp_report_async(IN CONST CHAR_T *dev_id, IN BYTE_T dp_id,IN DP_PROP_TP_E type,IN VOID * pVal,IN CONST UINT_T cnt);

/**
 * @brief report dp to tuya cloud synchronous
 * 
 * @param[in] dev_id: device id, can be NULL if not a sub device
 * @param[in] dp_id: data point number
 * @param[in] type: data point type
 * @param[in] p_val: data point value
 * @param[in] cnt: data point count
 * @param[in] timeout: wait interface return until timeout or success, timeout in seconds
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_dp_report_sync(IN CONST CHAR_T *dev_id, IN BYTE_T dp_id, IN DP_PROP_TP_E type, IN VOID *p_val, IN CONST UINT_T cnt, IN CONST UINT_T timeout);

/**
 * @brief report dp to tuya cloud synchronous with report timestamp
 * 
 * @param[in] dev_id: device id, can be NULL if not a sub device
 * @param[in] dp_id: data point number
 * @param[in] type: data point type
 * @param[in] p_val: data point value
 * @param[in] cnt: data point count
 * @param[in] timeout: wait interface return until timeout or success, timeout in seconds
 * @param[in] time: report timestamp
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_dp_report_sync_with_time(IN CONST CHAR_T *dev_id, IN BYTE_T dp_id, IN DP_PROP_TP_E type,
                                              IN VOID *p_val, IN CONST UINT_T cnt, IN CONST UINT_T timeout, UINT_T time);


#ifdef __cplusplus
}
#endif

#endif

