/*********************************************************************************
  *Copyright(C),2015-2020, TUYA www.tuya.comm
  *FileName:    tuya_sdk_media_demo
  *brief：1.This article introduces the implementation of real-time video preview,
            audio playback, and intercom functionality.
          2.Developers can determine specific events and implementation logic in
            the interaction process with the client based on audio and video event
            callbacks. Please refer to the implementation of the following demo.
**********************************************************************************/
#include "utilities/uni_log.h"
#include <libgen.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>
#include <unistd.h>

#include "tal_video_enc.h"
#include "tuya_album_sweeper_api.h"
#include "tuya_iot_config.h"
#include "tuya_ipc_media.h"
#include "tuya_ipc_media_stream.h"
#include "tuya_ipc_media_stream_event.h"
#include "tuya_ipc_p2p.h"
#include "tuya_ipc_stream_storage.h"
#include "tuya_ring_buffer.h"
#include "tuya_sdk_common.h"
#include "tuya_g711_utils.h"
#include "rkcamera/rkcamera.h"
#include "rksound/rksound.h"

typedef struct
{
    BOOL_T enabled; //预留
    TRANSFER_VIDEO_CLARITY_TYPE_E live_clarity; //视频清晰度（高清&标清）
} TUYA_APP_P2P_MGR;

STATIC TUYA_APP_P2P_MGR s_p2p_mgr = { 0 }; //音视频流资源全局变量

/**
 * @brief  视频原始帧进行编码
 * @param  [IN CONST MEDIA_FRAME_T] 流入的视频原数据
 * @param  [INOUT CONST MEDIA_VIDEO_FRAME_T] 输出编码后的数据
 * @return [*]
 */
STATIC VOID tuya_app_media_frame_TO_trans_video(IN CONST MEDIA_FRAME_T* p_in, INOUT MEDIA_VIDEO_FRAME_T* p_out)
{
    UINT_T codec_type = 0;
    codec_type = (p_in->type & 0xff00) >> 8;
    p_out->video_codec = (codec_type == 0 ? TUYA_CODEC_VIDEO_H264 : TUYA_CODEC_VIDEO_H265); //选择编码方式
    p_out->video_frame_type = (p_in->type && 0xff) == E_VIDEO_PB_FRAME ? TUYA_VIDEO_FRAME_PBFRAME : TUYA_VIDEO_FRAME_IFRAME; //选择帧类型
    p_out->p_video_buf = p_in->p_buf; //原始数据
    p_out->buf_len = p_in->size; //原始数据长度
    p_out->pts = p_in->pts; //呈现时间戳，确保视频同步和正确的播放顺序。
    p_out->timestamp = p_in->timestamp; //时间戳 ms

    return;
}

/**
 * @brief  音频原始帧进行编码
 * @param  [IN CONST MEDIA_FRAME_T] 流入的音频原数据
 * @param  [INOUT CONST MEDIA_VIDEO_FRAME_T] 输出编码后的数据
 * @return [*]
 */
STATIC VOID tuya_app_media_frame_TO_trans_audio(IN CONST MEDIA_FRAME_T* p_in, INOUT MEDIA_AUDIO_FRAME_T* p_out)
{
    DEVICE_MEDIA_INFO_T media_info = { 0 };
    tuya_ipc_media_adapter_get_media_info(0, 0, &media_info); //获取音频编码参数
    p_out->audio_codec = media_info.av_encode_info.audio_codec[E_IPC_STREAM_AUDIO_MAIN]; //编码方式
    p_out->audio_sample = media_info.av_encode_info.audio_sample[E_IPC_STREAM_AUDIO_MAIN]; //采集频率
    p_out->audio_databits = media_info.av_encode_info.audio_databits[E_IPC_STREAM_AUDIO_MAIN]; //音频带宽
    p_out->audio_channel = media_info.av_encode_info.audio_channel[E_IPC_STREAM_AUDIO_MAIN]; //音频频道
    p_out->p_audio_buf = p_in->p_buf; //原始数据
    p_out->buf_len = p_in->size; //原始数据长度
    p_out->pts = p_in->pts; //呈现时间戳，确保音频同步和正确的播放顺序。
    p_out->timestamp = p_in->timestamp; //时间戳 ms

    return;
}

