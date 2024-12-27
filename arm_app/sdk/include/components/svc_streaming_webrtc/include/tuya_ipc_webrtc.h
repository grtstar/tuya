/**
* @tuya_ipc_webrtc.h
* @brief tuya_ipc_webrtc used
* @version 0.1
* @date 2021-11-17
*
* @copyright Copyright (c) tuya.inc 2011
* 
*/

#ifndef __TUYA_IPC_WEBRTC_H__
#define __TUYA_IPC_WEBRTC_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
* @enum TUYA_WEBRTC_MODE_E
*
* @brief define  webrtc mode
*/
typedef enum TUYA_WEBRTC_MODE_ {
    TUYA_WEBRTC_ECHOSHOW,
    TUYA_WEBRTC_CHROMECAST,
    TUYA_WEBRTC_WEB,
} TUYA_WEBRTC_MODE_E;

/**
* @brief indicate webrtc stream start
*
* @param[in] context: user data 
* @param[in] mode: stream mode
* @param[in] priv_data: reserve

* @return INT_T 0:success, other: fail
*/
typedef INT_T (*TUYA_WEBRTC_START_CB)(PVOID_T context, TUYA_WEBRTC_MODE_E mode, PVOID_T priv_data);

/**
* @brief indicate webrtc stream stop
*
* @param[in] context: user data 
* @param[in] mode: stream mode
* @param[in] priv_data: reserve

* @return INT_T 0:success, other: fail
*/
typedef INT_T (*TUYA_WEBRTC_STOP_CB)(PVOID_T context, TUYA_WEBRTC_MODE_E mode, PVOID_T priv_data);

/**
* @struct TUYA_WEBRTC_CALLBACK
*
* @brief define  webrtc callback
*/

typedef struct {
    PVOID_T                     pcontext;   /* context, user data */
    TUYA_WEBRTC_START_CB           start;      /* indicate webrtc stream start */
    TUYA_WEBRTC_STOP_CB            stop;       /* indicate webrtc stream stop */
} TUYA_WEBRTC_CALLBACK;

/**
* @struct TUYA_WEBRTC_PARAM_T
*
* @brief define  webrtc params
*/
typedef struct {
    TUYA_WEBRTC_CALLBACK  cbk;
} TUYA_WEBRTC_PARAM_T;

/**
* @brief initialize tuya webrtc sdk
*
* @param[in] VOID
*
* @@return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tuya_ipc_webrtc_init(VOID);

/**
* @brief uninitialize tuya webrtc sdk
*
* @param[in] VOID
*
* @@return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tuya_ipc_webrtc_deinit(VOID);

/**
* @brief stop current webrtc services
*
* @param[in] VOID
*
* @@return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tuya_ipc_webrtc_stop(VOID);

/**
* @brief cur alive web cnt
*
* @param[out] pCnt:cur alive web cnt
*
* @@return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/

OPERATE_RET tuya_ipc_webrtc_alive_cnt(INT_T * pCnt);

/**
* @brief stop current webrtc services
*
* @param[in] pause_flag:0 resume 1:stop
*
* @@return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tuya_ipc_webRt_pause_set(CHAR_T pause_flag);

/**
* @brief tuya_ipc_webrtc_playback_init
*
* @param[in] start_id: min id
*
* @@return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
int tuya_ipc_webrtc_playback_init(int start_id);

/**
* @brief session is webrtc ?
*
* @param[in] id: session id
*
* @@return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
BOOL_T tuya_ipc_webrtc_is_webrtc_session(int id);

/**
* @brief webrtc playback end api
*
* @param[in] client: session id start_timestamp :start pts end_timestamp:end pts
*
* @@return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tuya_ipc_webrtc_playback_send_fragment_end(IN CONST UINT_T client, IN UINT_T start_timestamp, IN UINT_T end_timestamp);

/**
* @brief webrtc playback finish api
*
* @param[in] client: session id
*
* @@return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tuya_ipc_webrtc_playback_send_finish(IN CONST UINT_T client);

/**
* @brief webrtc playback send video api
*
* @param[in] client: session id p_video_frame:video data
*
* @@return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
int tuya_ipc_webrtc_playback_send_video_frame(IN CONST UINT_T client, IN CONST void *p_video_frame);

/**
* @brief webrtc playback send audio api
*
* @param[in] client: session id p_video_frame:audio data
*
* @@return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
int tuya_ipc_webrtc_playback_send_audio_frame(IN CONST UINT_T client, IN CONST void *p_audio_frame);

#ifdef __cplusplus
}
#endif

#endif
