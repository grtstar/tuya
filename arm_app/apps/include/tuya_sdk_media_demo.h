/*********************************************************************************
  *Copyright(C),2015-2020, TUYA www.tuya.comm
  *FileName:    tuya_sdk_common
**********************************************************************************/
#ifndef INCLUDE_TUYA_SDK_MEDIA_DEMO_H_
#define INCLUDE_TUYA_SDK_MEDIA_DEMO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "tuya_ipc_media_stream_event.h"
#include "tuya_ipc_media_adapter.h"

/**
 * @brief  音视频 event 回调具体处理
 * @param  [IN CONST channel] 连接哪里客户端
 * @param  [IN CONST MEDIA_STREAM_EVENT_E] event 事件
 * @param  [IN CONST args] 回调数据
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
INT_T tuya_sweeper_av_event_cb(IN CONST INT_T channel, IN CONST MEDIA_STREAM_EVENT_E event, IN PVOID_T args);

/** @brief 接收音频参数回调(语音对讲)
 * @param[in]  device    device number
 * @param[in]  channel   channel number
 * @param[in]  p_audio_frame  audio frame info
 * @return[*]
 */
VOID tuya_sweeper_app_rev_audio_cb(IN INT_T device, IN INT_T channel, IN CONST MEDIA_AUDIO_FRAME_T *p_audio_frame);

/** @brief 接收视频参数回调(双向可视)
 * @param[in]  device  device number
 * @param[in]  channel channel number
 * @param[in]  p_video_frame  video frame info
 * @return[*]
 */
VOID tuya_sweeper_app_rev_video_cb(IN INT_T device, IN INT_T channel, IN CONST MEDIA_VIDEO_FRAME_T *p_video_frame);

/** @brief APP下载文件回调(云相框功能)
 * @param[in]  device  device number
 * @param[in]  channel channel number
 * @param[in]  p_file_data  file data info
 * @return[*]
 */
VOID tuya_sweeper_app_rev_file_cb(IN INT_T device, IN INT_T channel, IN CONST MEDIA_FILE_DATA_T *p_file_data);

/** @brief APP获取一帧数据回调
 * @param[in] device  device number
 * @param[in] channel  channel number
 * @param[in] snap_addr  snap buffer
 * @param[in] snap_size  snap length
 * @return[*]
 */
VOID tuya_sweeper_app_get_snapshot_cb(IN INT_T device, IN INT_T channel, OUT CHAR_T *snap_addr, OUT INT_T *snap_size);

/** @brief 获取一帧数据
 * @param[out]  snap_addr    data addr
 * @param[out]  snap_size    data len
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
int sweeper_app_get_snapshot(char *snap_addr, int *snap_size);

#ifdef __cplusplus
}
#endif
#endif  /*INCLUDE_TUYA_SDK_MEDIA_DEMO_H_*/