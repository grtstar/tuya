#ifndef UPLOAD_NOTIFICATION_H
#define UPLOAD_NOTIFICATION_H

#include "tuya_cloud_types.h"
#include "tuya_cloud_error_code.h"
#include "cloud_operation.h"

#define  IPC_AES_ENCRYPT_KEY_LEN 16
#define  KEY_LEN (2*IPC_AES_ENCRYPT_KEY_LEN+1)
#define  INFO_LEN               256
#define  BUCKET_LEN             64


typedef PVOID_T Notify_Handler;

 typedef struct{
     CHAR_T key[KEY_LEN];
     CHAR_T info[INFO_LEN];
 }NOTIFICATION_MESSAGE_UNIT;

typedef struct{
   INT_T count;
   CHAR_T bucket[BUCKET_LEN];
   NOTIFICATION_MESSAGE_UNIT unit[0];
}NOTIFICATION_MESSAGE;


typedef enum {
    UPLOAD_NOTIFICATION, //for ipc log
    UPLOAD_COMMON, //for sweep-rebot
    UPLOAD_EVENT_UNIFY,
	UPLOAD_COMMON_V2, //for sweep-rebot
    UPLOAD_INVALID
}UPLOAD_CFG_E;


VOID notification_message_free(IN Notify_Handler message);

OPERATE_RET upload_notification_init(VOID);

OPERATE_RET notification_message_malloc(IN INT_T count, OUT Notify_Handler *message, uint32_t* size);

OPERATE_RET notification_content_upload_from_buffer(UPLOAD_CFG_E cfg_type, IN TIME_T current_time, IN CHAR_T* suffix,
                                    IN CHAR_T *data, IN INT_T data_len, IN BOOL_T encryptNeeded, OUT VOID *message);

OPERATE_RET notification_content_upload_from_custom_ctx(UPLOAD_CFG_E cfg_type, IN CHAR_T* content_type, IN CHAR_T *cloud_name, IN CLOUD_UPLOAD_PATH_E path,
                                                        IN CLOUD_CUSTOM_CONTENT_CTX_S *p_ctx, INOUT Notify_Handler message);

OPERATE_RET notification_content_upload_from_file(UPLOAD_CFG_E cfg_type, IN CHAR_T *local_name, IN CHAR_T* content_type, IN CHAR_T *cloud_name,
                                                  IN CLOUD_UPLOAD_PATH_E path, INOUT Notify_Handler message);

OPERATE_RET notification_message_upload(IN CONST BYTE_T mqtt_number, INOUT Notify_Handler message, IN UINT_T time_out);

#if 0
OPERATE_RET upload_recordFile(IN INT_T map_id, IN CHAR_T *layout_name, IN CHAR_T *route_name);
#endif

//废弃功能
OPERATE_RET notification_snapshot_message_upload(IN CONST BYTE_T snapshot, INOUT Notify_Handler message, IN UINT_T time_out);

INT_T notification_check_init(UPLOAD_CFG_E cfg_type);

OPERATE_RET notification_cloud_put_data_to_file(IN CONST UPLOAD_CFG_E cfg_type, IN CONST CHAR_T *cloud_name, IN CHAR_T *buf, IN UINT_T size);

OPERATE_RET __notification_content_upload_from_buffer(UPLOAD_CFG_E cfg_type, IN CHAR_T* content_type, IN CHAR_T *cloud_name, CLOUD_UPLOAD_PATH_E path,
                                                    IN CHAR_T*data, IN INT_T data_len, IN CHAR_T* secret_key, INOUT Notify_Handler message);

OPERATE_RET __notification_content_upload_from_buffer_subpack(UPLOAD_CFG_E cfg_type, IN CHAR_T* content_type, IN INT_T content_len, IN CHAR_T *cloud_name, CLOUD_UPLOAD_PATH_E path,
                                                    IN CHAR_T*data, IN INT_T data_len, IN CHAR_T* secret_key, INOUT Notify_Handler message);

OPERATE_RET __notification_content_upload_from_buffer_subpack_free(UPLOAD_CFG_E cfg_type);


#endif//UPLOAD_NOTIFICATION_H

