/*********************************************************************************
 *Copyright(C),2015-2020, TUYA www.tuya.comm
 *FileName:    tuya_sdk_av_demo
 *brief：1.This article is based on an audio and video testing demo on Ubuntu.
         2.The simulated audio and video data is transmitted from the TKL layer
           interface to the ring buffer, enabling normal display of audio and
           video on the client side.
         3.Developers need to properly adapt the tkl_audio, tkl_video_enc, and
           tkl_video_in files to transmit real audio and video data to the ring
           buffer.
 **********************************************************************************/
#include <libgen.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>
#include <unistd.h>

#include "tuya_iot_config.h"
#include "tuya_ipc_api.h"
#include "tuya_ipc_media.h"
#include "tuya_ipc_media_stream_event.h"
#include "tuya_ipc_p2p.h"
#include "tuya_ipc_stream_storage.h"
#include "tuya_ring_buffer.h"
#include "utilities/uni_log.h"

#include "tal_audio.h"
#include "tal_video_enc.h"
#include "tal_video_in.h"

#define APP_SYS_AV_AUDIO_FRAME_SIZE (640) ////音频一帧最大长度 640，开发者根据自身的硬件来确定 16000*2/25=1280  8000*2/25=640 ak 帧长:512(pcm),256(g711u)
#define APP_SYS_AV_VIDEO_FRAME_SIZE_100K (100 * 1024) //标清子码流最大 100k

RING_BUFFER_USER_HANDLE_T s_ring_buffer_handles[E_IPC_STREAM_MAX] = {
    // ring buff 资源全局变量
    NULL,
};

void read_frame_from_file(char* file_path, UCHAR_T* frame_buff, int bufSize)
{
    FILE* fp = fopen(file_path, "rb");
    if (fp == NULL) {
        PR_ERR("open file failed\n");
        return;
    }
    fread(frame_buff, 1, bufSize, fp);
    fclose(fp);
}

/**
 * @brief  启动主视频流任务
 * @param
 * @return [*]
 */
void* __video_main(void* args)
{
    UCHAR_T* frame_buff = NULL;
    int bufSize = MAX_MEDIA_FRAME_SIZE; //主高清视频一帧最大长度 300K
    TAL_VENC_FRAME_T frame = { 0 };

    frame_buff = (UCHAR_T*)malloc(bufSize);
    if (NULL == frame_buff) {
        PR_ERR("malloc failed type = %d\n", E_IPC_STREAM_VIDEO_MAIN);
        return NULL;
    }
    frame.pbuf = (char*)frame_buff;
    frame.buf_size = bufSize;
    while (1) {
        /*获取到底层的视频流数据，该接口实际映射的是 tkl_venc_get_frame，需要开发者去适配 (可参考开发者文档)，
           demo 中是强制把固定的数据放到 tkl_venc_get_frame 中，去实现播放的闭环。*/
        #if 0   
        if (0 == tal_venc_get_frame(0, 0, &frame)) {

            if (s_ring_buffer_handles[E_IPC_STREAM_VIDEO_MAIN] == NULL) { //主视频流 ring buffer 空闲时，打开一个新的会话进行写入操作
                s_ring_buffer_handles[E_IPC_STREAM_VIDEO_MAIN] = tuya_ipc_ring_buffer_open(0, 0, E_IPC_STREAM_VIDEO_MAIN, E_RBUF_WRITE);
            }

            if (s_ring_buffer_handles[E_IPC_STREAM_VIDEO_MAIN]) { //新的会话打开之后，将原始视频流数据（来自 tkl 层）放到 ring buffer 中
                tuya_ipc_ring_buffer_append_data(s_ring_buffer_handles[E_IPC_STREAM_VIDEO_MAIN], frame_buff,
                    frame.used_size, frame.frametype, frame.pts);

            } else {
                PR_ERR("tuya_ipc_ring_buffer_open failed,channle:%d\n", E_IPC_STREAM_VIDEO_MAIN);
            }
        } else {
            usleep(10 * 1000);
        }
        #else
        usleep(10 * 1000);
        #endif
    }
    if (frame_buff != NULL) {
        free(frame_buff); //异常情况需释放 buff，并关闭会话。
    }
    
    if (s_ring_buffer_handles[E_IPC_STREAM_VIDEO_MAIN] != NULL) {
        tuya_ipc_ring_buffer_close(s_ring_buffer_handles[E_IPC_STREAM_VIDEO_MAIN]); //关闭会话
    }
    return NULL;
}

/**
 * @brief  启动子视频流任务
 * @param
 * @return [*]
 */
