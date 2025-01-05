/**
* @file      upload_notification.h
* @brief     upload notification api
* @version   1.0
* @date      2021-11-19
*
* copyright  Copyright (c) tuya.inc 2021
*/
#ifndef __UPLOAD_NOTIFICATION_H__
#define __UPLOAD_NOTIFICATION_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "tuya_cloud_types.h"
#include "cloud_operation.h"

typedef PVOID_T Notify_Handler_T;

#define  NOTIFY_AES_ENCRYPT_KEY_LEN    16
#define  NOTIFY_KEY_LEN                (2 * NOTIFY_AES_ENCRYPT_KEY_LEN + 1)
#define  NOTIFY_INFO_LEN               256
#define  NOTIFY_BUCKET_LEN             64

/**
* @struct  NOTIFICATION_MESSAGE_UNIT_T
* @brief   notification unit
*/
typedef struct {
    CHAR_T key[NOTIFY_KEY_LEN];
    CHAR_T info[NOTIFY_INFO_LEN];
}NOTIFICATION_MESSAGE_UNIT_T;

/**
* @struct  NOTIFICATION_MESSAGE_T
* @brief   notification message
*/
typedef struct {
   INT_T count;
   CHAR_T bucket[NOTIFY_BUCKET_LEN];
   NOTIFICATION_MESSAGE_UNIT_T unit[0];
}NOTIFICATION_MESSAGE_T;

/**
* @enum  UPLOAD_CFG_E
* @brief upload config enum
*/
typedef enum {
    UPLOAD_NOTIFICATION,    /** for ipc log */
    UPLOAD_COMMON,          /** for sweep-robot */
    UPLOAD_EVENT_UNIFY,     /** for unify notification */
    UPLOAD_COMMON_V2,       /** for sweep-robot */
    UPLOAD_INVALID
}UPLOAD_CFG_E;

/**
* @struct  CLOUD_CFG_CTX_T
* @brief cloud config ctx
*/
typedef struct {
    INT_T inited;
    TIME_T  update_time;    /** cloud config update time */
    TIME_T  expire_time;    /** cloud config expire time */
    CLOUD_CONFIG_T config;  /** cloud config */
    MUTEX_HANDLE mutex;

    // subpack upload
    PVOID_T handle;
    INT_T total_len;
    INT_T upload_len;
}CLOUD_CFG_CTX_T;

/**
* @struct  TUYA_NOTIFICATION_CONTENT_CTX_T
* @brief   notification content ctx
*/
typedef struct {
    INT_T index;               /** device index, 0 for ipc */
    CHAR_T *dev_id;            /** dev_id, NULL for ipc */
    TIME_T current_time;
    CHAR_T *suffix;
    CHAR_T *data;
    INT_T data_len;
    INT_T content_len;
    CHAR_T *secret_key;
    CHAR_T *content_type;
    CHAR_T *content_name;
    BOOL_T no_encrypt;         /** TRUE: send content to cloud without encryption for pic */
    UPLOAD_CFG_E cfg_type;     /** upload config type */
    CLOUD_CFG_CTX_T *cloud_cfg_ctx_ptr;
    Notify_Handler_T message;
} TUYA_NOTIFICATION_CONTENT_CTX_T;

typedef struct {
    INT_T index;                /** device index, 0 for ipc */
    CHAR_T *dev_id;             /** dev_id, NULL for ipc */
    BYTE_T notification_dp;     /** notification dp */
    UINT_T time_out;
    CHAR_T *notify_cmd;
    CHAR_T *notify_with;
    CHAR_T *notify_type;
    CHAR_T *extra_data;
    BOOL_T is_alarm;            /** TRUE: send notification to message center */
    TIME_T start_time;
    UPLOAD_CFG_E cfg_type;      /** upload config type */
    Notify_Handler_T message;
} TUYA_NOTIFICATION_MESSAGE_CTX_T;

/**
* @struct  TUYA_NOTIFICATION_SNAPSHOT_MESSAGE_CTX_T;
* @brief   notification snapshot message ctx
*/
typedef struct {
    CHAR_T *dev_id;            /** dev_id, NULL for ipc */
    INT_T index;               /** device index, 0 for ipc */
    CLOUD_CFG_CTX_T *cloud_cfg_ctx_ptr;
    BYTE_T snapshot_dp;
    Notify_Handler_T message;
    UINT_T time_out;
} TUYA_NOTIFICATION_SNAPSHOT_MESSAGE_CTX_T;

/**
* @brief      upload notification module init
* @return     OPERATE_RET
* - OPRT_OK   init success
* - Others    init failed
*/
OPERATE_RET upload_notification_init(VOID);

