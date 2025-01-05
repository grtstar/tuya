/*********************************************************************************
  *Copyright(C),2017, 涂鸦科技 www.tuya.comm
  *FileName:    tuya_ipc_p2p.h
**********************************************************************************/

#ifndef __TUYA_IPC_P2P_H__
#define __TUYA_IPC_P2P_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "tuya_cloud_types.h"
#include "tuya_cloud_error_code.h"
#include "tuya_ipc_media.h"

/**
 * \brief  p2p callback message enums
 * \struct TRANSFER_EVENT_E
 */
typedef enum
{
    TRANS_LIVE_VIDEO_START = 0,
    TRANS_LIVE_VIDEO_STOP, 
    TRANS_LIVE_AUDIO_START,
    TRANS_LIVE_AUDIO_STOP,
    TRANS_LIVE_VIDEO_CLARITY_SET, /* set clarity from APP */
    TRANS_LIVE_VIDEO_CLARITY_QUERY, /* query clarity informations*/
    TRANS_LIVE_LOAD_ADJUST,
    TRANS_PLAYBACK_LOAD_ADJUST,
    TRANS_PLAYBACK_QUERY_MONTH_SIMPLIFY, /* query storage info of month  */
    TRANS_PLAYBACK_QUERY_DAY_TS, /* query storage info of day */

    TRANS_PLAYBACK_START_TS, /* start playback */
    TRANS_PLAYBACK_PAUSE, /* pause playback */
    TRANS_PLAYBACK_RESUME, /* resume playback */
    TRANS_PLAYBACK_MUTE, /* mute playback */
    TRANS_PLAYBACK_UNMUTE, /* unmute playback */
    TRANS_PLAYBACK_STOP, /* stop playback */ 
    TRANS_PLAYBACK_SET_SPEED, /*set playback speed*/


    TRANS_SPEAKER_START, /* start APP-to-IPC speak */
    TRANS_SPEAKER_STOP,  /* stop APP-to-IPC speak */
    TRANS_ABILITY_QUERY,/* query the alibity of audion video strraming */
    
    TRANS_DOWNLOAD_START,   /* start to download */
    TRANS_DOWNLOAD_STOP,    /* abondoned */
    TRANS_DOWNLOAD_PAUSE,
    TRANS_DOWNLOAD_RESUME,
    TRANS_DOWNLOAD_CANCLE,
        
    TRANS_STREAMING_VIDEO_START = 100,
    TRANS_STREAMING_VIDEO_STOP = 101,

    TRANS_DOWNLOAD_IMAGE = 201, /* download image */
    TRANS_PLAYBACK_DELETE = 202, /* delete video */
    TRANS_ALBUM_QUERY = 203,
    TRANS_ALBUM_DOWNLOAD_START = 204,
    TRANS_ALBUM_DOWNLOAD_CANCEL = 205,
    TRANS_ALBUM_DELETE = 206,
}TRANSFER_EVENT_E;

typedef enum
{
    TRANS_EVENT_SUCCESS = 0,                        /* 返回成功 */
    TRANS_EVENT_SPEAKER_ISUSED       = 10,          /* speker 已被使用，不同的TRANSFER_SOURCE_TYPE_E */
    TRANS_EVENT_SPEAKER_REPSTART     = 11,          /* speker 重复开启，同一个TRANSFER_SOURCE_TYPE_E */
    TRANS_EVENT_SPEAKER_STOPFAILED   = 12,          /* speker stop 失败*/
    TRANS_EVENT_SPEAKER_INVALID      = 99
}TRANSFER_EVENT_RETURN_E;


typedef enum
{
    TRANSFER_SOURCE_TYPE_P2P    = 1,
    TRANSFER_SOURCE_TYPE_WEBRTC = 2,
    TRANSFER_SOURCE_TYPE_STREAMER = 3,
} TRANSFER_SOURCE_TYPE_E;

/**
 * \brief P2P online status
 * \enum TRANSFER_ONLINE_E
 */