/**
 * @brief  音频播放接口
 * @param  [IN CONST BOOL_T] 是否开启音频对讲
 * @return [*]
 */
STATIC INT_T tuya_app_Enable_Speaker_CB(IN BOOL_T enable)
{
    if (enable == TRUE) {
        /*按需要来实现，比如播放文件先暂停，让语音对讲优先*/
        /*开发者在这里会收到 APP 下发开始语音对讲请求*/

    } else {
        /*开发者在这里会收到 APP 下发结束语音对讲请求*/
    }
    return 0;
}

/**
 * @brief  音视频回放事件回调
 * @param  [IN UINT_T] pb_idx
 * @param  [IN SS_PB_EVENT_E] 事件
 * @param  [IN PVOID_T] 参数
 * @return [*]
 */
STATIC VOID tuya_app_ss_pb_event_cb(IN UINT_T pb_idx, IN SS_PB_EVENT_E pb_event, IN PVOID_T args)
{
    PR_DEBUG("ss pb rev event: %u %d", pb_idx, pb_event);
    if (pb_event == SS_PB_FINISH) { //事件结束，不再有数据传输
        tuya_ipc_media_playback_send_finish(pb_idx); //回放完成
    }
    return;
}

/**
 * @brief  视频回放传输回调
 * @param  [IN UINT_T] pb_idx
 * @param  [IN CONST MEDIA_FRAME_T] 原始数据帧
 * @return [*]
 */
STATIC VOID tuya_app_ss_pb_get_video_cb(IN UINT_T pb_idx, IN CONST MEDIA_FRAME_T* p_frame)
{
    MEDIA_VIDEO_FRAME_T video_frame = { 0 };
    tuya_app_media_frame_TO_trans_video(p_frame, &video_frame); //视频数据转化
    tuya_ipc_media_playback_send_video_frame(pb_idx, &video_frame); //发送视频数据

    return;
}

/**
 * @brief  音频回放传输回调
 * @param  [IN UINT_T] pb_idx
 * @param  [IN CONST MEDIA_FRAME_T] 原始数据帧
 * @return [*]
 */
STATIC VOID tuya_app_ss_pb_get_audio_cb(IN UINT_T pb_idx, IN CONST MEDIA_FRAME_T* p_frame)
{
    MEDIA_AUDIO_FRAME_T audio_frame = { 0 };
    tuya_app_media_frame_TO_trans_audio(p_frame, &audio_frame); //音频数据转化
    tuya_ipc_media_playback_send_audio_frame(pb_idx, &audio_frame); //发送音频数据

    return;
}

extern void * rkcamera;

/**
 * @brief  音视频 event 回调具体处理
 * @param  [IN CONST channel] 连接哪里客户端
 * @param  [IN CONST MEDIA_STREAM_EVENT_E] event 事件
 * @param  [IN CONST args] 回调数据
 * @return [*]
 */
