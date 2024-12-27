/**
 * @file tuya_ipc_video_msg.h
 * @brief This is tuya ipc video message file
 * @version 1.0
 * @date 2021-11-17
 *
 * @copyright Copyright 2021-2031 Tuya Inc. All Rights Reserved.
 *
 */

#ifndef __TUYA_IPC_VIDEO_MSG_H__
#define __TUYA_IPC_VIDEO_MSG_H__

#include "tuya_cloud_types.h"
#include "tuya_error_code.h"
#include "tuya_ipc_media.h"


#ifdef __cplusplus
extern "C" {
#endif


#define MAX_MESSAGE_TIME 10
#define MAX_AUDIO_MSG_TIME 15


typedef enum {
    MSG_BOTH = 0,
    MSG_VIDEO_ONLY,
    MSG_AUDIO_ONLY,
    MSG_INVALID
} MESSAGE_E;


/**
 * @brief initialize video msg leaving, which will malloc all needed memory after this
 * 
 * @param[in] type: msg type(audio or video)
 * @param[in] msg_duration: duration of msg(the video longgest duration is 10 seconds,audio:15 seconds)
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_video_msg_init(IN MESSAGE_E type, IN INT_T msg_duration);

/**
 * @brief uninit video msg leaving, free used memory
 * 
 * @param VOID
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_video_msg_uninit(VOID);

/**
 * @brief leave a video msg from now.
 * 
 * @param[in] extra_data: extra data
 * @param[in] data_len: extra data length
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_leave_video_msg(IN CHAR_T *extra_data, IN INT_T data_len);

/**
 * @brief push a md alarm with video
 * 
 * @param[in] extra_data: extra data
 * @param[in] data_len: extra data length
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_md_video_msg(IN CHAR_T *extra_data, IN INT_T data_len);

/**
 * @brief get video msg status
 * 
 * @param VOID
 * 
 * @return TRUE/FALSE 
 */
BOOL_T tuya_ipc_video_msg_status_is_busy(VOID);


#ifdef __cplusplus
}
#endif

#endif