typedef enum
{
    TY_DEVICE_OFFLINE,
    TY_DEVICE_ONLINE,
}TRANSFER_ONLINE_E;

    
typedef enum{
    TY_CMD_QUERY_IPC_FIXED_ABILITY_TYPE_VIDEO = 0x1,      // if support video
    TY_CMD_QUERY_IPC_FIXED_ABILITY_TYPE_SPEAKER = 0x2,    // if support speaker
    TY_CMD_QUERY_IPC_FIXED_ABILITY_TYPE_MIC = 0x4,        // is support MIC
}TY_CMD_QUERY_IPC_FIXED_ABILITY_TYPE;


// request, response
typedef struct tagC2CCmdQueryFixedAbility{
    unsigned int channel;
    unsigned int ability_mask;//ability is assigned by bit
}C2C_TRANS_QUERY_FIXED_ABI_REQ, C2C_TRANS_QUERY_FIXED_ABI_RESP;

typedef enum
{
    TY_VIDEO_CLARITY_STANDARD = 0,
    TY_VIDEO_CLARITY_HIGH,
}TRANSFER_VIDEO_CLARITY_TYPE_E;

typedef struct
{
    TRANSFER_VIDEO_CLARITY_TYPE_E clarity;
    VOID *pReserved;
}C2C_TRANS_LIVE_CLARITY_PARAM_S;

typedef struct tagC2C_TRANS_CTRL_LIVE_VIDEO{
    unsigned int channel;
    unsigned int type;      //拉流类型
}C2C_TRANS_CTRL_VIDEO_START,C2C_TRANS_CTRL_VIDEO_STOP;

typedef struct tagC2C_TRANS_CTRL_LIVE_AUDIO{
    unsigned int channel;   
}C2C_TRANS_CTRL_AUDIO_START,C2C_TRANS_CTRL_AUDIO_STOP;


typedef struct
{
    UINT_T start_timestamp; /* start timestamp in second of playback */
    UINT_T end_timestamp;   /* end timestamp in second of playback */
} PLAYBACK_TIME_S;

typedef struct tagPLAY_BACK_ALARM_FRAGMENT{
    unsigned int type;//not used now
	PLAYBACK_TIME_S time_sect;
}PLAY_BACK_ALARM_FRAGMENT;

typedef struct{
    unsigned int file_count;                            // file count of the day
    PLAY_BACK_ALARM_FRAGMENT file_arr[0];                  // play back file array
}PLAY_BACK_ALARM_INFO_ARR;

typedef struct{
    unsigned int channel;
    unsigned int year;
    unsigned int month;
    unsigned int day;  
}C2C_TRANS_QUERY_PB_DAY_REQ;

typedef struct{
    unsigned int channel;
    unsigned int year;
    unsigned int month;
    unsigned int day;  
    PLAY_BACK_ALARM_INFO_ARR * alarm_arr;
}C2C_TRANS_QUERY_PB_DAY_RESP;


// 回放数据删除 按天request 
typedef struct tagC2C_TRANS_CTRL_PB_DELDATA_BYDAY_REQ{
    unsigned int channel;
    unsigned int year;                                          // 要删除的年份
    unsigned int month;                                         // 要删除的月份
    unsigned int day;                                           // 要删除的天数
}C2C_TRANS_CTRL_PB_DELDATA_BYDAY_REQ;

typedef struct tagC2C_TRANS_CTRL_PB_DOWNLOAD_IMAGE_S{
    unsigned int channel;
    PLAYBACK_TIME_S time_sect;                                  // 开始下载时间点
    char reserved[32];
    int result;                      // 结果，可以扩展错误码TY_C2C_CMD_IO_CTRL_STATUS_CODE
    int image_fileLength ;                                      //  文件长度 后面紧跟着h文件内容过来
    unsigned char *pBuffer;                                    // 文件内容
}C2C_TRANS_CTRL_PB_DOWNLOAD_IMAGE_PARAM_S;

