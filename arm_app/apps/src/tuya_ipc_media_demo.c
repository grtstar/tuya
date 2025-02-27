/*********************************************************************************
  *Copyright(C),2015-2020, 
  *TUYA 
  *www.tuya.comm
  *FileName:    tuya_ipc_media_demo
**********************************************************************************/
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/statfs.h>  

#include "uni_log.h"
#include "tuya_ipc_api.h"
#include "tuya_ipc_stream_storage.h"
#include "tuya_ipc_cloud_storage.h"
#include "tuya_ipc_media_demo.h"

IPC_MEDIA_INFO_S s_media_info = {0};
extern CHAR_T s_raw_path[128];

/* Set audio and video properties */
VOID IPC_APP_Set_Media_Info(VOID)
{
    memset(&s_media_info, 0 , sizeof(IPC_MEDIA_INFO_S));

    /* main stream(HD), video configuration*/
    /* NOTE
    FIRST:If the main stream supports multiple video stream configurations, set each item to the upper limit of the allowed configuration.
    SECOND:E_CHANNEL_VIDEO_MAIN must exist.It is the data source of SDK.
    please close the E_CHANNEL_VIDEO_SUB for only one stream*/
    s_media_info.channel_enable[E_CHANNEL_VIDEO_MAIN] = TRUE;    /* Whether to enable local HD video streaming */
    s_media_info.video_fps[E_CHANNEL_VIDEO_MAIN] = 30;  /* FPS */
    s_media_info.video_gop[E_CHANNEL_VIDEO_MAIN] = 30;  /* GOP */
    s_media_info.video_bitrate[E_CHANNEL_VIDEO_MAIN] = TUYA_VIDEO_BITRATE_2M; /* Rate limit */
    s_media_info.video_width[E_CHANNEL_VIDEO_MAIN] = 1280; /* Single frame resolution of width*/
    s_media_info.video_height[E_CHANNEL_VIDEO_MAIN] = 720;/* Single frame resolution of height */
    s_media_info.video_freq[E_CHANNEL_VIDEO_MAIN] = 90000; /* Clock frequency */
    s_media_info.video_codec[E_CHANNEL_VIDEO_MAIN] = TUYA_CODEC_VIDEO_H264; /* Encoding format */

    /* substream(HD), video configuration */
    /* Please note that if the substream supports multiple video stream configurations, please set each item to the upper limit of the allowed configuration. */
    s_media_info.channel_enable[E_CHANNEL_VIDEO_SUB] = TRUE;     /* Whether to enable local SD video stream */
    s_media_info.video_fps[E_CHANNEL_VIDEO_SUB] = 30;  /* FPS */
    s_media_info.video_gop[E_CHANNEL_VIDEO_SUB] = 30;  /* GOP */
    s_media_info.video_bitrate[E_CHANNEL_VIDEO_SUB] = TUYA_VIDEO_BITRATE_1M; /* Rate limit */
    s_media_info.video_width[E_CHANNEL_VIDEO_SUB] = 1280; /* Single frame resolution of width */
    s_media_info.video_height[E_CHANNEL_VIDEO_SUB] = 720;/* Single frame resolution of height */
    s_media_info.video_freq[E_CHANNEL_VIDEO_SUB] = 90000; /* Clock frequency */
    s_media_info.video_codec[E_CHANNEL_VIDEO_SUB] = TUYA_CODEC_VIDEO_H264; /* Encoding format */

    /* Audio stream configuration. 
    Note: The internal P2P preview, cloud storage, and local storage of the SDK are all use E_CHANNEL_AUDIO data. */
    s_media_info.channel_enable[E_CHANNEL_AUDIO] = TRUE;         /* Whether to enable local sound collection */
    s_media_info.audio_codec[E_CHANNEL_AUDIO] = TUYA_CODEC_AUDIO_PCM;/* Encoding format */
    s_media_info.audio_sample [E_CHANNEL_AUDIO]= TUYA_AUDIO_SAMPLE_8K;/* Sampling Rate */
    s_media_info.audio_databits [E_CHANNEL_AUDIO]= TUYA_AUDIO_DATABITS_16;/* Bit width */
    s_media_info.audio_channel[E_CHANNEL_AUDIO]= TUYA_AUDIO_CHANNEL_MONO;/* channel */
    s_media_info.audio_fps[E_CHANNEL_AUDIO] = 25;/* Fragments per second */

    PR_DEBUG("channel_enable:%d %d %d", s_media_info.channel_enable[0], s_media_info.channel_enable[1], s_media_info.channel_enable[2]);

    PR_DEBUG("fps:%u", s_media_info.video_fps[E_CHANNEL_VIDEO_MAIN]);
    PR_DEBUG("gop:%u", s_media_info.video_gop[E_CHANNEL_VIDEO_MAIN]);
    PR_DEBUG("bitrate:%u kbps", s_media_info.video_bitrate[E_CHANNEL_VIDEO_MAIN]);
    PR_DEBUG("video_main_width:%u", s_media_info.video_width[E_CHANNEL_VIDEO_MAIN]);
    PR_DEBUG("video_main_height:%u", s_media_info.video_height[E_CHANNEL_VIDEO_MAIN]);
    PR_DEBUG("video_freq:%u", s_media_info.video_freq[E_CHANNEL_VIDEO_MAIN]);
    PR_DEBUG("video_codec:%d", s_media_info.video_codec[E_CHANNEL_VIDEO_MAIN]);

    PR_DEBUG("audio_codec:%d", s_media_info.audio_codec[E_CHANNEL_AUDIO]);
    PR_DEBUG("audio_sample:%d", s_media_info.audio_sample[E_CHANNEL_AUDIO]);
    PR_DEBUG("audio_databits:%d", s_media_info.audio_databits[E_CHANNEL_AUDIO]);
    PR_DEBUG("audio_channel:%d", s_media_info.audio_channel[E_CHANNEL_AUDIO]);
}

