/**
* @file     tuya_ipc_cloud_storage.h
* @brief    cloud storage for video and audio stream
* @version  1.0
* @date     2021-11-16
* 
* copyright Copyright (c) tuya.inc 2021
*/
#ifndef __TUYA_IPC_CLOUD_STORAGE_H__
#define __TUYA_IPC_CLOUD_STORAGE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "tuya_cloud_types.h"
#include "tuya_error_code.h"
#include "tal_symmetry.h"
#include "tuya_ipc_media.h"

#define INVALID_EVENT_ID                    0xFFFFFFFF

typedef UINT_T EVENT_ID;

/**
* @enum     EVENT_STATUS_E
* @brief    cloud storage event status
*/
typedef enum {
    EVENT_NONE,
    EVENT_ONGOING,
    EVENT_READY,
    EVENT_INVALID
}EVENT_STATUS_E;

/**
* @enum     ClOUD_STORAGE_EVENT_TYPE_E
* @brief    cloud storage event type
*/
typedef enum {
    EVENT_TYPE_MOTION_DETECT = 0,   /** ipc_event_motion,"ipc_motion"*/
    EVENT_TYPE_DOOR_BELL = 1,       /** ipc_event_doorbell,"ipc_doorbell"*/
    EVENT_TYPE_DEV_LINK = 2,        /** event triggerred by other tuya cloud device*/
    EVENT_TYPE_PASSBY = 3,          /** for doorbell, detect someone passby */
    EVENT_TYPE_LINGER = 4,          /** for doorbell, detect someone linger */
    EVENT_TYPE_LEAVE_MESSAGE = 5,   /** for doorbell, a video message leaved */
    EVENT_TYPE_CALL_ACCEPT = 6,     /** for doorbell, call is accepted */
    EVENT_TYPE_CALL_NOT_ACCEPT = 7, /** for doorbell, call is not accepted */
    EVENT_TYPE_CALL_REFUSE = 8,     /** ipc_event_call_refuse*/
    EVENT_TYPE_HUMAN_DETECT = 9,    /** ipc_event_human,"ipc_human"*/
    EVENT_TYPE_CAT_DETECT = 10,     /** ipc_event_cat,"ipc_cat"*/
    EVENT_TYPE_CAR_DETECT = 11,     /** ipc_event_car,"ipc_car"*/
    EVENT_TYPE_BABY_CRY = 12,       /** ipc_event_baby_cry,"ipc_baby_cry"*/
    EVENT_TYPE_BANG = 13,           /** ipc_event_bang,"ipc_bang"*/
    EVENT_TYPE_FACE = 14,           /** ipc_event_face,"ipc_face"*/
    EVENT_TYPE_ANTIBREAK = 15,      /** ipc_event_antibreak,"ipc_antibreak"*/
    EVENT_TYPE_RECORD_ONLY = 16,    /** only to start event record, no event reported or will be reported later */
    EVENT_TYPE_IO_ALARM = 17,       /** event triggerred by local IO devices */
    EVENT_TYPE_INVALID
}ClOUD_STORAGE_EVENT_TYPE_E;

typedef enum
{
    E_STREAM_VIDEO_MAIN_CLARITY = 0,//主码流云存index
    E_STREAM_VIDEO_SUB_CLARITY
}XVR_CLOUD_STORAGE_CLARITY_E;
typedef enum
{
    E_STREAM_AUDIO_MAIN_CHANNEL = 0,//主音频通道
    E_STREAM_AUDIO_SUB_CHANNEL
}XVR_CLOUD_STORAGE_AUDIO_CHANNEL_E;
typedef struct {
    XVR_CLOUD_STORAGE_CLARITY_E stream_clarity;
    XVR_CLOUD_STORAGE_AUDIO_CHANNEL_E audio_channel;
}TUYA_XVR_CLOUD_STORAGE_INIT_T;

/**
* @enum     CLOUD_STORAGE_TYPE_E
* @brief    cloud storage order type
*/
typedef enum {
    ClOUD_STORAGE_TYPE_CONTINUE,  /** continus record type */
    ClOUD_STORAGE_TYPE_EVENT,     /** event-based type, only record and store audio/video when there is event happening*/ 
    ClOUD_STORAGE_TYPE_INVALID    /** no cloud storage order exist */
}ClOUD_STORAGE_TYPE_E;

/**
* @struct TUYA_CLOUD_STORAGE_INFO_T
* @brief  cloud storage info
*/
typedef struct {
    INT_T slice_duration;         /** time duration in second for cloud storage slice */
}CLOUD_STORAGE_INFO_T;

/**
* @struct CLOUD_STORAGE_SLICE_FRAME_T
* @brief  cloud storage slice frame
*/
typedef struct {
    MEDIA_FRAME_TYPE_E type;      /** media frame type */
    INT_T size;                   /** data size */
    UINT64_T timestamp;           /** time stamp in ms */
    UINT64_T pts;                 /** time stamp in us */
    CHAR_T *data;                 /** data */
}CLOUD_STORAGE_SLICE_FRAME_T;

/**
* @brief    initialize cloud storage, which will malloc all needed memory after this
* @return   OPERATE_RET
* - OPRT_OK init success
* - Others  init failed
*/
OPERATE_RET tuya_ipc_cloud_storage_init(VOID);

/**
* @brief    uninit cloud storage, free used memory
* @return   VOID
*/
VOID tuya_ipc_cloud_storage_uninit(VOID);