// query playback data by month
typedef struct tagC2CCmdQueryPlaybackInfoByMonth{
	unsigned int channel;
    unsigned int year;
    unsigned int month;
    unsigned int day;   //list days that have playback data. Use each bit for one day. For example day=26496=0110 0111 1000 0000 means day 7/8/9/19/13/14 have playback data.
}C2C_TRANS_QUERY_PB_MONTH_REQ, C2C_TRANS_QUERY_PB_MONTH_RESP;


typedef struct tagC2C_TRANS_CTRL_PB_START{
    unsigned int channel;
    PLAYBACK_TIME_S time_sect;   
    UINT_T playTime;  /* the actual playback time, in second */
}C2C_TRANS_CTRL_PB_START;

typedef struct tagC2C_TRANS_CTRL_PB_STOP{
    unsigned int channel;
}C2C_TRANS_CTRL_PB_STOP;

typedef struct tagC2C_TRANS_CTRL_PB_PAUSE{
    unsigned int channel;
}C2C_TRANS_CTRL_PB_PAUSE,C2C_TRANS_CTRL_PB_RESUME;

typedef struct tagC2C_TRANS_CTRL_PB_MUTE{
    unsigned int channel;
}C2C_TRANS_CTRL_PB_MUTE,C2C_TRANS_CTRL_PB_UNMUTE;

typedef struct tagC2C_TRANS_CTRL_PB_SET_SPEED{
    unsigned int channel;
    unsigned int speed;
}C2C_TRANS_CTRL_PB_SET_SPEED;


/**
 * \brief network load change callback struct
 * \note NOT supported now
 */
typedef struct
{
    INT_T client_index;
    INT_T curr_load_level; /**< 0:best 5:worst */
    INT_T new_load_level; /**< 0:best 5:worst */

    VOID *pReserved;
}C2C_TRANS_PB_LOAD_PARAM_S;

typedef struct
{
    INT_T client_index;
    INT_T curr_load_level; /**< 0:best 5:worst */
    INT_T new_load_level; /**< 0:best 5:worst */

    VOID *pReserved;
}C2C_TRANS_LIVE_LOAD_PARAM_S;


typedef struct
{
    UINT_T start_timestamp; /* download start time, in second */
    UINT_T end_timestamp;   /* download end time, in second */
} DOWNLOAD_TIME_S;

typedef struct tagC2C_TRANS_CTRL_DL_START{
    unsigned int channel;
    unsigned int fileNum;
    unsigned int downloadStartTime;
    unsigned int downloadEndTime;
    PLAYBACK_TIME_S *pFileInfo;   
}C2C_TRANS_CTRL_DL_START;


typedef struct tagC2C_TRANS_CTRL_DL_STOP{
    unsigned int channel;  
}C2C_TRANS_CTRL_DL_STOP,C2C_TRANS_CTRL_DL_PAUSE,C2C_TRANS_CTRL_DL_RESUME,C2C_TRANS_CTRL_DL_CANCLE;

/**
 * \brief audio frame struct for P2P
 * \struct TRANSFER_AUDIO_FRAME_S
 */
typedef struct
{
    TUYA_CODEC_ID audio_codec;
    TUYA_AUDIO_SAMPLE_E audio_sample;
    TUYA_AUDIO_DATABITS_E audio_databits;
    TUYA_AUDIO_CHANNEL_E audio_channel;
    BYTE_T *p_audio_buf;
    UINT_T buf_len;
    UINT64_T pts;
    UINT64_T timestamp;//in milliseconds
    VOID *p_reserved;
}TRANSFER_AUDIO_FRAME_S;

/**
 * \brief video frame type for P2P
 * \enum TRANSFER_VIDEO_FRAME_TYPE_E
 */
typedef enum
{
    TY_VIDEO_FRAME_PBFRAME,     /**< P/B frame */
    TY_VIDEO_FRAME_IFRAME,     /**< I frame */
}TRANSFER_VIDEO_FRAME_TYPE_E;

/**
 * \brief video frame struct for P2P
 * \struct TRANSFER_VIDEO_FRAME_S
 */