/*
 * The sample code simulates audio and video by reading and writing files in rawfiles.tar.gz
 */
#define AUDIO_FRAME_SIZE 640
#define AUDIO_FPS 25
#define VIDEO_BUF_SIZE	(1024 * 400) //Maximum frame

/* This is for demo only. Should be replace with real PCM/AAC/G711 encoder output */
void *thread_live_audio(void *arg)
{
    char fullpath[128] = {0};
    sprintf(fullpath, "%s/resource/media/demo_audio.raw", s_raw_path);

	Ring_Buffer_User_Handle_S a_handle = tuya_ipc_ring_buffer_open(0, 0, E_RINGBUFFER_STREAM_A, E_RBUF_WRITE);
	if(a_handle == NULL)
	{
		pthread_exit(0);
	}

    FILE *aFp = fopen(fullpath, "rb");
    if(aFp == NULL)
    {
        printf("can't read live audio file %s\n",fullpath);
        pthread_exit(0);
    }
    char audioBuf[AUDIO_FRAME_SIZE];
    MEDIA_FRAME_S pcm_frame = {0};
    pcm_frame.type = E_AUDIO_FRAME;

    while(1)
    {
        int size = fread(audioBuf, 1, AUDIO_FRAME_SIZE, aFp);
        if(size < AUDIO_FRAME_SIZE)
        {
            rewind(aFp);
            continue;
        }
        int frameRate = AUDIO_FPS;
        int sleepTick = 1000000/frameRate;
        static UINT64_T pts = 0;
        pts += sleepTick;
        pcm_frame.size = size;
        pcm_frame.p_buf = audioBuf;
        pcm_frame.pts = pts;
//        TUYA_APP_Put_Frame(E_CHANNEL_AUDIO,&pcm_frame);
        TUYA_APP_Put_Frame(a_handle,&pcm_frame);


        usleep(sleepTick);
    }

    pthread_exit(0);
}

/* This is for demo only. Should be replace with real H264 encoder output */
int read_one_frame_from_demo_video_file(unsigned char *pVideoBuf, unsigned int offset, unsigned int BufSize, unsigned int *IskeyFrame, unsigned int *FramLen, unsigned int *Frame_start)
{
    int pos = 0;
    int bNeedCal = 0;
    unsigned char NalType=0;
    int idx=0;
    if(BufSize<=5)
    {
        printf("bufSize is too small\n");
        return -1;
    }
    for(pos=0;pos <= BufSize-5;pos++)
    {
        if(pVideoBuf[pos]==0x00
            &&pVideoBuf[pos+1]==0x00
            &&pVideoBuf[pos+2]==0x00
            &&pVideoBuf[pos+3]==0x01)
        {
            NalType = pVideoBuf[pos + 4] & 0x1f;
            if(NalType == 0x7)
            {
                if(bNeedCal==1)
                {
                    *FramLen=pos-idx;
                    return 0;
                }
                
                *IskeyFrame = 1;
                *Frame_start = offset+pos;
                bNeedCal=1;
                idx=pos;
            }
            else if(NalType ==0x1)                
            {
               if(bNeedCal)
               {
                  *FramLen=pos-idx;
                  return 0;
               }
               *Frame_start=offset+pos;
               *IskeyFrame = 0;
               idx = pos;
               bNeedCal=1;
            }
        }
    }

	*FramLen=BufSize;
    return 0;
}

