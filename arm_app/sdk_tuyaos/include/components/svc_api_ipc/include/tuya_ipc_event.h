/**
 * @file tuya_ipc_event.h
 * @brief This is tuya ipc event file
 * @version 1.0
 * @date 2021-11-17
 *
 * @copyright Copyright 2021-2031 Tuya Inc. All Rights Reserved.
 *
 */

#ifndef __EVENT_H__
#define __EVENT_H__

#include "tuya_ipc_api.h"
#include "tuya_ipc_cloud_storage.h"


#ifdef __cplusplus
extern "C" {
#endif


#define MAX_TEXT_LEN 32

#define RESOURCE_PIC    1
#define RESOURCE_TEXT   2
#define RESOURCE_VIDEO  4
#define RESOURCE_AUDIO  8

#define AES_KEY_LEN 16

#define ALARM_STORAGE_BASE    (0x1)

#define MAX_ALARM_BITMAP_NUM 32
#define MAX_ALARM_TYPE_NUM (MAX_ALARM_BITMAP_NUM << 5)


typedef enum {
    E_ALARM_MOTION        = 0,
    E_ALARM_DOORBELL      = 1,
    E_ALARM_DEV_LINK      = 2,
    E_ALARM_PASSBY        = 3,
    E_ALARM_LINGER        = 4,
    E_ALARM_MSG           = 5,
    E_ALARM_CONNECTED     = 6,
    E_ALARM_UNCONNECTED   = 7,
    E_ALARM_REFUSE        = 8,
    E_ALARM_HUMAN         = 9,
    E_ALARM_CAT           = 10,
    E_ALARM_CAR           = 11,
    E_ALARM_BABY_CRY      = 12,
    E_ALARM_BANG          = 13,
    E_ALARM_FACE          = 14,
    E_ALARM_ANTIBREAK     = 15,
    E_ALARM_RESERVE       = 16,
    E_ALARM_IO            = 17,
    E_ALARM_LOW_BATTERY   = 18,
    E_ALARM_DOORBELL_PRESS = 19,
    E_ALARM_COVER         = 20,
} TUYA_ALARM_TYPE_E;

typedef enum {
    E_ALARM_SD_STORAGE    = ALARM_STORAGE_BASE<<0,
    E_ALARM_CLOUD_STORAGE = ALARM_STORAGE_BASE<<1,
} TUYA_ALARM_STORAGE_TYPE_E;


typedef struct {
    INT_T text_type;//enum text
    CHAR_T text[MAX_TEXT_LEN];
} TUYA_ALARM_TEXT_T;

typedef struct {
    INT_T bitmap[MAX_ALARM_BITMAP_NUM];
} TUYA_ALARM_BITMAP_T;

typedef struct {
    INT_T type;                         ///< alarm type
    TUYA_ALARM_BITMAP_T wait_for;       ///< depends on other alarms
    INT_T is_notify;                    ///< notify to cloud
    TIME_T trigger_time;
    TIME_T upload_time;                 ///< 0 means immediately and -1 means wait until event end
    BOOL_T valid;                       ///< 1 means needs to handle and 0 means had handled. user should set to 1
    BOOL_T force;                       ///< force upload or not, independent with event like E_ALARM_COVER
    INT_T resource_type;                ///< refer to macro RESOURCE_XXX
    CHAR_T *extra_data;                 ///< extra data append to upload info, json format: "aaa":3. NULL if not need
    CHAR_T *pic_buf;
    INT_T pic_size;
    TUYA_ALARM_TEXT_T  *context;        ///< text alarm info, can be NULL
    INT_T context_cnt;                  ///< TUYA_ALARM_TEXT_T count, can be 0
    NOTIFICATION_UNIT_T *media;         ///< media info, can be NULL
    INT_T media_cnt;                    ///< NOTIFICATION_UNIT_T count, can be 0
    CHAR_T dev_id[DEV_ID_LEN+1];        ///< deive id
    INT_T  dev_chan_index;              ///< device channle
} TUYA_ALARM_T;


/**
 * @brief start an event
 * 
 * @param[in] alarms_types: TUYA_ALARM_TYPE_E
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_start_event(IN TUYA_ALARM_TYPE_E alarms_types);

/**
 * @brief start an event an with storage
 * 
 * @param[in] alarms_types: TUYA_ALARM_TYPE_E
 * @param[in] storage_type: OR operation with type TUYA_ALARM_STORAGE_TYPE_E
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_start_event_with_storage(IN TUYA_ALARM_TYPE_E alarms_types, IN INT_T storage_type);

/**
 * @brief start an event and only once
 * 
 * @param[in] alarms_types: TUYA_ALARM_TYPE_E
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_start_event_once(IN TUYA_ALARM_TYPE_E alarms_types);

/**
 * @brief stop an event
 * 
 * @param[in] alarms_types: TUYA_ALARM_TYPE_E
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_stop_event(IN TUYA_ALARM_TYPE_E alarms_types);

/**
 * @brief stop an event and stop storage
 * 
 * @param[in] alarms_types: TUYA_ALARM_TYPE_E
 * @param[in] storage_type: OR operation with type TUYA_ALARM_STORAGE_TYPE_E
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_stop_event_with_storage(IN TUYA_ALARM_TYPE_E alarms_types, IN INT_T storage_type);

/**
 * @brief stop an event and stop storage synchronous
 * 
 * @param[in] alarms_types: TUYA_ALARM_TYPE_E
 * @param[in] storage_type: OR operation with type TUYA_ALARM_STORAGE_TYPE_E
 * @param[in] timeout: will return until after timeout*2*500 ms or success
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_stop_event_with_storage_sync(IN TUYA_ALARM_TYPE_E alarms_types, IN INT_T storage_type, IN INT_T timeout);

/**
 * @brief check alarm event begin or not
 * 
 * @param[in] alarms_types: TUYA_ALARM_TYPE_E
 * 
 * @return TRUE/FALSE
 */
BOOL_T tuya_ipc_event_check_status(IN TUYA_ALARM_TYPE_E alarms_types);

/**
 * @brief check storage is working or not
 * 
 * @param[in] storage_type: OR operation with type TUYA_ALARM_STORAGE_TYPE_E
 * 
 * @return TRUE/FALSE 
 */
BOOL_T tuya_ipc_event_check_status_with_storage(IN INT_T storage_type);

/**
 * @brief notify an alarm base on event
 * 
 * @param[in] alarm: TUYA_ALARM_T
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_alarm_event(IN TUYA_ALARM_T *alarm);

/**
 * @brief notify an alarm not base on event
 * 
 * @param alarm: TUYA_ALARM_T
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_trigger_alarm_without_event(IN TUYA_ALARM_T *alarm);

/**
 * @brief notify event module device is online, SDK can abtain cofig as soon as possible
 * 
 * @param VOID 
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_event_set_online(VOID);

/**
 * @brief add alarm info bitmap
 * 
 * @param[in] bitmap: TUYA_ALARM_BITMAP_T
 * @param[in] alarm_types: TUYA_ALARM_BITMAP_T
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_event_add_alarm_types(IN TUYA_ALARM_BITMAP_T *bitmap, IN TUYA_ALARM_TYPE_E alarm_types);

/**
 * @brief init event module
 * 
 * @param[in] max_event: number of event, like [video detect] [audio detect] [doorbell]. alarm upload depends on priority in same type
 * @param[in] alarms_of_event: alarms in every single event, usually is a TUYA_ALARM_BITMAP_T array and items number is max_event
 * @param[in] alarm_cnt_per_event: every single event support number of notify in same time
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_event_module_init(IN INT_T max_event, IN TUYA_ALARM_BITMAP_T *alarms_of_event, IN INT_T alarm_cnt_per_event);

/**
 * @brief exit event module release resource
 * 
 * @param VOID 
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_event_module_exit(VOID);

/**
 * @brief get string cmd of alarm. like: 0 -> ipc_motion
 * 
 * @param id :TUYA_ALARM_TYPE_E
 * @param cmd_buf 
 * @param cmd_buf_len :max len is MAX_TEXT_LEN
 * @return OPERATE_RET 
 */
OPERATE_RET tuya_ipc_event_alarm_id_to_cmd(INT_T id, CHAR_T *cmd_buf, INT_T cmd_buf_len);

#ifdef __cplusplus
}
#endif

#endif