INT_T tuya_sweeper_av_event_cb(IN CONST INT_T channel, IN CONST MEDIA_STREAM_EVENT_E event, IN PVOID_T args)
{
    int ret = 0;
    PR_DEBUG("p2p rev av event cb=[%d] ", event);
    if (event != MEDIA_STREAM_SPEAKER_START && event != MEDIA_STREAM_SPEAKER_STOP && args == NULL) {
        PR_DEBUG("p2p rev event args null ");
        return -1;
    }
    switch (event) {
    case MEDIA_STREAM_LIVE_VIDEO_START: {
        C2C_TRANS_CTRL_VIDEO_START* parm = (C2C_TRANS_CTRL_VIDEO_START*)args;
        PR_DEBUG("chn[%u] video start", parm->channel);
        /*这里是 APP 通知设备开始播放视频信息，用户可以在此次做必要的业务逻辑*/
        printf("=============OPEN %d CAMERA==============\n", parm->channel);
        rkcamera = rkcamera_open();
        rksound_record_open();
        break;
    }
    case MEDIA_STREAM_LIVE_VIDEO_STOP: {
        C2C_TRANS_CTRL_VIDEO_STOP* parm = (C2C_TRANS_CTRL_VIDEO_STOP*)args;
        PR_DEBUG("chn[%u] video stop", parm->channel);
        /*这里是 APP 通知设备停止播放视频信息，用户可以在此次做必要的业务逻辑*/
        rkcamera_close(rkcamera);
        rksound_record_close();
        printf("=============CLOSE %d CAMERA==============\n", parm->channel);
        break;
    }
    case MEDIA_STREAM_LIVE_AUDIO_START: {
        C2C_TRANS_CTRL_AUDIO_START* parm = (C2C_TRANS_CTRL_AUDIO_START*)args;
        PR_DEBUG("chn[%u] audio start", parm->channel);
        /*这里是 APP 通知设备开始播放语音信息，用户可以在此次做必要的业务逻辑*/
        printf("=============OPEN %d AUDIO==============\n", parm->channel);

        rksound_play_open();
        break;
    }
    case MEDIA_STREAM_LIVE_AUDIO_STOP: {
        C2C_TRANS_CTRL_AUDIO_STOP* parm = (C2C_TRANS_CTRL_AUDIO_STOP*)args;
        PR_DEBUG("chn[%u] audio stop", parm->channel);
        /*这里是 APP 通知设备停止播放语音信息，用户可以在此次做必要的业务逻辑*/
        printf("=============CLOSE %d AUDIO==============\n", parm->channel);

        rksound_play_close();
        break;
    }
    case MEDIA_STREAM_SPEAKER_START: {
        PR_DEBUG("enbale audio speaker");
        tuya_app_Enable_Speaker_CB(TRUE);
        /*这里是 APP 通知设备开始语音对讲*/
        break;
    }
    case MEDIA_STREAM_SPEAKER_STOP: {
        PR_DEBUG("disable audio speaker");
        tuya_app_Enable_Speaker_CB(FALSE);
        /*这里是 APP 通知设备停止语音对讲*/
        break;
    }
    case MEDIA_STREAM_ABILITY_QUERY: { //查询音频视频流的能力
        C2C_TRANS_QUERY_FIXED_ABI_REQ* pAbiReq;
        pAbiReq = (C2C_TRANS_QUERY_FIXED_ABI_REQ*)args;
        pAbiReq->ability_mask = TY_CMD_QUERY_IPC_FIXED_ABILITY_TYPE_VIDEO | TY_CMD_QUERY_IPC_FIXED_ABILITY_TYPE_SPEAKER | TY_CMD_QUERY_IPC_FIXED_ABILITY_TYPE_MIC;
        break;
    }
    /**以下流程如果使用非涂鸦存储的格式回放，是可以实现的，但可能需要大量的调试****/
    case MEDIA_STREAM_PLAYBACK_QUERY_MONTH_SIMPLIFY: { //按月查询本地视频信息
        C2C_TRANS_QUERY_PB_MONTH_REQ* p = (C2C_TRANS_QUERY_PB_MONTH_REQ*)args;
        PR_DEBUG("pb query by month: %d-%d", p->year, p->month);

        OPERATE_RET ret = tuya_ipc_pb_query_by_month(p->ipcChan, p->year, p->month, &(p->day)); //根据日期查询对应的视频是否存在
        if (OPRT_OK != ret) {
            PR_ERR("pb query by month: %d-%d ret:%d", p->year, p->month, ret);
        }

        break;
    }
    case MEDIA_STREAM_PLAYBACK_QUERY_DAY_TS: { //按日查询本地视频信息
        C2C_TRANS_QUERY_PB_DAY_RESP* pquery = (C2C_TRANS_QUERY_PB_DAY_RESP*)args;
        PR_DEBUG("pb_ts query by day: idx[%d]%d-%d-%d", pquery->channel, pquery->year, pquery->month, pquery->day);
        SS_QUERY_DAY_TS_ARR_T* p_day_ts = NULL;
        OPERATE_RET ret = tuya_ipc_pb_query_by_day(pquery->channel, pquery->ipcChan, pquery->year, pquery->month, pquery->day, &p_day_ts); //根据时间查询对应的视频是否存在
        if (OPRT_OK != ret) {
            PR_ERR("pb_ts query by day: %d-%d-%d Fail", pquery->channel, pquery->year, pquery->month, pquery->day);
            break;
        }
        if (p_day_ts) {
            PR_DEBUG("%s %d count = %d\n", __FUNCTION__, __LINE__, p_day_ts->file_count);
            PLAY_BACK_ALARM_INFO_ARR* pResult = (PLAY_BACK_ALARM_INFO_ARR*)malloc(sizeof(PLAY_BACK_ALARM_INFO_ARR) + p_day_ts->file_count * sizeof(PLAY_BACK_ALARM_FRAGMENT));
            if (NULL == pResult) {
                PR_ERR("%s %d malloc failed \n", __FUNCTION__, __LINE__);
                free(p_day_ts);
                pquery->alarm_arr = NULL;
                return 0;
            }

            INT_T i;
            pResult->file_count = p_day_ts->file_count;
            for (i = 0; i < p_day_ts->file_count; i++) {
                pResult->file_arr[i].type = p_day_ts->file_arr[i].type;
                pResult->file_arr[i].time_sect.start_timestamp = p_day_ts->file_arr[i].start_timestamp;
                pResult->file_arr[i].time_sect.end_timestamp = p_day_ts->file_arr[i].end_timestamp;
            }
            pquery->alarm_arr = pResult;
            free(p_day_ts);

        } else {
            pquery->alarm_arr = NULL;
        }
        break;
    }
    /*开始执行回放，即从指定时间找到音视频帧数据，依次把之后的音视频数据按照要求的格式填入结构体并调用接口发送，
    参考 tuya_app_ss_pb_get_video_cb, tuya_app_ss_pb_get_audio_cb 的实现，即获取到的音视频帧数据通过以上两个接口发送。****/
    case MEDIA_STREAM_PLAYBACK_START_TS: { //开始回放视频
        /* 客户端在播放时会带上开始时间。这里只进行日志打印。 */
        C2C_TRANS_CTRL_PB_START* pParam = (C2C_TRANS_CTRL_PB_START*)args;
        PR_DEBUG("PB StartTS idx:%d %u [%u %u]", pParam->channel, pParam->playTime, pParam->time_sect.start_timestamp, pParam->time_sect.end_timestamp);

        SS_FILE_TIME_TS_T pb_file_info;
        int ret;
        memset(&pb_file_info, 0x00, sizeof(SS_FILE_TIME_TS_T));
        // memcpy(&pb_file_info, &pParam->time_sect, sizeof(SS_FILE_TIME_TS_T));
        pb_file_info.start_timestamp = pParam->time_sect.start_timestamp;
        pb_file_info.end_timestamp = pParam->time_sect.end_timestamp;
        ret = tuya_ipc_ss_pb_start(pParam->channel, tuya_app_ss_pb_event_cb, tuya_app_ss_pb_get_video_cb, tuya_app_ss_pb_get_audio_cb); //开始时间戳回放
        if (0 != ret) {
            PR_ERR("%s %d pb_start failed\n", __FUNCTION__, __LINE__);
            tuya_ipc_media_playback_send_finish(pParam->channel); //回放结束
        } else {

            if (0 != tuya_ipc_ss_pb_seek(pParam->channel, &pb_file_info, pParam->playTime)) { //对应的时间戳查找文件
                PR_ERR("%s %d pb_seek failed\n", __FUNCTION__, __LINE__);
                tuya_ipc_media_playback_send_finish(pParam->channel); //回放结束
            }
        }

        break;
    }
    case MEDIA_STREAM_PLAYBACK_PAUSE: { //暂停回放视频
        C2C_TRANS_CTRL_PB_PAUSE* pParam = (C2C_TRANS_CTRL_PB_PAUSE*)args;
        PR_DEBUG("PB Pause idx:%d", pParam->channel);

        tuya_ipc_ss_pb_set_status(pParam->channel, SS_PB_PAUSE);
        break;
    }
    case MEDIA_STREAM_PLAYBACK_RESUME: { //继续回放视频
        C2C_TRANS_CTRL_PB_RESUME* pParam = (C2C_TRANS_CTRL_PB_RESUME*)args;
        PR_DEBUG("PB Resume idx:%d", pParam->channel);

        tuya_ipc_ss_pb_set_status(pParam->channel, SS_PB_RESUME);
        break;
    }
    case MEDIA_STREAM_PLAYBACK_MUTE: { //静音
        C2C_TRANS_CTRL_PB_MUTE* pParam = (C2C_TRANS_CTRL_PB_MUTE*)args;
        PR_DEBUG("PB idx:%d mute", pParam->channel);

        tuya_ipc_ss_pb_set_status(pParam->channel, SS_PB_MUTE);
        break;
    }
    case MEDIA_STREAM_PLAYBACK_UNMUTE: { //取消静音
        C2C_TRANS_CTRL_PB_UNMUTE* pParam = (C2C_TRANS_CTRL_PB_UNMUTE*)args;
        PR_DEBUG("PB idx:%d unmute", pParam->channel);

        tuya_ipc_ss_pb_set_status(pParam->channel, SS_PB_UN_MUTE);
        break;
    }
    case MEDIA_STREAM_PLAYBACK_SET_SPEED: { //设置回放倍速
        C2C_TRANS_CTRL_PB_SET_SPEED* pParam = (C2C_TRANS_CTRL_PB_SET_SPEED*)args;
        PR_DEBUG("chn[%u] video set speed[%u]\n", pParam->channel, pParam->speed);
        int ret = tuya_ipc_ss_pb_set_speed(pParam->channel, pParam->speed);
        if (0 != ret) {
            PR_ERR("%s %d pb set speed failed\n", __FUNCTION__, __LINE__);
            tuya_ipc_media_playback_send_finish(pParam->channel);
        }
        break;
    }
    case MEDIA_STREAM_PLAYBACK_STOP: { //停止回放视频
        C2C_TRANS_CTRL_PB_STOP* pParam = (C2C_TRANS_CTRL_PB_STOP*)args;
        PR_DEBUG("PB Stop idx:%d", pParam->channel);

        tuya_ipc_ss_pb_stop(pParam->channel);
        break;
    }
    /**以上流程如果使用非涂鸦存储的格式回放，是可以实现的，但可能需要大量的调试****/
    case MEDIA_STREAM_LIVE_VIDEO_CLARITY_SET: { //设置视频清晰度
        C2C_TRANS_LIVE_CLARITY_PARAM_S* pParam = (C2C_TRANS_LIVE_CLARITY_PARAM_S*)args;
        PR_DEBUG("set clarity:%d", pParam->clarity);
        if ((pParam->clarity == TY_VIDEO_CLARITY_STANDARD) || (pParam->clarity == TY_VIDEO_CLARITY_HIGH)) {
            PR_DEBUG("set clarity:%d OK", pParam->clarity);
            s_p2p_mgr.live_clarity = pParam->clarity;
        }
        break;
    }
    case MEDIA_STREAM_LIVE_VIDEO_CLARITY_QUERY: { //查下视频清晰度
        C2C_TRANS_LIVE_CLARITY_PARAM_S* pParam = (C2C_TRANS_LIVE_CLARITY_PARAM_S*)args;
        pParam->clarity = s_p2p_mgr.live_clarity;
        PR_DEBUG("query larity:%d", pParam->clarity);
        break;
    }
    default:
        break;
    }
    return ret;
}

