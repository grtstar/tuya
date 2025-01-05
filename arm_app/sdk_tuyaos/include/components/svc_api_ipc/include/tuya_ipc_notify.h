/**
 * @file tuya_ipc_notify.h
 * @brief This is tuya ipc notify file
 * @version 1.0
 * @date 2021-11-17
 *
 * @copyright Copyright 2021-2031 Tuya Inc. All Rights Reserved.
 *
 */

#ifndef __TUYA_IPC_NOTIFY_H__
#define __TUYA_IPC_NOTIFY_H__

#include <time.h>
#include <stdio.h>
#include "tuya_iot_config.h"
#include "tuya_cloud_wifi_defs.h"
#include "tuya_cloud_types.h"
#include "tuya_error_code.h"
#include "tuya_cloud_com_defs.h"


#ifdef __cplusplus
extern "C" {
#endif


#define IPC_AES_ENCRYPT_KEY_LEN 16
#define IPC_NOTIFY_INFO_LEN 16

#define NOTIFICATION_MOTION_DETECTION 115


/**
 * notification type of the event, currently only JPEG and PNG are supported
 * NOTIFICATION_CONTENT_TYPE_E
 */
typedef enum {
    NOTIFICATION_CONTENT_MP4 = 0,   ///< mp4 for video, NOT supported now
    NOTIFICATION_CONTENT_JPEG,      ///< jpeg for snapshot
    NOTIFICATION_CONTENT_PNG,       ///< png for snapshot
    NOTIFICATION_CONTENT_AUDIO,     ///< audio for video msg
    NOTIFICATION_CONTENT_MAX,
} NOTIFICATION_CONTENT_TYPE_E;

typedef enum {
    NOTIFICATION_NAME_MOTION = 0,             ///< motion detect
    NOTIFICATION_NAME_DOORBELL = 1,           ///< doorbell press
    NOTIFICATION_NAME_DEV_LINK = 2,           ///< iot device link
    NOTIFICATION_NAME_PASSBY = 3,             ///< normal pass
    NOTIFICATION_NAME_LINGER = 4,             ///< abnormal linger
    NOTIFICATION_NAME_MESSAGE = 5,            ///< leave message info
    NOTIFICATION_NAME_CALL_ACCEPT = 6,        ///< doorbell call accept
    NOTIFICATION_NAME_CALL_NOT_ACCEPT = 7,    ///< doorbell call not accept
    NOTIFICATION_NAME_CALL_REFUSE = 8,        ///< doorbell call refuse
    NOTIFICATION_NAME_HUMAN = 9,              ///< human detect
    NOTIFICATION_NAME_PCD = 10,               ///< pet detect
    NOTIFICATION_NAME_CAR = 11,               ///< car detect
    NOTIFICATION_NAME_BABY_CRY = 12,          ///< baby cry
    NOTIFICATION_NAME_ABNORMAL_SOUND = 13,    ///< abnormal sound
    NOTIFICATION_NAME_FACE = 14,              ///< face detect
    NOTIFICATION_NAME_ANTIBREAK = 15,         ///< anti break
    NOTIFICATION_NAME_RECORD_ONLY = 16,       ///< placeholder, mean nothing
    NOTIFICATION_NAME_IO_ALARM =17,           ///< local IO
    NOTIFICATION_NAME_LOW_BATTERY = 18,       ///< low battery
    NOTIFICATION_NAME_NOUSE = 19,             ///< ignore this
    NOTIFICATION_NAME_COVER = 20,             ///< cover
    NOTIFICATION_NAME_FULL_BATTERY = 21,      ///< full battery
    NOTIFICATION_NAME_USER_IO = 22,           ///< local IO device
    NOTIFY_TYPE_MAX = 1024
} NOTIFICATION_NAME_E;

typedef enum {
    DOORBELL_NORMAL,    ///< normal doorbell, push image
    DOORBELL_AC,        ///< AC doorbell, push P2P
    DOORBELL_TYPE_MAX,
} DOORBELL_TYPE_E;


typedef struct {
    char *data;
    int len;
    char secret_key[IPC_AES_ENCRYPT_KEY_LEN + 1];
    NOTIFICATION_CONTENT_TYPE_E type;
} NOTIFICATION_UNIT_T;

typedef struct {
    INT_T unit_count;
    NOTIFICATION_NAME_E name;
    CHAR_T *extra_data; ///< default:NULL
    CHAR_T type[IPC_NOTIFY_INFO_LEN];   ///< jpg:"image",video:""
    CHAR_T with[IPC_NOTIFY_INFO_LEN];   ///< default "resources"
    NOTIFICATION_UNIT_T unit[0];
} NOTIFICATION_DATA_GENERIC_T;

typedef struct {
    CHAR_T cmd[64];         ///< event
    CHAR_T sub_version[32]; ///< version
    CHAR_T event_type[64];  ///< event type
    CHAR_T event_id[64];    ///< event ID
    INT_T   notify_message_center; ///< 0 not notify, 1 notify
    INT_T body_len;
    CHAR_T *body_data;
} TUYA_IPC_AI_EVENT_CTX_T;


/**
 * @brief init notify module lock
 * 
 * @param VOID 
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_notify_init_lock(VOID);

/**
 * @brief send a motion-detecttion alarm to tuya cloud and APP
 * 
 * @param[in] snap_buffer: address of current snapshot
 * @param[in] snap_size: size of snapshot, in Byte
 * @param[in] type: snapshot file type, jpeg or png
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_notify_motion_detect(IN CONST CHAR_T *snap_buffer, IN CONST UINT_T snap_size, IN CONST NOTIFICATION_CONTENT_TYPE_E type);

/**
 * @brief send a doorbell pressing message to tuya cloud and APP
 * 
 * @param[in] snap_buffer: address of current snapshot
 * @param[in] snap_size: size of snapshot, in Byte
 * @param[in] type: snapshot file type, jpeg or png
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 * 
 * @warning this API will be abandoned later. Use tuya_ipc_door_bell_press instead
 */
OPERATE_RET tuya_ipc_notify_door_bell_press(IN CONST CHAR_T *snap_buffer, IN CONST UINT_T snap_size, IN CONST NOTIFICATION_CONTENT_TYPE_E type);

/**
 * @brief send a doorbell pressing message to tuya cloud and APP
 * 
 * @param[in] doorbell_type: DOORBELL_NORMAL or DOORBELL_AC
 * @param[in] snap_buffer: address of current snapshot
 * @param[in] snap_size: size of snapshot, in Byte
 * @param[in] type: snapshot file type, jpeg or png
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_door_bell_press(IN CONST DOORBELL_TYPE_E doorbell_type, IN CONST CHAR_T *snap_buffer, IN CONST UINT_T snap_size, IN CONST NOTIFICATION_CONTENT_TYPE_E type);

/**
 * @brief send a doorbell pressing stop message to tuya cloud and APP(only for ac doorbell)
 * 
 * @param VOID
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
*/
OPERATE_RET tuya_ipc_door_bell_press_stop(VOID);
/***********************************************************
*  Function: tuya_ipc_door_bell_press_cancel
*  Description: cancel ac doorbell
*  Input: none
*  Output: none
*  Return: Return: sucess: OPRT_OK.failed :not OPRT_OK;
***********************************************************/
OPERATE_RET tuya_ipc_door_bell_press_cancel();

/**
 * @brief send a editable alarm to tuya cloud and APP
 * 
 * @param[in] snap_buffer: address of current snapshot
 * @param[in] snap_size: size fo snapshot, in Byte
 * @param[in] type: snapshot file type, jpeg or png
 * @param[in] name: editable event type, NOTIFICATION_NAME_E
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_notify_with_event(IN CONST CHAR_T *snap_buffer, IN CONST UINT_T snap_size, IN CONST NOTIFICATION_CONTENT_TYPE_E type, IN CONST NOTIFICATION_NAME_E name);

/**
 * @brief start storage
 * 
 * @param[in] storage_type: OR operation use type TUYA_ALARM_STORAGE_TYPE_E
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_start_storage(IN INT_T storage_type);

/**
 * @brief stop storage
 * 
 * @param[in] storage_type: OR operation use type TUYA_ALARM_STORAGE_TYPE_E
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_stop_storage(IN INT_T storage_type);

/**
 * @brief send a editable alarm to tuya cloud and APP
 * 
 * @param[in] snap_buffer: address of current snapshot
 * @param[in] snap_size: size fo snapshot, in Byte
 * @param[in] name: editable event type, NOTIFICATION_NAME_E
 * @param[in] is_notify: send to message center if TRUE
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_notify_alarm(IN CONST CHAR_T *snap_buffer, IN CONST UINT_T snap_size, IN CONST NOTIFICATION_NAME_E name, IN BOOL_T is_notify);

/**
 * @brief send customized mqtt message to tuya cloud
 * 
 * @param[in] protocol: mqtt protocol number
 * @param[in] p_data: protocol data
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_send_custom_mqtt_msg(IN CONST UINT_T protocol, IN CONST CHAR_T *p_data);

/**
 * @brief report living message to tuya APP
 * 
 * @param[in] error_code: 0 success, others fail
 * @param[in] force: useless now
 * @param[in] timeout: useless now
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_report_living_msg(IN UINT_T error_code, IN UINT_T force, IN UINT_T timeout);

/**
 * @brief notify an ai event
 * 
 * @param[in] ctx: TUYA_IPC_AI_EVENT_CTX_T
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_ai_event_notify(IN TUYA_IPC_AI_EVENT_CTX_T *ctx);

/**
 * @brief upload ai image
 * 
 * @param[in] image_buf: image buffer
 * @param[in] image_size: image size
 * @param[out] result: result in json format
 * @param[in] result_len: can not less 256,must >=256
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_ai_image_upload(IN CHAR_T *image_buf, IN UINT_T image_size, OUT CHAR_T *result, IN UINT_T result_len);


#ifdef __cplusplus
}
#endif

#endif