void *thread_live_video(void *arg)
{
    char raw_fullpath[128] = {0};
    char info_fullpath[128] = {0};
    unsigned int FrameLen=0,Frame_start=0;
    unsigned int offset=0;
    unsigned int IsKeyFrame=0;
    unsigned char *pVideoBuf = NULL;
//    sprintf(raw_fullpath, "%s/resource/media/error1.264", s_raw_path);
    sprintf(raw_fullpath, "/userdata/output.264");

    PR_DEBUG("start live video using %s",raw_fullpath);

	Ring_Buffer_User_Handle_S v_handle = tuya_ipc_ring_buffer_open(0, 0, 0, E_RBUF_WRITE);
	if(v_handle == NULL)
	{
		pthread_exit(0);
	}
	Ring_Buffer_User_Handle_S v_sub_handle = tuya_ipc_ring_buffer_open(0, 0, E_RINGBUFFER_STREAM_V_SUB, E_RBUF_WRITE);
	if(v_sub_handle == NULL)
	{
		pthread_exit(0);
	}

    FILE *streamBin_fp = fopen(raw_fullpath, "rb");
    if((streamBin_fp == NULL))
    {
        printf("can't read live video file %s\n",raw_fullpath);
        pthread_exit(0);
    }
    fseek(streamBin_fp, 0, SEEK_END);
    UINT_T file_size = ftell(streamBin_fp);
    fseek(streamBin_fp, 0, SEEK_SET);
    pVideoBuf = malloc(file_size);
    fread(pVideoBuf, 1, file_size, streamBin_fp);

    MEDIA_FRAME_S h264_frame = {0};
    while(1)
    {
        offset=Frame_start+FrameLen;
        if(offset>=file_size)
        {
            offset=0;
        }
        read_one_frame_from_demo_video_file(pVideoBuf+offset, offset, file_size-offset, &IsKeyFrame, &FrameLen, &Frame_start);
        //Note: For I frame of H264, SPS/PPS/SEI/IDR should be combined within one frame, and the NALU separator should NOT be deleted.
        if(IsKeyFrame==1)
        {
            h264_frame.type = E_VIDEO_I_FRAME;
            h264_frame.size = FrameLen;
        }
        else
        {
            h264_frame.type = E_VIDEO_PB_FRAME;
            h264_frame.size = FrameLen;
        }

        //printf("type:%d size:%u\n", h264_frame.type, h264_frame.size);
        int frameRate = 30;
        int sleepTick = 1000000/frameRate;
        static UINT64_T pts = 0;
        pts += sleepTick;

        h264_frame.p_buf = pVideoBuf+Frame_start;
        h264_frame.pts = pts;
#if 0
        /* Send HD video data to the SDK */
        TUYA_APP_Put_Frame(E_CHANNEL_VIDEO_MAIN, &h264_frame);
        /* Send SD video data to the SDK */
        TUYA_APP_Put_Frame(E_CHANNEL_VIDEO_SUB, &h264_frame);
#endif
        /* Send HD video data to the SDK */
        TUYA_APP_Put_Frame(v_handle, &h264_frame);
		TUYA_APP_Put_Frame(v_sub_handle, &h264_frame);

        /* Send SD video data to the SDK */
       // TUYA_APP_Put_Frame(E_CHANNEL_VIDEO_SUB, &h264_frame);

        usleep(sleepTick);
    }

    pthread_exit(0);
}