/**
* @brief      malloc memory for message notification
* @param[in]  count    the count of resources in message
* @param[out] message  notify handler, must use API tuya_notification_message_free after message uploaded
* @param[out] size     memory size malloced for notify handler
* @return     OPERATE_RET
* - OPRT_OK   success
* - Others    failed
*/
OPERATE_RET tuya_notification_message_malloc(IN INT_T count, OUT Notify_Handler_T *message, OUT UINT_T *size);

/**
* @brief      free memory malloced by API tuya_notification_message_malloc
* @param[in]  message    notify handler malloced by API tuya_notification_message_malloc
* @return     VOID
*/
VOID tuya_notification_message_free(IN Notify_Handler_T message);

/**
* @brief      get config instance
* @param[in]  index    dev index ,defualt 0 for ipc
* @param[in]  cfg_type        config type
* @param[out] ctx     config ctx
* @return     OPERATE_RET
* - OPRT_OK   success
* - Others    failed
*/
OPERATE_RET tuya_notify_cloud_config_instance_get(IN CONST INT_T index, IN CONST UPLOAD_CFG_E cfg_type, OUT CLOUD_CFG_CTX_T * ctx);

/**
* @brief      upload content (all) to cloud
* @param[in]  content_ctx    content
* @return     OPERATE_RET
*/
OPERATE_RET tuya_notification_content_upload(TUYA_NOTIFICATION_CONTENT_CTX_T * content_ctx);
/**
* @brief        upload content (image/video/...) to cloud
* @param[in]    cfg_type        config type
* @param[in]    current_time    current utc time
* @param[in]    suffix          suffix
* @param[in]    data            data to be uploaded
* @param[in]    data_len        length of data
* @param[in]    encrypt_needed  whether encrypt uploaded data
* @param[inout] message         notify handler malloced by API tuya_notification_message_malloc
* @return       OPERATE_RET
* - OPRT_OK     success
* - Others      failed
*/
OPERATE_RET tuya_notification_content_upload_from_buffer(IN UPLOAD_CFG_E cfg_type,
                                                                             IN TIME_T current_time,
                                                                             IN CHAR_T *suffix,
                                                                             IN CHAR_T *data,
                                                                             IN INT_T data_len,
                                                                             IN BOOL_T encrypt_needed,
                                                                             INOUT Notify_Handler_T *message);

/**
* @brief        upload content (image/video/...) to cloud by subpack
* @param[in]    cfg_type        config type
* @param[in]    content_type    content type
* @param[in]    cloud_name      file name in cloud
* @param[in]    path            upload path
* @param[in]    data            upload data
* @param[in]    data_len        data length
* @param[in]    secret_key      secret key
* @param[in]    message         message
* @return       OPERATE_RET
* - OPRT_OK     success
* - Others      failed
*/
OPERATE_RET tuya_notification_content_upload_from_buffer_subpack(UPLOAD_CFG_E cfg_type,
                                                                IN CHAR_T *content_type,
                                                                IN INT_T content_len,
                                                                IN CHAR_T *cloud_name,
                                                                CLOUD_UPLOAD_PATH_E path,
                                                                IN CHAR_T *data,
                                                                IN INT_T data_len,
                                                                IN CHAR_T *secret_key,
                                                                INOUT Notify_Handler_T message);

/**
* @brief      free upload subpack resource
* @param[in]  config type
* @return       OPERATE_RET
* - OPRT_OK     success
* - Others      failed
*/
OPERATE_RET tuya_notification_content_upload_from_buffer_subpack_free(UPLOAD_CFG_E cfg_type);

/**
* @brief        upload custom content to cloud
* @param[in]    cfg_type        config type
* @param[in]    content_type    content type
* @param[in]    cloud_name      filename in cloud
* @param[in]    path            upload path
* @param[in]    p_ctx           custom content ctx
* @param[inout] message         notify handler malloced by API tuya_notification_message_malloc
* @return       OPERATE_RET
* - OPRT_OK     success
* - Others      failed
*/
OPERATE_RET tuya_notification_content_upload_from_custom_ctx(IN UPLOAD_CFG_E cfg_type,
                                                                                   IN CHAR_T *content_type,
                                                                                   IN CHAR_T *cloud_name,
                                                                                   IN CLOUD_UPLOAD_PATH_E path,
                                                                                   IN CLOUD_CUSTOM_CONTENT_CTX_T *p_ctx,
                                                                                   INOUT Notify_Handler_T message);

