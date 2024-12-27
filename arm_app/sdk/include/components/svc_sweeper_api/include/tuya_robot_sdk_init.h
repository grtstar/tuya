/**
 * @file tuya_robot_sdk_init.h
 * @brief This is tuya ipc sdk init file
 * @version 1.0
 * @date 2021-11-17
 *
 * @copyright Copyright 2021-2031 Tuya Inc. All Rights Reserved.
 *
 */

#ifndef __TUYA_ROBOT_SDK_INIT_H__
#define __TUYA_ROBOT_SDK_INIT_H__

#include <stdio.h>
#include <time.h>
#include "tuya_iot_config.h"
#include "tuya_cloud_wifi_defs.h"
#include "tuya_cloud_types.h"
#include "tuya_error_code.h"
#include "tuya_cloud_com_defs.h"
#include "cJSON.h"
#include "tuya_ipc_upgrade.h"
#if defined(ENABLE_IPC_GW_BOTH) && (ENABLE_IPC_GW_BOTH==1)
#include "tuya_gw_init_api.h"
#endif

#include "tuya_ipc_p2p.h"
#include "tuya_ipc_stream_storage.h"
#include "tuya_ipc_media_stream.h"
#include "tuya_album_sweeper_api.h"

#ifdef __cplusplus
extern "C" {
#endif


#define ROBOT_STORAGE_PATH_LEN 64
#define ROBOT_PRODUCT_KEY_LEN 16
#define ROBOT_UUID_LEN 25
#define ROBOT_AUTH_KEY_LEN 32
#define ROBOT_SW_VER_LEN 20
#define ROBOT_SERIAL_NUM_LEN 32
#define ROBOT_P2P_ID_LEN 25
#define ROBOT_FIRMWARE_KEY_LEN 16

typedef enum {
    ROBOT_NORMAL_POWER_DEV =0,
    ROBOT_LOW_POWER_DEV,
    ROBOT_DEV_TYPE_MAX
} TUYA_ROBOT_DEV_TYPE_E;


typedef enum {
	TUYA_ROBOT_DIRECT_CONNECT_QRCODE = 0, ///< direct connect mode, compared to wifi EZ mode, get ssid/psw via QR etc.
	TUYA_ROBOT_DIRECT_CONNECT_MAX,
} TUYA_ROBOT_DIRECT_CONNECT_TYPE_E;

typedef VOID (*TUYA_ROBOT_RESTART_INFORM_CB)(VOID);
/**
 * @brief reset callback function
 * 
 * @param[in] type reset type
 * 
 * @return VOID
 */
typedef VOID (*TUYA_ROBOT_RST_INFORM_CB)(GW_RESET_TYPE_E from);

#if defined(QRCODE_ACTIVE_MODE) && (QRCODE_ACTIVE_MODE==1)
typedef VOID(*TUYA_ROBOT_QRCODE_ACTIV_CB)(CHAR_T * shorturl);
#endif

/**
 * @brief robot init sdk param
 */
typedef struct {
    BOOL_T mem_save_mode;   ///< woking in low memory mode
    TUYA_ROBOT_DEV_TYPE_E dev_type;   ///< device type ,eg low power device
    VOID *p_reserved;
} TUYA_ROBOT_ENV_VAR_T;

typedef struct
{
	BOOL_T enable; /*enable == false ,other var is invalid*/
	UINT_T max_stream_client; /* stream clinet max connect number*/
    TRANS_DEFAULT_QUALITY_E live_mode;  /* for multi-streaming ipc, the default quality for live preview */
	MEDIA_STREAM_EVENT_CB media_event_cb; /*media event callback*/
	MEDIA_RECV_AUDIO_CB rev_audio_cb;/*app to dev audio callback*/
    MEDIA_RECV_VIDEO_CB rev_video_cb;/*app to dev audio callback*/
    MEDIA_RECV_FILE_CB rev_file_cb;/*app to dev file callback*/
    MEDIA_GET_SNAPSHOT_CB get_snapshot_cb;
}TUYA_SDK_MEDIA_ADAPTER_S;
/** @struct DEVICE_MEDIA_INFO_T
 */
typedef struct
{
    IPC_MEDIA_INFO_T av_encode_info;                ///< encoder info
    MEDIA_AUDIO_DECODE_INFO_T audio_decode_info;    ///< decoder info
    INT_T max_pic_len;                              ///< max picture size, in KB
}DEVICE_SDK_MEDIA_INFO_T;

/**
 * @brief initialize tuya robot event
 * 
 * @param[in] 
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_robot_event_init(VOID);

/**
 * @brief enable preson flow detect skill
 * 
 * @param VOID
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_robot_skill_enable_person_flow(VOID);

/**
 * @brief upload device skills to cloud after init success
 * 
 * @return VOID 
 */
VOID tuya_robot_upload_skills(VOID);

/**
 * @brief cascade skill enable, call before tuya_ipc_upload_skills
 * 
 * @param VOID
 * 
 * @return VOID 
 */
VOID tuya_robot_cascade_enable(VOID);


/**
 * @brief set SDK dns cache priority
 * 
 * @param[in] priority: 0 use region dns, 1 use random dns, 2 use system dns
 *  
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_robot_set_dns_cache_priority(IN CONST INT_T priority);

/**
 * @brief use this API to connect to specific wifi and register to tuya cloud, data can be obtain via QR code e.g.
 * 
 * @param[in] p_str: in format: {"p":"password_of_wifi","s":"ssid_of_wifi","t":"token_to_register"}
 * @param[in] source: only TUYA_ROBOT_DIRECT_CONNECT_QRCODE is supported now
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_robot_direct_connect(IN CONST CHAR_T *p_str, IN CONST TUYA_ROBOT_DIRECT_CONNECT_TYPE_E source);
/**
 * @brief use this API for robot sdk init in meger sdk.
 *
 * @param VOID
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_robot_service_event_subscribe(VOID);

/**
 * @brief get mqtt connection status, mqtt is used by tuya SDK to commnicate with tuya cloud
 * 
 * @param VOID 
 * 
 * @return 0:offline 1:online 
 */
INT_T tuya_robot_get_mqtt_status(VOID);

/**
 * @brief set mqtt connection status,  called by status cb. useless for customers.
 * 
 * @param stat: mqtt status, notified by mqtt event. 0:offline, 1:online
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_robot_set_mqtt_status(INT_T stat);

/**
 * @brief initialize tuya sdk for media adapter
 * 
 * @param[in] p_media_adatper_info: TUYA_SDK_MEDIA_ADAPTER_S 
 * @param[in] p_media_infos: DEVICE_SDK_MEDIA_INFO_T 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_sdk_media_adapter_init(TUYA_SDK_MEDIA_ADAPTER_S *p_media_adatper_info, DEVICE_SDK_MEDIA_INFO_T *p_media_infos);

/**
 * @brief  initialize media ring buffer init
 * @param  [DEVICE_SDK_MEDIA_INFO_T] pMediaInfo 
 * @param  [INT_T] max_buffer_seconds:   buffer timer 6~10S
 * @param  [INT_T] channel:  media channel
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_media_ring_buffer_init(CONST DEVICE_SDK_MEDIA_INFO_T *pMediaInfo, INT_T max_buffer_seconds, INT_T channel);

/**
 * @brief  initialize media alarm event init
 * @param  [*]
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_media_alarm_event_init(void);

/**
 * @brief  initialize media server customize init
 * @param  [in] max_client_num   range: 0 ~ 5
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_media_server_customize_init(int max_client_num);

/**
 * @brief  initialize media server init(default max_client_num = 5)
 * @param[*]
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_media_server_init(void);
/**
 * @brief  sweeper registration callback int
 * @param  [in] event_cb  
 * @return [*]OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_media_event_init(SWEEPER_FILE_STREAM_EVENT_CB event_cb);

/**
 * @brief  sweeper audio and video registration callback int
 * @param  [in] event_cb  
 * @return [*]OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_media_av_event_init(SWEEPER_AV_STREAM_EVENT_CB event_cb);

#ifdef __cplusplus
}
#endif

#endif