/*
---------------------------------------------------------------------------------
code related RingBuffer
---------------------------------------------------------------------------------
*/
OPERATE_RET TUYA_APP_Init_Ring_Buffer(VOID)
{
    STATIC BOOL_T s_ring_buffer_inited = FALSE;
    if(s_ring_buffer_inited == TRUE)
    {
        printf("The Ring Buffer Is Already Inited\n");
        return OPRT_OK;
    }

    CHANNEL_E channel;
    OPERATE_RET ret;
    Ring_Buffer_Init_Param_S param;
    for( channel = E_CHANNEL_VIDEO_MAIN; channel < E_CHANNEL_MAX; channel++ )
    {
        printf("init ring buffer Channel:%d Enable:%d\n", channel, s_media_info.channel_enable[channel]);
        if(s_media_info.channel_enable[channel] == TRUE)
        {
            if(channel == E_CHANNEL_AUDIO)
            {
                param.bitrate = s_media_info.audio_sample[E_CHANNEL_AUDIO]*s_media_info.audio_databits[E_CHANNEL_AUDIO]/1024;
                param.fps = s_media_info.audio_fps[E_CHANNEL_AUDIO];
                param.max_buffer_seconds = 0;
                param.requestKeyFrameCB = NULL;
                printf("audio_sample %d, audio_databits %d, audio_fps %d\n",s_media_info.audio_sample[E_CHANNEL_AUDIO],s_media_info.audio_databits[E_CHANNEL_AUDIO],s_media_info.audio_fps[E_CHANNEL_AUDIO]);
                ret = tuya_ipc_ring_buffer_init(0,0,channel,&param);
            }
            else
            {
                param.bitrate = s_media_info.video_bitrate[channel];
                param.fps = s_media_info.video_fps[channel];
                param.max_buffer_seconds = 0;
                param.requestKeyFrameCB = NULL;
                printf("video_bitrate %d, video_fps %d\n",s_media_info.video_bitrate[channel],s_media_info.video_fps[channel]);
                ret = tuya_ipc_ring_buffer_init(0,0,channel,&param);
            }
            if(ret != 0)
            {
                printf("init ring buffer fails. %d %d\n", channel, ret);
                return OPRT_MALLOC_FAILED;
            }
            printf("init ring buffer success. channel:%d\n", channel);
        }
    }

    s_ring_buffer_inited = TRUE;

    return OPRT_OK;
}

OPERATE_RET TUYA_APP_Put_Frame(Ring_Buffer_User_Handle_S handle, IN CONST MEDIA_FRAME_S *p_frame)
{
    PR_TRACE("Put Frame. type:%d size:%u pts:%llu ts:%llu",
             p_frame->type, p_frame->size, p_frame->pts, p_frame->timestamp);

    OPERATE_RET ret = tuya_ipc_ring_buffer_append_data(handle,p_frame->p_buf, p_frame->size,p_frame->type,p_frame->pts);

    if(ret != OPRT_OK)
    {
        PR_ERR("Put Frame Fail.%d  type:%d size:%u pts:%llu ts:%llu",ret,
                  p_frame->type, p_frame->size, p_frame->pts, p_frame->timestamp);
    }
    return ret;
}

// snapshot

//According to different chip platforms, users need to implement the interface of capture.
VOID tuya_ipc_get_snapshot_cb(char* pjbuf,  int* size)
{
    *size = 0;
}


#if 0
OPERATE_RET TUYA_APP_Get_Frame_Backwards(IN CONST CHANNEL_E channel,
                                                  IN CONST USER_INDEX_E user_index,
                                                  IN CONST UINT_T backward_frame_num,
                                                  INOUT MEDIA_FRAME_S *p_frame)
{
    if(p_frame == NULL)
    {
        PR_ERR("input is null");
        return OPRT_INVALID_PARM;
    }

    Ring_Buffer_Node_S *node;
    if(channel == E_CHANNEL_VIDEO_MAIN || channel == E_CHANNEL_VIDEO_SUB)
        node = tuya_ipc_ring_buffer_get_pre_video_frame(channel,user_index,backward_frame_num);
    else
        node = tuya_ipc_ring_buffer_get_pre_audio_frame(channel,user_index,backward_frame_num);
    if(node != NULL)
    {
        p_frame->p_buf = node->rawData;
        p_frame->size = node->size;
        p_frame->timestamp = node->timestamp;
        p_frame->type = node->type;
        p_frame->pts = node->pts;
        return OPRT_OK;
    }
    else
    {
        PR_ERR("Fail to re-locate for user %d backward %d frames, channel %d",user_index,backward_frame_num,channel);
        return OPRT_COM_ERROR;
    }
}