/**
* @brief        upload local file to cloud
* @param[in]    cfg_type        config type
* @param[in]    local_name      local filename to be uploaded
* @param[in]    content_type    content type
* @param[in]    cloud_name      filename in cloud
* @param[in]    path            upload path
* @param[inout] message         notify handler malloced by API tuya_notification_message_malloc
* @return       OPERATE_RET
* - OPRT_OK     success
* - Others      failed
*/
OPERATE_RET tuya_notification_content_upload_from_file(IN UPLOAD_CFG_E cfg_type,
                                                                          IN CHAR_T *local_name,
                                                                          IN CHAR_T *content_type,
                                                                          IN CHAR_T *cloud_name,
                                                                          IN CLOUD_UPLOAD_PATH_E path,
                                                                          INOUT Notify_Handler_T message);

/**
* @brief        upload generic content (image/video/...) to cloud
* @param[in]    current_time    current utc time
* @param[in]    suffix          suffix
* @param[in]    data            data to be uploaded
* @param[in]    data_len        length of data
* @param[in]    secret_key      secret key used for data encryption
* @param[inout] message         notify handler malloced by API tuya_notification_message_malloc
* @return       OPERATE_RET
* - OPRT_OK     success
* - Others      failed
*/
OPERATE_RET tuya_notification_content_upload_from_buffer_generic(IN TIME_T current_time,
                                                                                         IN CHAR_T *suffix,
                                                                                         IN CHAR_T *data,
                                                                                         IN INT_T data_len,
                                                                                         IN CHAR_T *secret_key,
                                                                                         INOUT Notify_Handler_T *message);

/**
* @brief        upload unify content (image/video/...) to cloud
* @param[in]    cfg_type        config type
* @param[in]    current_time    current utc time
* @param[in]    suffix          suffix
* @param[in]    data            data to be uploaded
* @param[in]    data_len        length of data
* @param[in]    secret_key      secret key used for data encryption
* @param[inout] message         notify handler malloced by API tuya_notification_message_malloc
* @return       OPERATE_RET
* - OPRT_OK     success
* - Others      failed
*/
OPERATE_RET tuya_notification_content_upload_from_buffer_unify(IN UPLOAD_CFG_E cfg_type,
                                                                                      IN TIME_T current_time,
                                                                                      IN CHAR_T *suffix,
                                                                                      IN CHAR_T *data,
                                                                                      IN INT_T data_len,
                                                                                      IN CHAR_T *secret_key,
                                                                                      INOUT Notify_Handler_T *message);

/**
* @brief        upload notification message to cloud
* @param[in]    notification   notification dp id
* @param[in]    message        notify handler malloced by API tuya_notification_message_malloc
* @param[in]    time_out       timeout in seconds
* @return       OPERATE_RET
* - OPRT_OK     success
* - Others      failed
*/
OPERATE_RET tuya_notification_message_upload(IN CONST BYTE_T notification, INOUT Notify_Handler_T message, IN UINT_T time_out);

/**
* @brief        upload generic notification message to cloud
* @param[in]    notification    notification dp id
* @param[in]    message         notify handler malloced by API tuya_notification_message_malloc
* @param[in]    time_out        timeout in seconds
* @param[in]    notify_cmd      notify cmd
* @param[in]    notify_with     string in "with"
* @param[in]    notify_type     notify type
* @param[in]    extra_data      extra data
* @return       OPERATE_RET
* - OPRT_OK     success
* - Others      failed
*/
OPERATE_RET tuya_notification_message_upload_generic(IN CONST BYTE_T notification,
                                                                       IN Notify_Handler_T *message,
                                                                       IN UINT_T time_out,
                                                                       IN CHAR_T *notify_cmd,
                                                                       IN CHAR_T *notify_with,
                                                                       IN CHAR_T *notify_type,
                                                                       IN CHAR_T *extra_data);

/**
* @brief        upload unify notification message to cloud
* @param[in]    notification    notification dp id
* @param[in]    message         notify handler malloced by API tuya_notification_message_malloc
* @param[in]    time_out        timeout in seconds
* @param[in]    notify_cmd      notify cmd
* @param[in]    notify_with     string in "with"
* @param[in]    notify_type     notify type
* @param[in]    extra_data      extra data
* @param[in]    is_alarm        upload message to message center if TRUE
* @param[in]    start_time      message start utc time
* @return       OPERATE_RET
* - OPRT_OK     success
* - Others      failed
*/
OPERATE_RET tuya_notification_message_upload_for_event_unify(IN CONST BYTE_T notification,
                                                                                   IN VOID *message,
                                                                                   IN UINT_T time_out,
                                                                                   IN CHAR_T * notify_cmd,
                                                                                   IN CHAR_T *notify_with,
                                                                                   IN CHAR_T *notify_type,
                                                                                   IN CHAR_T *extra_data,
                                                                                   IN BOOL_T is_alarm,
                                                                                   IN TIME_T start_time);

