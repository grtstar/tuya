/**
* @file      cloud_http.h
* @brief     http request to cloud
* @version   1.0
* @date      2021-11-18
*
* copyright  Copyright (c) tuya.inc 2021
*/
#ifndef __CLOUD_HTTPC_H__
#define __CLOUD_HTTPC_H__

#include "tuya_cloud_types.h"
#include "ty_cJSON.h"
#include "gw_intf.h"
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

#define HTTPC_NULL_CHECK(pointer) if(pointer == NULL) { PR_ERR("%s is null", #pointer); return OPRT_INVALID_PARM; }

/**
* @brief      upload print log to cloud
* @param[in]  p_file        file name
* @param[in]  line          line number
* @param[in]  p_func        function name
* @param[in]  log_type      log type \n
* @ref        tuya_ipc_log  ipc log \n
* @ref        tuya_p2p_log  p2p log
* @param[in]  p_fmt         printf format
* @return     OPERATE_RET
* - OPRT_OK   upload success
* - Others    upload failed
*/
OPERATE_RET tuya_ipc_printf_log_up(IN CONST PCHAR_T p_file,
                                             IN CONST INT_T line,
                                             IN CONST PCHAR_T p_func,
                                             IN CONST unsigned char log_type,
                                             IN CONST PCHAR_T p_fmt, ...);

#define tuya_ipc_log 1
#define tuya_p2p_log 2

#ifdef __LITEOS__
#define TUYA_IPC_PRINT_LOG_UP(logo_type,fmt, ...)
#else

