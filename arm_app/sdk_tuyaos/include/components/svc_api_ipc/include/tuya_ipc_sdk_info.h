/**
 * @file tuya_ipc_sdk_info.h
 * @brief This is tuya ipc sdk info file
 * @version 1.0
 * @date 2021-11-17
 *
 * @copyright Copyright 2021-2031 Tuya Inc. All Rights Reserved.
 *
 */

#ifndef _TUYA_IPC_SDK_INFO_H
#define _TUYA_IPC_SDK_INFO_H

#include "ty_cJSON.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum  {
    E_IPC_UNREGISTERED = 0,
    E_IPC_REGISTERED = 1,
    E_IPC_ACTIVEATED = 2    ///< after successful register and active process, device is in this status
} IPC_REGISTER_STATUS_E;


/**
 * @brief return sdk info, sdk version etc.
 *
 * @param VOID 
 * 
 * @return CHAR_T* 
 */
CHAR_T* tuya_ipc_get_sdk_info(VOID);

/**
 * @brief get register status on tuya cloud
 * 
 * @param VOID 
 * 
 * @return IPC_REGISTER_STATUS_E 
 */
IPC_REGISTER_STATUS_E tuya_ipc_get_register_status(VOID);

/**
 * @brief get time which sync with tuya server and APP
 * 
 * @param[out] time_utc: utc time
 * @param[out] time_zone: time zone in seconds
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_get_service_time(OUT TIME_T *time_utc, OUT INT_T *time_zone);

/**
 * @brief check if specified utc time is in daylight saving time
 * 
 * @param[in] time_utc: utc time
 * @param[out] p_is_dls: TRUE/FALSE set into *p_is_dls
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_check_in_dls(IN TIME_T time_utc, OUT BOOL_T *p_is_dls);

/**
 * @brief transfer utc time to local time with timezone
 * 
 * @param[in] in_time: input utc time
 * @param[out] local_time: output local time 
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_get_local_time(IN TIME_T in_time, OUT struct tm *local_time);

/**
 * @brief get utc time in tm format
 * 
 * @param[out] utc_time
 *  
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_get_utc_tm(OUT struct tm *utc_time);

/**
 * @brief get utc time
 * 
 * @param[out] time_utc: utc time
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_get_utc_time(OUT TIME_T *time_utc);

/**
 * @brief get free memory of system in KB, only standard linux supported
 * 
 * @param[out] free: KB
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_get_free_ram(OUT ULONG_T *free);

/**
 * @brief Get signatrue key from ipc sdk.
 * 
 * @param[out] dest_key_buf: len >= 17
 * @param[out] len: key length
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_get_local_key(OUT CHAR_T *dest_key_buf, OUT UINT_T *len);

/**
 * @brief get device id
 * 
 * @param[out] devid: device id
 * @param[out] id_len: device id length
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_get_device_id(OUT CHAR_T *devid, OUT INT_T *id_len);

/**
 * @brief get low power server info
 * 
 * @param[out] ip: server ip
 * @param[out] port: server port
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_get_low_power_server(OUT UINT_T *ip, OUT UINT_T *port);

/**
 * @brief get low power server info v2
 * 
 * @param[out] domain: server domain
 * @param[in] domain_len: domain length
 * @param[out] ip: server ip
 * @param[out] port: server port
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_get_low_power_server_v2(OUT CHAR_T *domain, IN INT_T domain_len, OUT UINT_T *ip, OUT UINT_T *port);

/**
 * @brief get mqtt connection status, mqtt is used by tuya SDK to commnicate with tuya cloud
 * 
 * @param VOID 
 * 
 * @return 0:offline 1:online 
 */
INT_T tuya_ipc_get_mqtt_status(VOID);

/**
 * @brief set mqtt connection status,  called by status cb. useless for customers.
 * 
 * @param stat: mqtt status, notified by mqtt event. 0:offline, 1:online
 * 
 * @return 0:success 
 */
OPERATE_RET tuya_ipc_set_mqtt_status(INT_T stat);
/**
 * @brief set time of tuya SDK
 * 
 * @param[in] new_time_utc: new utc time
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_set_service_time(IN TIME_T new_time_utc);

/**
 * @brief get SDK time, if failed will force update from cloud
 * 
 * @param[out] time_utc: utc time
 * @param[out] time_zone: time zone in seconds
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_get_service_time_force(OUT TIME_T *time_utc, OUT INT_T *time_zone);


typedef VOID (*ALGO_MODEL_NOTIFY_CB)(IN CONST INT_T download_result, IN PVOID_T pri_data);
typedef OPERATE_RET (*GET_ALGO_MODEL_DATA_CB)(IN CONST UINT_T total_len,IN CONST UINT_T offset,
                                             IN CONST BYTE_T *data,IN CONST UINT_T len,
                                             OUT UINT_T *remain_len,IN PVOID_T priv_data);
/**
 * @brief get algorithm model information
 *
 * @param[IN] model_id: algorithm model id; allow to NULL;
 * @param[IN] model_ver: algorithm model version; allow to NULL;
 * @param[OUT] result: algorithm model result from cloud;
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_ipc_get_algorithm_model_info(IN CHAR_T * model_id,IN CHAR_T *model_ver,OUT ty_cJSON ** result);
/**
 * @brief get algorithm model file
 *
 * @param[IN] url: file url;
 * @param[IN] total_file_size: file size;
 * @param[IN] file_hmac: file hmac;
 * @param[IN] algo_model_data_cb: file data callback;
 * @param[IN] algo_model_notify_cb: file dowlowd end callback;
 * @param[IN] pri_data: user custome data;
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_ipc_download_algorithm_model_start(IN CHAR_T * url, IN UINT_T total_file_size,IN CHAR_T file_hmac[64],
                                                    IN GET_ALGO_MODEL_DATA_CB algo_model_data_cb,
                                                    IN ALGO_MODEL_NOTIFY_CB algo_model_notify_cb,
                                                    IN PVOID_T pri_data);
#ifdef __cplusplus
}
#endif
    
#endif
    