OPERATE_RET TUYA_APP_Get_Frame(IN CONST CHANNEL_E channel, IN CONST USER_INDEX_E user_index, IN CONST BOOL_T isRetry, IN CONST BOOL_T ifBlock, INOUT MEDIA_FRAME_S *p_frame)
{
    if(p_frame == NULL)
    {
        PR_ERR("input is null");
        return OPRT_INVALID_PARM;
    }
    PR_TRACE("Get Frame Called. channel:%d user:%d retry:%d", channel, user_index, isRetry);

    Ring_Buffer_Node_S *node = NULL;
    while(node == NULL)
    {
        if(channel == E_CHANNEL_VIDEO_MAIN || channel == E_CHANNEL_VIDEO_SUB)
        {
            node = tuya_ipc_ring_buffer_get_video_frame(channel,user_index,isRetry);
        }
        else if(channel == E_CHANNEL_AUDIO)
        {
            node = tuya_ipc_ring_buffer_get_audio_frame(channel,user_index,isRetry);
        }
        if(NULL == node)
        {
            if(ifBlock)
            {
                usleep(10*1000);
            }
            else
            {
                return OPRT_NO_MORE_DATA;
            }
        }
    }
    p_frame->p_buf = node->rawData;
    p_frame->size = node->size;
    p_frame->timestamp = node->timestamp;
    p_frame->type = node->type;
    p_frame->pts = node->pts;

    PR_TRACE("Get Frame Success. channel:%d user:%d retry:%d size:%u ts:%ull type:%d pts:%llu",
             channel, user_index, isRetry, p_frame->size, p_frame->timestamp, p_frame->type, p_frame->pts);
    return OPRT_OK;
}
#endif
/*
---------------------------------------------------------------------------------
---------------------------------------------------------------------------------
*/

/*
---------------------------------------------------------------------------------
code related EchoShow
---------------------------------------------------------------------------------
*/
#if ENABLE_ECHO_SHOW == 1

INT_T TUYA_APP_Echoshow_Start(PVOID_T context, PVOID_T priv_data)
{
    printf("echoshow start...\n");

    return 0;
}

INT_T TUYA_APP_Echoshow_Stop(PVOID_T context, PVOID_T priv_data)
{
    printf("echoshow stop...\n");

    return 0;
}

INT_T TUYA_APP_Chromecast_Start(PVOID_T context, PVOID_T priv_data)
{
    printf("chromecast start...\n");

    return 0;
}

INT_T TUYA_APP_Chromecast_Stop(PVOID_T context, PVOID_T priv_data)
{
    printf("chromecast stop...\n");

    return 0;
}

OPERATE_RET TUYA_APP_Enable_EchoShow_Chromecast(VOID)
{
#if 0
     STATIC BOOL_T s_echoshow_inited = FALSE;
     if(s_echoshow_inited == TRUE)
     {
         PR_DEBUG("The EchoShow Is Already Inited");
         return OPRT_OK;
     }

    PR_DEBUG("Init EchoShow");

    TUYA_ECHOSHOW_PARAM_S es_param = {0};

    es_param.pminfo = &s_media_info;
    es_param.cbk.pcontext = NULL;
    es_param.cbk.start = TUYA_APP_Echoshow_Start;
    es_param.cbk.stop = TUYA_APP_Echoshow_Stop;
    /*Channel settings according to requirements*/
    es_param.vchannel = 1;
    es_param.mode = TUYA_ECHOSHOW_MODE_ECHOSHOW;

    tuya_ipc_echoshow_init(&es_param);

    TUYA_CHROMECAST_PARAM_S param = {0};

    param.pminfo = &s_media_info;
    /*Channel settings according to requirements*/
    param.audio_channel = E_CHANNEL_AUDIO_2RD;
    param.video_channel = E_CHANNEL_VIDEO_SUB;
    param.src = TUYA_STREAM_SOURCE_RINGBUFFER;
    param.mode = TUYA_STREAM_TRANSMIT_MODE_ASYNC;
    param.cbk.pcontext = NULL;
    param.cbk.start = TUYA_APP_Chromecast_Start;
    param.cbk.stop = TUYA_APP_Chromecast_Stop;
    param.cbk.get_frame = NULL;

    tuya_ipc_chromecast_init(&param);

    s_echoshow_inited = TRUE;
#endif
    return OPRT_OK;
}
#endif
/*
---------------------------------------------------------------------------------

---------------------------------------------------------------------------------
*/