void* __video_sub(void* args)
{
    UCHAR_T* frame_buff = NULL;
    int bufSize = APP_SYS_AV_VIDEO_FRAME_SIZE_100K; //子标清视频一帧最大长度 100K
    TAL_VENC_FRAME_T frame = { 0 };

    frame_buff = (UCHAR_T*)malloc(bufSize);
    if (NULL == frame_buff) {
        PR_ERR("malloc failed type = %d\n", E_IPC_STREAM_VIDEO_SUB);
        return NULL;
    }
    frame.pbuf = (char*)frame_buff;
    frame.buf_size = bufSize;
    while (1) {
        /*获取到底层的视频流数据，该接口实际映射的是 tkl_venc_get_frame，需要开发者去适配 (可参考开发者文档)，
            demo 中是强制把固定的数据放到 tkl_venc_get_frame 中，去实现播放的闭环。*/
        if (0 == tal_venc_get_frame(0, 1, &frame)) {

            if (s_ring_buffer_handles[E_IPC_STREAM_VIDEO_SUB] == NULL) { //子视频流 ring buffer 空闲时，打开一个新的会话进行写入操作
                s_ring_buffer_handles[E_IPC_STREAM_VIDEO_SUB] = tuya_ipc_ring_buffer_open(0, 0, E_IPC_STREAM_VIDEO_SUB, E_RBUF_WRITE);
            }

            if (s_ring_buffer_handles[E_IPC_STREAM_VIDEO_SUB]) { //新的会话打开之后，将原始视频流数据放到 ring buffer 中
                tuya_ipc_ring_buffer_append_data(s_ring_buffer_handles[E_IPC_STREAM_VIDEO_SUB], frame_buff,
                    frame.used_size, frame.frametype, frame.pts);

            } else {
                PR_ERR("tuya_ipc_ring_buffer_open failed,channle:%d\n", E_IPC_STREAM_VIDEO_SUB);
            }
        } else {
            usleep(10 * 1000);
        }
    }
    if (frame_buff != NULL) {
        free(frame_buff); //异常情况需释放 buff，并关闭会话。
    }

    if (s_ring_buffer_handles[E_IPC_STREAM_VIDEO_SUB] != NULL) { //关闭会话
        tuya_ipc_ring_buffer_close(s_ring_buffer_handles[E_IPC_STREAM_VIDEO_SUB]);
    }
    return NULL;
}

/**
 * @brief  启动音频流任务
 * @param
 * @return [*]
 */
void* __audio_main(void* args)
{
    UCHAR_T* frame_buff = NULL;
    int bufSize = APP_SYS_AV_AUDIO_FRAME_SIZE; //音频一帧最大长度 640 字节
    TAL_AUDIO_FRAME_INFO_T frame = { 0 };

    frame_buff = (UCHAR_T*)malloc(bufSize);
    if (NULL == frame_buff) {
        PR_ERR("malloc failed type = %d\n", E_IPC_STREAM_AUDIO_MAIN);
        return NULL;
    }
    frame.pbuf = (char*)frame_buff;
    frame.buf_size = bufSize;
    while (1) {
        /*获取到底层的音频流数据，该接口实际映射的是 tkl_ai_get_frame，需要开发者去适配 (可参考开发者文档)，
           demo 中是强制把固定的数据放到 tkl_ai_get_frame 中，去实现播放的闭环。*/
        if (0 == tal_ai_get_frame(0, 0, &frame)) {
            if (s_ring_buffer_handles[E_IPC_STREAM_AUDIO_MAIN] == NULL) { //音频流 ring buffer 空闲时，打开一个新的会话进行写入操作
                s_ring_buffer_handles[E_IPC_STREAM_AUDIO_MAIN] = tuya_ipc_ring_buffer_open(0, 0, E_IPC_STREAM_AUDIO_MAIN, E_RBUF_WRITE);
            }

            if (s_ring_buffer_handles[E_IPC_STREAM_AUDIO_MAIN]) { //新的会话打开之后，将原始音频流数据放到 ring buffer 中
                tuya_ipc_ring_buffer_append_data(s_ring_buffer_handles[E_IPC_STREAM_AUDIO_MAIN], frame_buff, frame.used_size,
                    frame.type, frame.pts);

            } else {
                PR_ERR("tuya_ipc_ring_buffer_open failed,channle:%d\n", E_IPC_STREAM_AUDIO_MAIN);
            }
        } else {
            usleep(10 * 1000);
        }
    }
    if (frame_buff != NULL) {
        free(frame_buff); //异常情况需释放 buff，并关闭会话。
    }
    
    if (s_ring_buffer_handles[E_IPC_STREAM_AUDIO_MAIN] != NULL) { //关闭会话
        tuya_ipc_ring_buffer_close(s_ring_buffer_handles[E_IPC_STREAM_AUDIO_MAIN]);
    }
    return NULL;
}

/**
 * @brief  创建音视频塞流任务
 * @param
 * @return [*]
 */
void tuya_av_start(void)
{
#define TY_DSP_MEDIA_AUDIO_MAX 1 //音频采集 支持 1 路
#define TY_DSP_MEDIA_VI_MAX 1 //视频输入 支持 1 路
#define TY_DSP_MEDIA_VENC_MAX 4 //视频编码，使用 4 路
#if 0
    //开发者可以参考开发者文档，根据自己的硬件资源来初始化以下接口
    PR_DEBUG("create av task!!!\n");
    int ret = 0;
    ret = tal_vi_init(NULL, TY_DSP_MEDIA_VI_MAX); //底层 TKL 硬件视频采集初始化接口
    if (0 != ret) {
        PR_ERR("ty_dsp_init failed,%d\n", ret);
        return;
    }

    ret = tal_venc_init(0, NULL, TY_DSP_MEDIA_VENC_MAX); //底层 TKL 视频编码初始化接口
    if (0 != ret) {
        PR_ERR("ty_dsp_init failed,%d\n", ret);
        return;
    }
    TKL_AUDIO_CONFIG_T pconfig = { -1 };
    ret = tal_ai_init(&pconfig, TY_DSP_MEDIA_AUDIO_MAX); //底层 TKL 硬件音频采集初始化接口
    if (0 != ret) {
        PR_ERR("tal_ai_init failed,%d\n", ret);
        return;
    }
#endif
    /*创建线程*/
    pthread_t main_pid = -1;
    pthread_t sub_pid = -1;
    pthread_t audio_pid = -1;
    pthread_create(&main_pid, NULL, __video_main, NULL);
    pthread_create(&sub_pid, NULL, __video_sub, NULL);
    pthread_create(&audio_pid, NULL, __audio_main, NULL);

    return;
}