#define TUYA_IPC_PRINT_LOG_UP(logo_type,fmt, ...)  \
do { \
    tuya_ipc_printf_log_up(__FILE__,__LINE__,(PCHAR_T)__func__,(unsigned char)logo_type, \
                           fmt,##__VA_ARGS__); \
} while(0)
#endif

#define TUYA_IPC_PR_UPLOAD(fmt, ...) IPC_PRINT_LOG_UP(tuya_ipc_log,fmt, ##__VA_ARGS__)

/**
* @brief      transfer errcode string to tuya errcoe
* @param[in]  err_val    error value string
* @return     OPERATE_RET
* - OPRT_OK   success
* - Others    failed
*/
OPERATE_RET tuya_httpc_order_request_errcode_transfer(char *err_val);

/**
* @brief      notify cloud storage, v1.0 version
* @param[in]  gw_id        gw_id
* @param[in]  cloud_type   request type \n
* @ref        0    notify cloud storage start \n
* @ref        1    notify cloud storage stop \n
* @ref        2    notify cloud storage event
* @param[out] result   result
* @return     OPERATE_RET
* - OPRT_OK   success
* - Others    failed
*/

OPERATE_RET tuya_httpc_ipc_oss_storage_cfg_get(IN CONST CHAR_T *param, OUT ty_cJSON **result);

/**
* @brief      post custom defined message to cloud
* @param[in]  api_name        api name to cloud
* @param[in]  api_version     api version, such as 1.0, 2.0
* @param[in]  message         upload message, jsong string
* @param[out] result          result, malloc memory in this api, please Free memory when result is processed
* @return     OPERATE_RET
* - OPRT_OK   success
* - Others    failed
*/
OPERATE_RET tuya_http_gw_ipc_custom_msg(IN CONST CHAR_T *api_name, IN CONST CHAR_T *api_version, IN CONST CHAR_T *message, OUT ty_cJSON **result);

/**
* @brief      init ipc log upload function
* @return     VOID
*/
VOID tuya_ipc_log_upload_init(VOID);

#if defined(ENABLE_CLOUD_OPERATION) && (ENABLE_CLOUD_OPERATION==1)
/**
* @brief      get storage config from cloud
* @param[out] result    result
* @param[in]  with_videostorage \n
* @ref        TRUE    for cloud storage \n
* @ref        FALSE   for notification
* @return     OPERATE_RET
* - OPRT_OK   success
* - Others    failed
*/
OPERATE_RET tuya_httpc_cloud_storage_cfg_get(OUT ty_cJSON **result, IN CONST BOOL_T with_videostorage);

/**
 * @brief get cloud config v3.1
 * 
 * @param result 
 * @param post_data 
 * @param data_len 
 * @return OPERATE_RET 
 */
OPERATE_RET tuya_httpc_cloud_config_get(OUT ty_cJSON **result, IN CONST CHAR_T *post_data, IN CONST INT_T data_len);

/**
* @brief      get common storage config from cloud
* @param[out] result    result
* @return     OPERATE_RET
* - OPRT_OK   success
* - Others    failed
*/
OPERATE_RET tuya_httpc_cloud_storage_common_cfg_get(OUT ty_cJSON **result);

/**
* @brief      get common storage config v4.0 from cloud
* @param[out] result    result
* @return     OPERATE_RET
* - OPRT_OK   success
* - Others    failed
*/
OPERATE_RET tuya_httpc_cloud_storage_common_cfg_get_v40(IN CONST UINT_T file_size, OUT ty_cJSON **result);

/**
* @brief      get unify token from cloud
* @return     result    result
* @return     OPERATE_RET
* - OPRT_OK   success
* - Others    failed
*/
OPERATE_RET tuya_httpc_cloud_unify_token_get(OUT ty_cJSON **result);

/**
* @brief      get ai face storage config from cloud
* @param[out] result    result
* @return     OPERATE_RET
* - OPRT_OK   success
* - Others    failed
*/
OPERATE_RET tuya_httpc_cloud_ai_face_storage_cfg_get(OUT ty_cJSON **result);

/**
* @brief      get storage restriction config
* @param[out] result    result
* @return     OPERATE_RET
* - OPRT_OK   success
* - Others    failed
*/
OPERATE_RET tuya_httpc_restriction_cfg_get(OUT ty_cJSON **result);

/**
* @brief      upload file complete
* @param[in]  time        time
* @param[in]  p_filetype  upload file type
* @param[in]  p_extend    extend string to cloud
* @param[in]  p_bucket    bucket
* @param[in]  p_file      upload file name
* @return     OPERATE_RET
* - OPRT_OK   success
* - Others    failed
*/
OPERATE_RET tuya_httpc_cloud_upload_complete(IN CONST UINT_T time,
                                                           IN CONST CHAR_T *p_filetype,
                                                           IN CONST CHAR_T *p_extend,
                                                           IN CONST CHAR_T *p_bucket,
                                                           IN CONST CHAR_T *p_file);

#endif

/**
* @brief      get cloud storage encrypt key from cloud, for nvr sub devices
* @param[in]  node_id    node_id
* @param[out] result    result
* @return     OPERATE_RET
* - OPRT_OK   success
* - Others    failed
*/
OPERATE_RET tuya_httpc_sub_cloud_storage_encrypt_key_get(IN CHAR_T *node_id, OUT ty_cJSON **result);


/**
* @brief      xvr get cloud config
* @param[in]  nodeId    xvr nodeId
* @param[in]  with_video_storage   video cloud config
* @return     result    result
* @return     OPERATE_RET
* - OPRT_OK   success
* - Others    failed
*/
OPERATE_RET tuya_httpc_xvr_cloud_storage_cfg_get(IN CHAR_T * nodeId, OUT cJSON **result, IN CONST BOOL_T with_video_storage);

/**
* @brief      xvr get unify token from cloud
* @param[in]  nodeId    xvr nodeId
* @return     result    result
* @return     OPERATE_RET
* - OPRT_OK   success
* - Others    failed
*/

OPERATE_RET tuya_httpc_xvr_cloud_unify_cfg_get(IN CHAR_T * nodeId, OUT ty_cJSON **result);
#ifdef __cplusplus
}
#endif
#endif