/**
* @brief        upload map data to cloud
* @param[in]    buf              data to be uploaded
* @param[in]    len              data length in buf
* @param[in]    cloud_file_name  file name in cloud
* @param[in]    map_type         map type
* @param[in]    send_mqtt        send mqtt message if TRUE
* @param[in]    extend_msg       extend message
* @param[in]    http_update      send http update message to cloud if TRUE
* @return       OPERATE_RET
* - OPRT_OK     success
* - Others      failed
*/
OPERATE_RET upload_map_custom_buffer(IN CONST INT_T map_id,
                                                       IN CONST BYTE_T *buf,
                                                       IN CONST UINT_T len,
                                                       IN CONST CHAR_T *cloud_file_name,
                                                       IN CONST INT_T map_type,
                                                       IN CONST BOOL_T send_mqtt,
                                                       IN CONST CHAR_T *extend_msg,
                                                       IN CONST BOOL_T http_update);

/**
* @brief        upload local map file to cloud
* @param[in]    local_name    local filename
* @param[in]    map_type      map type
* @param[in]    send_mqtt     send mqtt message if TRUE
* @param[in]    extend_msg    extend message
* @param[in]    http_update   send http update message to cloud if TRUE
* @return       OPERATE_RET
* - OPRT_OK     success
* - Others      failed
*/
OPERATE_RET upload_map_custom_file(IN CONST INT_T map_id,
                                                    IN CONST CHAR_T *local_name,
                                                    IN CONST CHAR_T *cloud_file_name,
                                                    IN CONST INT_T map_type,
                                                    IN CONST BOOL_T send_mqtt,
                                                    IN CONST CHAR_T *extend_msg,
                                                    IN CONST BOOL_T http_update);

/**
* @brief        upload map custom buffer with encrypt
* @param[in]    cfg_type    upload config type
* @param[in]    cloud_name  file name in cloud
* @param[in]    path        upload path
* @param[in]    data        upload data
* @param[in]    data_len    data length
* @param[in]    secret_key  secret key
* @param[out]   encrypt_buf_len    encrypt buf length
* @param[inout] message     message
*/
OPERATE_RET upload_map_custom_buffer_with_encrypt(UPLOAD_CFG_E cfg_type,
                                                        IN CHAR_T *content_type,
                                                        IN CHAR_T *cloud_name,
                                                        IN CLOUD_UPLOAD_PATH_E path,
                                                        IN CHAR_T *data,
                                                        IN INT_T data_len,
                                                        IN CHAR_T *secret_key,
                                                        OUT UINT_T *encrpt_buf_len,
                                                        INOUT Notify_Handler_T message);
/**
* @brief        upload snapshot notification message to cloud
* @param[in]    message_ptr    snapshot notification message
* @return       OPERATE_RET
* - OPRT_OK     success
* - Others      failed
*/
OPERATE_RET tuya_notify_snapshot_message_upload(TUYA_NOTIFICATION_SNAPSHOT_MESSAGE_CTX_T *message_ptr);

/**
* @brief        upload notification message to cloud
* @param[in]    message_ptr    notification message
* @return       OPERATE_RET
* - OPRT_OK     success
* - Others      failed
*/
OPERATE_RET tuya_notify_message_upload(TUYA_NOTIFICATION_MESSAGE_CTX_T *message_ptr);

/**
* @brief        upload notification content (image/video/...) to cloud
* @param[in]    content_ctx_ptr    notification content
* @return       OPERATE_RET
* - OPRT_OK     success
* - Others      failed
*/
OPERATE_RET tuya_notify_content_upload(TUYA_NOTIFICATION_CONTENT_CTX_T *content_ctx_ptr);

/**
* @brief        notification check init
* @param[in]    index       device index, 0 for ipc
* @param[in]    cfg_type    config type
* @return       OPERATE_RET
* - OPRT_OK     success
* - Others      failed
*/
INT_T tuya_notification_check_init(INT_T index, UPLOAD_CFG_E cfg_type);

/**
* @brief        notification check start
* @return       INT_T
* - OPRT_OK     success
*/
INT_T notification_check_start();

/**
* @brief        get security level for notification
* @param[in]    index       device index, 0 for ipc
* @param[in]    cfg_type    config type
* @return       security level
*/
INT_T tuya_notification_get_security_level(INT_T index, UPLOAD_CFG_E cfg_type);

#ifdef __cplusplus
}
#endif

#endif // __UPLOAD_NOTIFICATION_H__