typedef struct
{
    TUYA_CODEC_ID video_codec;
    TRANSFER_VIDEO_FRAME_TYPE_E video_frame_type;
    BYTE_T *p_video_buf;
    UINT_T buf_len;
    UINT64_T pts;
    UINT64_T timestamp;   //in milliseconds
    VOID *p_reserved;
}TRANSFER_VIDEO_FRAME_S;


/**
 * \typedef TRANSFER_EVENT_CB
 * \brief P2P transfer events callback function
 * \param[in] event: event type
 * \param[in] args: event info needed
 */
typedef INT_T (*TRANSFER_EVENT_CB)(IN CONST TRANSFER_EVENT_E event, IN CONST PVOID_T args);

/**
 * \typedef TRANSFER_REV_AUDIO_CB
 * \brief audio receiving (APP->IPC) callback function
 * \param [in] p_audio_frame: one frame received from APP via P2P
 * \param [in] src_type: protocol type of APP->IPC
 */
typedef VOID (*TRANSFER_REV_AUDIO_CB)(IN CONST TRANSFER_AUDIO_FRAME_S *p_audio_frame, IN CONST TRANSFER_SOURCE_TYPE_E src_type);

/**
 * \typedef TRANSFER_ONLINE_CB
 * \brief callback function when P2P status changes
 */
typedef VOID (*TRANSFER_ONLINE_CB)(IN TRANSFER_ONLINE_E status);

/**
 * \brief quality for live P2P transferring
 * \enum TRANS_LIVE_QUALITY_E
 */
typedef enum
{
    TRANS_LIVE_QUALITY_MAX = 0,     /**< ex. 640*480, 15fps, 320kbps (or 1280x720, 5fps, 320kbps) */
    TRANS_LIVE_QUALITY_HIGH,        /**< ex. 640*480, 10fps, 256kbps */
    TRANS_LIVE_QUALITY_MIDDLE,      /**< ex. 320*240, 15fps, 256kbps */
    TRANS_LIVE_QUALITY_LOW,         /**< ex. 320*240, 10fps, 128kbps */
    TRANS_LIVE_QUALITY_MIN,         /**< ex. 160*120, 10fps, 64kbps */
}TRANS_LIVE_QUALITY_E;

/**
 * \brief default quality for live P2P transferring
 * \enum TRANS_DEFAULT_QUALITY_E
 */
typedef enum
{
    TRANS_DEFAULT_STANDARD = 0,     /* ex. 640*480, 15fps */
    TRANS_DEFAULT_HIGH,        /* ex. 1920*1080, 20fps */
}TRANS_DEFAULT_QUALITY_E;

/**
 * \brief P2P settings
 * \struct TUYA_IPC_TRANSFER_VAR_S
 */
typedef struct
{
    TRANSFER_ONLINE_CB online_cb; /* callback function when P2P status changes */
    TRANSFER_REV_AUDIO_CB on_rev_audio_cb; /* audio receiving (APP->IPC) callback function */
    TUYA_CODEC_ID rev_audio_codec; /* supported audio codec type for data APP->IPC */
    TUYA_AUDIO_SAMPLE_E audio_sample;/* supported audio sampling for data APP->IPC  */
    TUYA_AUDIO_DATABITS_E audio_databits;/* supported audio databits for data APP->IPC  */
    TUYA_AUDIO_CHANNEL_E audio_channel;/* supported audio channel type for data APP->IPC  */
    TRANSFER_EVENT_CB on_event_cb; /* p2p event callback function */
    TRANS_LIVE_QUALITY_E live_quality;
    INT_T max_client_num;
    IPC_MEDIA_INFO_S AVInfo;
    TRANS_DEFAULT_QUALITY_E defLiveMode;  /* for multi-streaming ipc, the default quality for live preview */  
    VOID *p_reserved;
}TUYA_IPC_TRANSFER_VAR_S;

/**
 * \fn OPERATE_RET tuya_ipc_tranfser_init(IN CONST TUYA_IPC_TRANSFER_VAR_S *p_var)
 * \brief initialize tuya P2P
 * \suggestion do init after ipc has been activated(mqtt online)
 * \param[in] p_var P2P settings
 * \return OPERATE_RET
 */