/** @brief 获取一帧数据
 * @param[out]  snap_addr    data addr
 * @param[out]  snap_size    data len
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
int sweeper_app_get_snapshot(char* snap_addr, int* snap_size)
{
    TAL_VENC_FRAME_T frame = { 0 };
    if (snap_addr == NULL || snap_size == NULL || *snap_size <= 0) {
        PR_ERR("parm is wrong\n");
        return OPRT_COM_ERROR;
    }

    frame.pbuf = snap_addr;
    frame.buf_size = *snap_size;
    OPERATE_RET ret = tal_venc_get_frame(0, 4, &frame); //获取底层原始数据
    if (ret != OPRT_OK) {
        PR_ERR("get pic failed\n", ret);
        return OPRT_COM_ERROR;
    }
    *snap_size = frame.used_size;
    PR_DEBUG("get pic suc, size:%d", frame.used_size);

    return ret;
}
/** @brief 接收音频参数回调（语音对讲）
 * @param[in]  device    device number
 * @param[in]  channel   channel number
 * @param[in]  p_audio_frame  audio frame info
 * @return[*]
 */
VOID tuya_sweeper_app_rev_audio_cb(IN INT_T device, IN INT_T channel, IN CONST MEDIA_AUDIO_FRAME_T* p_audio_frame)
{
    MEDIA_FRAME_T audio_frame = { 0 };
    audio_frame.p_buf = p_audio_frame->p_audio_buf;
    audio_frame.size = p_audio_frame->buf_len;

    PR_DEBUG("Rev Audio. size:[%u] audio_codec:[%d] audio_sample:[%d] audio_databits:[%d] audio_channel:[%d]\n", audio_frame.size,
        p_audio_frame->audio_codec, p_audio_frame->audio_sample, p_audio_frame->audio_databits, p_audio_frame->audio_channel);
    /*这里是收到 APP 的语音信息，在设备端播放出来*/
    if(TUYA_CODEC_AUDIO_G711U == p_audio_frame->audio_codec) {
        uint8_t *p_buf = (uint8_t *)malloc(audio_frame.size * 3);
        if(p_buf)
        {
            uint32_t out_len = audio_frame.size * 3;
            tuya_g711_decode(TUYA_G711_MU_LAW, audio_frame.p_buf, audio_frame.size / 2, p_buf, &out_len);
            rksound_play_pcm(p_buf, out_len);
            free(p_buf);
        }
    }
    if(TUYA_CODEC_AUDIO_PCM == p_audio_frame->audio_codec) {
        rksound_play_pcm(audio_frame.p_buf, audio_frame.size);
    }
    return;
}
/** @brief 接收视频参数回调 (双向可视)
 * @param[in]  device  device number
 * @param[in]  channel channel number
 * @param[in]  p_video_frame  video frame info
 * @return[*]
 */
VOID tuya_sweeper_app_rev_video_cb(IN INT_T device, IN INT_T channel, IN CONST MEDIA_VIDEO_FRAME_T* p_video_frame)
{
    PR_DEBUG("Rev video. size:[%u] video_codec:[%d] video_frame_type:[%d]\n", p_video_frame->buf_len, p_video_frame->video_codec, p_video_frame->video_frame_type);

    return;
}
/** @brief APP 下载文件回调 (云相框功能)
 * @param[in]  device  device number
 * @param[in]  channel channel number
 * @param[in]  p_file_data  file data info
 * @return[*]
 */
VOID tuya_sweeper_app_rev_file_cb(IN INT_T device, IN INT_T channel, IN CONST MEDIA_FILE_DATA_T* p_file_data)
{
    return;
}
/** @brief APP 获取一帧数据回调
 * @param[in] device  device number
 * @param[in] channel  channel number
 * @param[in] snap_addr  snap buffer
 * @param[in] snap_size  snap length
 * @return[*]
 */
VOID tuya_sweeper_app_get_snapshot_cb(IN INT_T device, IN INT_T channel, OUT CHAR_T* snap_addr, OUT INT_T* snap_size)
{
    sweeper_app_get_snapshot(snap_addr, snap_size);
}
