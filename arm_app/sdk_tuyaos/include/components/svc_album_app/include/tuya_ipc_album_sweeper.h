/**
 * @file tuya_ipc_album_sweeper.h
 * @brief This is sweeper album deal
 * @version 1.0
 * @date 2024-02-07
 *
 * @copyright Copyright 2021-2031 Tuya Inc. All Rights Reserved.
 *
 */

#ifndef __TUYA_IPC_ALBUM_SWEEPER_H__
#define __TUYA_IPC_ALBUM_SWEEPER_H__

#include <stdio.h>
#include "tuya_cloud_types.h"
#include "tuya_album_sweeper_api.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \fn tuya_sweeper_stream_register_event_cb
 * \brief send file to app by p2p
 * \param[in] event_cb: p2p event cb
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_sweeper_stream_register_event_cb(SWEEPER_FILE_STREAM_EVENT_CB event_cb);

/**
 * \fn tuya_sweeper_av_stream_register_event_cb (video+audio)
 * \brief send file to app by p2p
 * \param[in] event_cb: p2p event cb
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_sweeper_av_stream_register_event_cb(SWEEPER_AV_STREAM_EVENT_CB event_cb);

/**
 * \fn tuya_sweeper_stream_event_call
 * \brief send file to app by p2p
 * \param[in] channel: client handle
 * \param[in] event_cb: p2p event cb
 * \param[in] args: p2p cb data
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_sweeper_stream_event_call(INT_T channel, SWEEPER_TRANSFER_EVENT_E event, PVOID_T args);

/**
 * \fn tuya_sweeper_av_stream_event_call
 * \brief send file to app by p2p
 * \param[in] channel: client handle
 * \param[in] event_cb: p2p event cb
 * \param[in] args: p2p cb data
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_sweeper_av_stream_event_call(INT_T channel, MEDIA_STREAM_EVENT_E event, IN PVOID_T args);

/**
 * \fn tuya_album_sweeper_set_customize_name
 * \brief send file to app by p2p
 * \param[in] name_info: file name
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_album_sweeper_set_customize_name(IN TUYA_SWEEPER_CUSTOMIZE_FILEINFO* name_info);
/**
 * @brief  p2p事件回调函数
 * @param  [TRANSFER_EVENT_E] 接收到事件event
 * @param  [PVOID_T] PVOID_T args
 * @return [*]
 */
INT_T ty_user_protocol_p2p_event_cb(IN CONST INT_T device, IN CONST INT_T channel, IN CONST SWEEPER_TRANSFER_EVENT_E event, IN PVOID_T args);

#ifdef __cplusplus
}
#endif

#endif