OPERATE_RET tuya_ipc_tranfser_init(IN CONST TUYA_IPC_TRANSFER_VAR_S *p_var);

/**
 * \fn OPERATE_RET tuya_ipc_tranfser_close()
 * \brief close all P2P conections, live preivew & playback
 * \note 
 * \param[in]
 * \return OPERATE_RET
 */
OPERATE_RET tuya_ipc_tranfser_close(VOID);

/**
 * \fn OPERATE_RET tuya_ipc_tranfser_quit(VOID)
 * \brief quit tuya P2P, free resources, memory e.g.
 * \return OPERATE_RET
 */
OPERATE_RET tuya_ipc_tranfser_quit(VOID);

/**
 * \fn OPERATE_RET tuya_ipc_playback_send_video_frame(IN CONST UINT_T client, IN CONST TRANSFER_VIDEO_FRAME_S *p_video_frame)
 * \brief send playback video frame to APP via P2P channel
 * \param[in] client cliend id
 * \param[in] p_video_frame
 * \return OPERATE_RET
 */
OPERATE_RET tuya_ipc_playback_send_video_frame(IN CONST UINT_T client, IN CONST TRANSFER_VIDEO_FRAME_S *p_video_frame);

/**
 * \fn OPERATE_RET tuya_ipc_playback_send_audio_frame(IN CONST UINT_T client, IN CONST TRANSFER_AUDIO_FRAME_S *p_audio_frame)
 * \brief send playback audio frame to APP via P2P channel
 * \param[in] client cliend id
 * \param[in] p_audio_frame
 * \return OPERATE_RET
 */
OPERATE_RET tuya_ipc_playback_send_audio_frame(IN CONST UINT_T client, IN CONST TRANSFER_AUDIO_FRAME_S *p_audio_frame);

/**
 * \fn OPERATE_RET tuya_ipc_playback_send_fragment_end(IN CONST UINT_T client, IN CONST PLAYBACK_TIME_S * fgmt)
 * \brief notify client(APP) playback fragment is finished, send frag info to app
 * \param[in] client cliend id
 * \return OPERATE_RET
 */
OPERATE_RET tuya_ipc_playback_send_fragment_end(IN CONST UINT_T client, IN CONST PLAYBACK_TIME_S * fgmt);

/**
 * \fn OPERATE_RET tuya_ipc_playback_send_finish(IN CONST UINT_T client)
 * \brief notify client(APP) playback data is finished, no more data outgoing
 * \param[in] client cliend id
 * \return OPERATE_RET
 */
OPERATE_RET tuya_ipc_playback_send_finish(IN CONST UINT_T client);

/**
 * \brief P2P connection information
 * \struct TUYA_IPC_TRANSFER_VAR_S
 */
typedef struct
{
    UCHAR_T p2p_mode; /**< 0: P2P mode, 1: Relay mode, 2: LAN mode, 255: Not connected. */
    UCHAR_T local_nat_type; /**< The local NAT type, 0: Unknown type, 1: Type 1, 2: Type 2, 3: Type 3, 10: TCP only */
    UCHAR_T remote_nat_type; /**< The remote NAT type, 0: Unknown type, 1: Type 1, 2: Type 2, 3: Type 3, 10: TCP only */
    UCHAR_T relay_type; /**< 0: Not Relay, 1: UDP Relay, 2: TCP Relay */

    VOID *p_reserved;
}CLIENT_CONNECT_INFO_S;

/**
 * \fn OPERATE_RET tuya_ipc_get_client_conn_info(OUT UINT_T *p_client_num, OUT CLIENT_CONNECT_INFO_S **p_p_conn_info)
 * \brief get P2P connection information
 * \param[out] p_client_num: current connected client number
 * \param[out] p_p_conn_info
 * \return OPERATE_RET
 */
OPERATE_RET tuya_ipc_get_client_conn_info(OUT UINT_T *p_client_num, OUT CLIENT_CONNECT_INFO_S **p_p_conn_info);