/**
* @brief    get current sotrage type, based on purchase order
* @return   ClOUD_STORAGE_TYPE_E
* - ClOUD_STORAGE_TYPE_CONTINUE   continus record type
* - ClOUD_STORAGE_TYPE_EVENT      event-based type, only record and store audio/video when there is event happening
* - ClOUD_STORAGE_TYPE_INVALID    no cloud storage order exist
*/
ClOUD_STORAGE_TYPE_E tuya_ipc_cloud_storage_get_store_mode(VOID);

/**
* @brief    report an event, cloud storage will start
* @return   OPERATE_RET
* - OPRT_OK cloud storage start success
* - Others  cloud storage start failed
*/
OPERATE_RET tuya_ipc_cloud_storage_event_start(VOID);

/**
* @brief    report the end of event, synchronous api
* @return   OPERATE_RET
* - OPRT_OK cloud storage stop success
* - Others  cloud storage stop failed
*/
OPERATE_RET tuya_ipc_cloud_storage_event_stop(VOID);

/**
* @brief    report the end of event, asynchronous api
* @return   OPERATE_RET
* - OPRT_OK cloud storage stop success
* - Others  cloud storage stop failed
*/
OPERATE_RET tuya_ipc_cloud_storage_event_stop_async(VOID);

/**
* @brief          get specified type event status
* @param[in]      type    cloud storage event type
* @return         EVENT_STATUS_E
* - EVENT_NONE    event is not onging
* - EVENT_READY   event is ready
* - EVENT_ONGING  event is onging
* - EVENT_INVALID event status is invalid
*/
EVENT_STATUS_E tuya_ipc_cloud_storage_get_event_status(ClOUD_STORAGE_EVENT_TYPE_E type);

/**
* @brief          set event duration
* @param[in]      max_duration   event max duration
* - OPRT_OK       set duration success
* - Others        set duration failed
*
*/
OPERATE_RET tuya_ipc_cloud_storage_event_set_duration(UINT_T max_duration);

/**
* @brief    get cloud storage status
* @return   BOOL_T
* - TRUE    cloud storage is uploading
* - FALSE   cloud storage is not ongoing
*/
BOOL_T tuya_ipc_cloud_storage_get_status(VOID);

/**
* @brief    pause cloud storage, used in privacy mode e.g.
* @return   VOID
*/
VOID tuya_ipc_cloud_storage_pause(VOID);

/**
* @brief    resume cloud storage which is paused by pause API
* @return   VOID
*/
VOID tuya_ipc_cloud_storage_resume(VOID);

/**
* @brief        set audio open/close
* @param[in]    is_audio_open \n
* @ref          TRUE open audio for cloud storage \n
* @ref          FALSE close audio for cloud storage
* @return       VOID
*/
VOID tuya_ipc_cloud_storage_set_audio_stat(IN CONST BOOL_T is_audio_open);

/**
* @brief        set cloud storage pre record time duration
* @param[in]    pre_record_time
* @result       OPERATE_RET
* - OPRT_OK     set pre record time success
* - Others      set pre record time failed
*/
OPERATE_RET tuya_ipc_cloud_storage_set_pre_record_time(INT_T pre_record_time);

/**
* @brief         initialize async cloud storage, upload media data to cloud by api tuya_dev_cloud_storage_slice_send_frame
* @return        OPERATE_RET
* - OPRT_OK      init success
* - Others       init failed
*/
OPERATE_RET tuya_ipc_async_cloud_storage_init(VOID);

/**
* @brief        get cloud storage info
* @param[in]    info cloud storage info
* @result       OPERATE_RET
* - OPRT_OK     success
* - Others      failed
*/
OPERATE_RET tuya_dev_cloud_storage_get_info(CLOUD_STORAGE_INFO_T *info);

/**
* @brief        cloud storage event start notify
* @param[in]    dev_id dev_id (NULL for ipc)
* @param[in]    start_time event start time in second
* @result       OPERATE_RET
* - OPRT_OK     success
* - Others      failed
*/
OPERATE_RET tuya_dev_cloud_storage_start_notify(CHAR_T *dev_id, UINT_T start_time);

/**
* @brief        cloud storage start slice uploading
* @param[in]    dev_id dev_id (NULL for ipc)
* @param[in]    size data size in one slice
* @result       OPERATE_RET
* - OPRT_OK     success
* - Others      failed
*/
OPERATE_RET tuya_dev_cloud_storage_start_slice(CHAR_T *dev_id, INT_T size);

/**
* @brief        cloud storage send frame in one slice
* @param[in]    dev_id dev_id (NULL for ipc)
* @param[in]    frame frame data
* @result       OPERATE_RET
* - OPRT_OK     success
* - Others      failed
*/
OPERATE_RET tuya_dev_cloud_storage_slice_send_frame(CHAR_T *dev_id, CLOUD_STORAGE_SLICE_FRAME_T *frame);

/**
* @brief        cloud storage stop slice uploading
* @param[in]    dev_id dev_id (NULL for ipc)
* @result       OPERATE_RET
* - OPRT_OK     success
* - Others      failed
*/
OPERATE_RET tuya_dev_cloud_storage_stop_slice(CHAR_T *dev_id);

/**
* @brief        cloud storage event stop notify
* @param[in]    dev_id dev_id (NULL for ipc)
* @result       OPERATE_RET
* - OPRT_OK     success
* - Others      failed
*/
OPERATE_RET tuya_dev_cloud_storage_stop_notify(CHAR_T *dev_id);

#ifdef __cplusplus
}
#endif

#endif