/**
 * \fn OPERATE_RET tuya_ipc_free_client_conn_info(IN CLIENT_CONNECT_INFO_S *p_conn_info)
 * \brief free resource required by tuya_ipc_get_client_conn_info in param p_p_conn_info
 * \param[in] p_conn_info
 * \return OPERATE_RET
 */
OPERATE_RET tuya_ipc_free_client_conn_info(IN CLIENT_CONNECT_INFO_S *p_conn_info);

typedef enum
{
    TUYA_DOWNLOAD_VIDEO = 0,
    TUYA_DOWNLOAD_ALBUM,
    TUYA_DOWNLOAD_MAX
}TUYA_DOWNLOAD_DATA_TYPE;
/***********************************************************
*  Function: tuya_ipc_4_app_download_data
*  Note:  download data transfer api
*  Input: client ,pData data buff; pHead  size for pData
*  Output: none
*  Return: 
***********************************************************/
OPERATE_RET tuya_ipc_4_app_download_data(IN CONST UINT_T client, IN CONST STORAGE_FRAME_HEAD_S * pHead, IN CONST CHAR_T * pData);
OPERATE_RET tuya_ipc_4_app_download_data_v2(IN CONST UINT_T client, TUYA_DOWNLOAD_DATA_TYPE type, IN CONST void * pHead, IN CONST CHAR_T * pData);

/***********************************************************
*  Function: tuya_ipc_4_app_download_status
*  Note:  download status 
*  Input: client ,percent(0-100),cur only 100 in use
*  Output: none
*  Return: 
***********************************************************/
OPERATE_RET tuya_ipc_4_app_download_status(IN CONST UINT_T client, IN CONST UINT_T percent);

/***********************************************************
*  Function: tuya_ipc_4_app_download_is_send_over
*  Note:  download status 
*  Input: client
*  Output: True/False
*  Return: 
***********************************************************/
OPERATE_RET tuya_ipc_4_app_download_is_send_over(IN CONST UINT_T client);

/***********************************************************
*  Function: tuya_ipc_tranfser_secret_mode
*  Note:  user set p2p to secret mode
*  Input: mode TRUE/FALSE secret/no secret
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_ipc_tranfser_secret_mode(BOOL_T mode);

    
/**
 * \fn OPERATE_RET tuya_ipc_get_client_online_num()
 * \param[in] 
 * \return INT_T cnt/-1
 */
INT_T tuya_ipc_get_client_online_num();

/***********************************************************
*  Function: tuya_ipc_meadia_pause
*  Note:  user set media pause (web,streamer,p2p)
*  Input: none
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_ipc_media_service_pause(void);

/***********************************************************
*  Function: tuya_ipc_meadia_resume
*  Note:  user set media resume (web,streamer,p2p)
*  Input: none
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_ipc_media_service_resume(void);

/***********************************************************
*  Function: tuya_ipc_delete_video_finish
*  Note:  delete video finish
*  Input: client
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_ipc_delete_video_finish(IN CONST UINT_T client);
OPERATE_RET tuya_ipc_delete_video_finish_v2(IN CONST UINT_T client, TUYA_DOWNLOAD_DATA_TYPE type, int success);

/*********************album protocol **************************/
#define TUYA_ALBUM_APP_FILE_NAME_MAX_LEN (48)
#define IPC_SWEEPER_ROBOT "ipc_sweeper_robot"
typedef struct {
    unsigned int channel; // 目前不需要，保留
    char albumName[48];
    int fileLen;
    void* pIndexFile;
} C2C_QUERY_ALBUM_REQ; // 查询请求头
typedef struct tagC2C_ALBUM_INDEX_ITEM {
    int idx; // 设备提供并保证唯一性
    char valid; // 0 invalid, 1 valid
    char channel; // 0  1通道号
    char type; // 0 保留，1 pic 2 mp4 3全景拼接图(文件夹) 4二进制文件 5流文件
    char dir; // 0 file 1 dir
    char filename[48]; // 123456789_1.mp4 123456789_1.jpg  xxx.xxx
    int createTime; // 文件创建时间
    short duration; // 视频文件时长
    char reserved[18];
} C2C_ALBUM_INDEX_ITEM; // 索引Item
typedef struct {
    char reserved[512 - 4]; // 保留,共512
    int itemCount; // include invalid items
    C2C_ALBUM_INDEX_ITEM itemArr[0];
} C2C_ALBUM_INDEX_HEAD; //查询返回:520 = 8 + 512,索引文件头+item

typedef struct {
    unsigned int channel; // 目前业务不需要，保留
    int result; // 查询返回结果
    char reserved[512 - 4]; // 保留,共512
    int itemCount; // include invalid items
    C2C_ALBUM_INDEX_ITEM itemArr[0];
} C2C_CMD_IO_CTRL_ALBUM_QUERY_RESP; //查询返回:520 = 8 + 512,索引文件头+item

typedef struct tagC2C_CMD_IO_CTRL_ALBUM_fileInfo {
    char filename[48]; //文件名，不带绝对路径
} C2C_CMD_IO_CTRL_ALBUM_fileInfo;
typedef struct tagC2C_CMD_IO_CTRL_ALBUM_DOWNLOAD_START {
    unsigned int channel; //暂无用保留
    int operation; // 参见 TY_CMD_IO_CTRL_DOWNLOAD_OP
    char albumName[48];
    int thumbnail; // 0 原图 ，1 缩略图
    int fileTotalCnt; //max 50
    C2C_CMD_IO_CTRL_ALBUM_fileInfo pFileInfoArr[0];
} C2C_CMD_IO_CTRL_ALBUM_DOWNLOAD_START;
typedef struct tagC2C_ALBUM_DOWNLOAD_CANCEL {
    unsigned int channel; //暂无用保留
    char albumName[48];
} C2C_ALBUM_DOWNLOAD_CANCEL;

typedef struct tagC2C_CMD_IO_CTRL_ALBUM_DELETE {
    unsigned int channel;
    char albumName[48];
    int fileNum; // -1 全部，其他：文件个数
    char res[64];
    C2C_CMD_IO_CTRL_ALBUM_fileInfo pFileInfoArr[0];
} C2C_CMD_IO_CTRL_ALBUM_DELETE; //删除文件

typedef struct {
    int reqId;
    int fileIndex; // start from 0
    int fileCnt; // max 50
    char fileName[48]; // 文件名
    int packageSize; // 当前文件片段的实际数据长度
    int fileSize; // 文件大小
    int fileEnd; // 文件结束标志,最后一个片段10KB
} C2C_DOWNLOAD_ALBUM_HEAD; //下载数据头

typedef enum {
    E_FILE_TYPE_2_APP_PANORAMA = 1, //全景拼接图
} FILE_TYPE_2_APP_E;
typedef struct {
    FILE_TYPE_2_APP_E fileType;
    int param; // 全景拼接图时，为子图总数
} TUYA_IPC_BRIEF_FILE_INFO_4_APP;

/**
 * \fn tuya_ipc_start_send_file_to_app
 * \brief start send file to app by p2p
 * \param[in] strBriefInfo: brief file infomation
 * \return handle , >=0 valid, -1 err
 */
OPERATE_RET tuya_ipc_start_send_file_to_app(IN CONST TUYA_IPC_BRIEF_FILE_INFO_4_APP* pStrBriefInfo);

/**
 * \fn tuya_ipc_stop_send_file_to_app
 * \brief stop send file to app by p2p
 * \param[in] handle
 * \return ret
 */
OPERATE_RET tuya_ipc_stop_send_file_to_app(IN CONST INT_T handle);

typedef struct {
    CHAR_T* fileName; //最长48字节，若为null，采用SDK内部命名
    INT_T len;
    CHAR_T* buff;
} TUYA_IPC_FILE_INFO_4_APP;
/**
 * \fn tuya_ipc_send_file_to_app
 * \brief start send file to app by p2p
 * \param[in] handle: handle
 * \param[in] strfileInfo: file infomation
 * \param[in] timeOut_s: suggest 30s, 0 no_block (current not support),
 * \return ret
 */
OPERATE_RET tuya_ipc_send_file_to_app(IN CONST INT_T handle, IN CONST TUYA_IPC_FILE_INFO_4_APP* pStrfileInfo, IN CONST INT_T timeOut_s);

typedef enum {
    SWEEPER_ALBUM_FILE_TYPE_MIN = 0,
    SWEEPER_ALBUM_FILE_MAP = SWEEPER_ALBUM_FILE_TYPE_MIN,//map file
    SWEEPER_ALBUM_FILE_CLEAN_PATH = 1,
    SWEEPER_ALBUM_FILE_NAVPATH = 2,
    SWEEPER_ALBUM_FILE_TYPE_MAX = SWEEPER_ALBUM_FILE_NAVPATH,

    SWEEPER_ALBUM_STREAM_TYPE_MIN = 3,
    SWEEPER_ALBUM_STREAM_MAP = SWEEPER_ALBUM_STREAM_TYPE_MIN, // map stream , devcie should send map file to app continue
    SWEEPER_ALBUM_STREAM_CLEAN_PATH = 4,
    SWEEPER_ALBUM_STREAM_NAVPATH = 5,
    SWEEPER_ALBUM_STREAM_TYPE_MAX = SWEEPER_ALBUM_STREAM_NAVPATH,

    SWEEPER_ALBUM_FILE_ALL_TYPE_MAX = SWEEPER_ALBUM_STREAM_TYPE_MAX, //最大值 5
    SWEEPER_ALBUM_FILE_ALL_TYPE_COUNT, //个数 6
} SWEEPER_ALBUM_FILE_TYPE_E;

typedef enum {
    SWEEPER_TRANS_NULL,
    SWEEPER_TRANS_FILE, //文件传输
    SWEEPER_TRANS_STREAM, //文件流传输
} SWEEPER_TRANS_MODE_E;
/**
 * \fn tuya_ipc_sweeper_set_file_info
 * \brief transfer file info into buff pointered by pIndexFile by tuya SDK
 * \param[in] fileArray: arr[0] means SWEEPER_ALBUM_FILE_TYPE_MIN, refer to SWEEPER_ALBUM_FILE_TYPE_E
 * \param[in] pIndexFileInfo: file infomation
 * \param[in] fileInfoLen: file infomation len
 * \return ret
 */
INT_T tuya_ipc_sweeper_convert_file_info(IN INT_T* fileArray, OUT VOID** pIndexFileInfo, OUT INT_T* fileInfoLen);

/**
 * \fn tuya_ipc_sweeper_get_file_info
 * \brief parse download file info into file Array
 * \param[in] srcfileInfo: handle
 * \param[in] fileArray: array of file infomation
 * \param[in] arrSize: array size 
 * \return ret
 */
INT_T tuya_ipc_sweeper_parse_file_info(IN C2C_CMD_IO_CTRL_ALBUM_DOWNLOAD_START* srcfileInfo, INOUT INT_T* fileArray, IN INT_T arrSize);

/**
 * \fn simple_album_send_data_with_buff
 * \brief send file to app by p2p
 * \param[in] client: handle
 * \param[in] fileCnt: file count
   * \param[in] userFileName: file name, can be null
  * \param[in] fileLen: file len
 * \param[in] fileBuff: file buff
 * \return ret
 */
INT_T tuya_ipc_sweeper_send_data_with_buff(IN INT_T client, SWEEPER_ALBUM_FILE_TYPE_E type , IN INT_T fileLen, IN CHAR_T* fileBuff);
/**
 * \fn simple_album_send_finish_2_app
 * \brief tell app sending is finished
 * \param[in] client: session client
 * \return ret
 */
INT_T tuya_ipc_sweeper_send_finish_2_app(IN INT_T client);

/*********************album protocol **************************/

/**
 * \fn tuya_ipc_p2p_keep_alive
 * \brief keep p2p connection alive
 * \param[in] client: handle
 * \return ret
 */
OPERATE_RET tuya_ipc_p2p_keep_alive(IN INT_T client);

#ifdef __cplusplus
}
#endif

#endif
