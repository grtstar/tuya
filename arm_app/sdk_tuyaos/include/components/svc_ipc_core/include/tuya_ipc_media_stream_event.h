#ifndef _TUYA_IPC_MEDIA_STREAM_EVENT_H_
#define _TUYA_IPC_MEDIA_STREAM_EVENT_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "tuya_cloud_types.h"


/**************************enum define***************************************/

// 媒体流相关事件
typedef enum
{
    // 枚举 0~29 留给与硬件相关的事件
    MEDIA_STREAM_NULL = 0,
    MEDIA_STREAM_SPEAKER_START = 1, 
    MEDIA_STREAM_SPEAKER_STOP = 2,
   
    MEDIA_STREAM_LIVE_VIDEO_START = 30,
    MEDIA_STREAM_LIVE_VIDEO_STOP = 31,
    MEDIA_STREAM_LIVE_AUDIO_START = 32,
    MEDIA_STREAM_LIVE_AUDIO_STOP = 33,
    MEDIA_STREAM_LIVE_VIDEO_CLARITY_SET = 34,
    MEDIA_STREAM_LIVE_VIDEO_CLARITY_QUERY = 35,         /* query clarity informations*/
    MEDIA_STREAM_LIVE_LOAD_ADJUST = 36,
    MEDIA_STREAM_PLAYBACK_LOAD_ADJUST = 37,
    MEDIA_STREAM_PLAYBACK_QUERY_MONTH_SIMPLIFY = 38,    /* query storage info of month  */
    MEDIA_STREAM_PLAYBACK_QUERY_DAY_TS = 39,            /* query storage info of day */
    MEDIA_STREAM_PLAYBACK_START_TS = 40,                /* start playback */
    MEDIA_STREAM_PLAYBACK_PAUSE = 41,                   /* pause playback */
    MEDIA_STREAM_PLAYBACK_RESUME = 42,                  /* resume playback */
    MEDIA_STREAM_PLAYBACK_MUTE = 43,                    /* mute playback */
    MEDIA_STREAM_PLAYBACK_UNMUTE = 44,                  /* unmute playback */
    MEDIA_STREAM_PLAYBACK_STOP = 45,                    /* stop playback */ 
    MEDIA_STREAM_PLAYBACK_SET_SPEED = 46,               /*set playback speed*/
    MEDIA_STREAM_ABILITY_QUERY = 47,                    /* query the alibity of audion video strraming */
    MEDIA_STREAM_DOWNLOAD_START = 48,                   /* start to download */
    MEDIA_STREAM_DOWNLOAD_STOP = 49,                    /* abondoned */
    MEDIA_STREAM_DOWNLOAD_PAUSE = 50,
    MEDIA_STREAM_DOWNLOAD_RESUME = 51,
    MEDIA_STREAM_DOWNLOAD_CANCLE = 52,

    /*与互联互通相关*/
    MEDIA_STREAM_LIVE_VIDEO_SEND_START = 60,            //对端请求被拉视频流
    MEDIA_STREAM_LIVE_VIDEO_SEND_STOP = 61,             //对端请求停止被拉视频流
    MEDIA_STREAM_LIVE_AUDIO_SEND_START = 62,            //对端请求被拉音频流
    MEDIA_STREAM_LIVE_AUDIO_SEND_STOP = 63,             //对端请求停止被拉音频流
        
    MEDIA_STREAM_STREAMING_VIDEO_START = 100,
    MEDIA_STREAM_STREAMING_VIDEO_STOP = 101,

    MEDIA_STREAM_DOWNLOAD_IMAGE = 201,                  /* download image */
    MEDIA_STREAM_PLAYBACK_DELETE = 202,                 /* delete video */
    MEDIA_STREAM_ALBUM_QUERY = 203,
    MEDIA_STREAM_ALBUM_DOWNLOAD_START = 204,
    MEDIA_STREAM_ALBUM_DOWNLOAD_CANCEL = 205,
    MEDIA_STREAM_ALBUM_DELETE = 206,

    //xvr相关
    MEDIA_STREAM_VIDEO_START_GW = 300,         /**< 直播开始视频，参数为C2C_TRANS_CTRL_VIDEO_START*/
    MEDIA_STREAM_VIDEO_STOP_GW,         /**< 直播结束视频，参数为C2C_TRANS_CTRL_VIDEO_STOP*/
    MEDIA_STREAM_AUDIO_START_GW,         /**< 直播开始音频，参数为C2C_TRANS_CTRL_AUDIO_START*/
    MEDIA_STREAM_AUDIO_STOP_GW,         /**< 直播结束音频，参数为C2C_TRANS_CTRL_AUDIO_STOP*/
    MEDIA_STREAM_VIDEO_CLARITY_SET_GW, /**< 设置视频直播清晰度 ，参数为*/
    MEDIA_STREAM_VIDEO_CLARITY_QUERY_GW, /**< 查询视频直播清晰度 ，参数为*/
    MEDIA_STREAM_LOAD_ADJUST_GW, /**< 直播负载变更 ，参数为*/
    MEDIA_STREAM_PLAYBACK_LOAD_ADJUST_GW, /**< 开始回放 ，参数为*/
    MEDIA_STREAM_PLAYBACK_QUERY_MONTH_SIMPLIFY_GW, /* 按月查询本地视频信息，参数为  */
    MEDIA_STREAM_PLAYBACK_QUERY_DAY_TS_GW, /* 按天查询本地视频信息，参数为  */

    MEDIA_STREAM_PLAYBACK_START_TS_GW, /* 开始回放视频，参数为  */
    MEDIA_STREAM_PLAYBACK_PAUSE_GW, /**< 暂停回放视频，参数为  */
    MEDIA_STREAM_PLAYBACK_RESUME_GW, /**< 继续回放视频，参数为  */
    MEDIA_STREAM_PLAYBACK_MUTE_GW, /**< 静音，参数为  */
    MEDIA_STREAM_PLAYBACK_UNMUTE_GW, /**< 取消静音，参数为  */
    MEDIA_STREAM_PLAYBACK_STOP_GW, /**< 停止回放视频，参数为  */

    MEDIA_STREAM_PLAYBACK_SPEED_GW, /**< 设置回放倍速，参数为  */
    MEDIA_STREAM_DOWNLOAD_START_GW, /**< 下载开始*/
    MEDIA_STREAM_DOWNLOAD_PAUSE_GW, /**< 下载暂停  */
    MEDIA_STREAM_DOWNLOAD_RESUME_GW,/**< 下载恢复*/
    MEDIA_STREAM_DOWNLOAD_CANCLE_GW,/**< 下载停止*/

    MEDIA_STREAM_SPEAKER_START_GW, /**< 开始对讲，无参数 */
    MEDIA_STREAM_SPEAKER_STOP_GW,  /**< 停止对讲，无参数 */
    MEDIA_STREAM_ABILITY_QUERY_GW,/**< 能力查询 C2C_MEDIA_STREAM_QUERY_FIXED_ABI_REQ*/
    MEDIA_STREAM_CONN_START_GW,    /**< 开启连接 */
    MEDIA_STREAM_PLAYBACK_DELETE_GW  , /* delete video */

    //for page mode play back enum
    MEDIA_STREAM_PLAYBACK_QUERY_DAY_TS_PAGE_MODE, /* query storage info of day with page id*/
    MEDIA_STREAM_PLAYBACK_QUERY_EVENT_DAY_TS_PAGE_MODE, /* query storage evnet info of day with page id */

}MEDIA_STREAM_EVENT_E;


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
    TY_VIDEO_CLARITY_THIRD,
    TY_VIDEO_CLARITY_FOURTH,
    TY_VIDEO_CLARITY_MAX
}TRANSFER_VIDEO_CLARITY_TYPE_E;

    

/**************************struct define***************************************/
typedef INT_T (*MEDIA_STREAM_EVENT_CB)(IN CONST INT_T device, IN CONST INT_T channel, IN CONST MEDIA_STREAM_EVENT_E event, IN PVOID_T args);


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
}C2C_TRANS_QUERY_PB_DAY_INNER_REQ;

typedef struct{
    unsigned int channel;
    unsigned int year;
    unsigned int month;
    unsigned int day;  
    PLAY_BACK_ALARM_INFO_ARR * alarm_arr;
    unsigned int ipcChan;
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
    unsigned int ipcChan;
}C2C_TRANS_QUERY_PB_MONTH_REQ, C2C_TRANS_QUERY_PB_MONTH_RESP;

typedef struct tagC2CCmdQueryPlaybackInfoByMonthInner{
	unsigned int channel;
    unsigned int year;
    unsigned int month;
    unsigned int day;   //list days that have playback data. Use each bit for one day. For example day=26496=0110 0111 1000 0000 means day 7/8/9/19/13/14 have playback data.
}C2C_TRANS_QUERY_PB_MONTH_INNER_REQ, C2C_TRANS_QUERY_PB_MONTH_INNER_RESP;


typedef struct tagC2C_TRANS_CTRL_PB_START{
    unsigned int channel;
    PLAYBACK_TIME_S time_sect;   
    UINT_T playTime;  /* the actual playback time, in second */
    TRANSFER_SOURCE_TYPE_E type;
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

typedef enum
{
    TUYA_DOWNLOAD_VIDEO = 0,
    TUYA_DOWNLOAD_ALBUM,
    TUYA_DOWNLOAD_MAX
}TUYA_DOWNLOAD_DATA_TYPE;


/***********************************album protocol ****************************************/
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


// 文件传输的状态
typedef enum
{
    TY_DATA_TRANSFER_IDLE,
    TY_DATA_TRANSFER_START,
    TY_DATA_TRANSFER_PROCESS,
    TY_DATA_TRANSFER_END,
    TY_DATA_TRANSFER_ONCE,
    TY_DATA_TRANSFER_CANCEL,
    TY_DATA_TRANSFER_MAX
}TY_DATA_TRANSFER_STAT;

/***********************************album protocol end ****************************************/


/***********************************xvr  protocol start ****************************************/
typedef struct{
    unsigned int channel;
    unsigned int idx;
    char subdid[64];
    unsigned int year;                                  // 要查询的年份
    unsigned int month;                                 // 要查询的月份
    unsigned int day;                                   // 要查询的天数
}C2C_TRANS_QUERY_GW_PB_DAY_REQ;

typedef struct{
    unsigned int channel;
    unsigned int idx;//会话的索引。
    unsigned int map_chan_index;;//一个绘话中，绑定的通道。用户透传回来即可。
    char subdid[64];
    unsigned int year;                                  // 要查询的年份
    unsigned int month;                                 // 要查询的月份
    unsigned int day;                                   // 要查询的天数
    PLAY_BACK_ALARM_INFO_ARR * alarm_arr;                    // 用户返回的查询结果
}C2C_TRANS_QUERY_GW_PB_DAY_RESP;

/**
UINT一共有32位，每1位表示对应天数是否有数据，最右边一位表示第0天。
比如 day = 26496 = B0110 0111 1000 0000
那么表示第7,8,9,10,13,14天有回放数据。
 */
// 按月查询有回放数据的天 request, response
typedef struct tagC2CCmdQueryGWPlaybackInfoByMonth{
    unsigned int channel;
    unsigned int idx;//会话的索引。
    unsigned int map_chan_index;//绘话中，绑定的通道。用户透传回来即可。
    char subdid[64];
    unsigned int year;                                  // 要查询的年份
    unsigned int month;                                 // 要查询的月份
    unsigned int day;                                  // 有回放数据的天
}C2C_TRANS_QUERY_GW_PB_MONTH_REQ, C2C_TRANS_QUERY_GW_PB_MONTH_RESP;


// request
//回放相关操作结构体
typedef struct tagC2C_TRANS_CTRL_GW_PB_START{
    unsigned int channel;
    unsigned int idx;
    char subdid[64];
    PLAYBACK_TIME_S time_sect;
    UINT_T playTime;  /**< 实际回放开始时间戳（以秒为单位） */
}C2C_TRANS_CTRL_GW_PB_START;

typedef struct tagC2C_TRANS_CTRL_GW_PB_STOP{
    unsigned int channel;
    unsigned int idx;
    char subdid[64];
}C2C_TRANS_CTRL_GW_PB_STOP;

typedef struct tagC2C_TRANS_CTRL_GW_PB_PAUSE{
    unsigned int channel;
    unsigned int idx;
    char subdid[64];
}C2C_TRANS_CTRL_GW_PB_PAUSE,C2C_TRANS_CTRL_GW_PB_RESUME;

typedef struct tagC2C_TRANS_CTRL_GW_PB_MUTE{
    unsigned int channel;
    unsigned int idx;
    char subdid[64];
}C2C_TRANS_CTRL_GW_PB_MUTE,C2C_TRANS_CTRL_GW_PB_UNMUTE;

// 能力集查询 C2C_CMD_QUERY_FIXED_ABILITY
// request, response
typedef struct tagC2CCmdQueryGWFixedAbility{
    unsigned int channel;
    unsigned int idx;
    char subdid[64];
    unsigned int ability_mask;//能力结果按位赋值
}C2C_TRANS_QUERY_GW_FIXED_ABI_REQ, C2C_TRANS_QUERY_GW_FIXED_ABI_RESP;

/**
 * \brief 直播模式下申请修改或者查询清晰度回调参数结构体
 * \struct C2C_TRANS_LIVE_CLARITY_PARAM_S
 */
typedef struct
{
    unsigned int channel;
    unsigned int idx;
    char subdid[64];
    TRANSFER_VIDEO_CLARITY_TYPE_E clarity; /**< 视频清晰度 */
    VOID *pReserved;
}C2C_TRANS_LIVE_GW_CLARITY_PARAM_S;

//预览相关操作结构体
typedef struct tagC2C_TRANS_CTRL_GW_LIVE_VIDEO{
    unsigned int channel;
    unsigned int idx;
    char subdid[64];
}C2C_TRANS_CTRL_GW_VIDEO_START,C2C_TRANS_CTRL_GW_VIDEO_STOP;

typedef struct tagC2C_TRANS_CTRL_GW_LIVE_AUDIO{
    unsigned int channel;
    unsigned int idx;
    char subdid[64];
}C2C_TRANS_CTRL_GW_AUDIO_START,C2C_TRANS_CTRL_GW_AUDIO_STOP;

typedef struct tagC2C_TRANS_CTRL_GW_SPEAKER{
    unsigned int channel;
    unsigned int idx;
    char subdid[64];
}C2C_TRANS_CTRL_GW_SPEAKER_START,C2C_TRANS_CTRL_GW_SPEAKER_STOP;

typedef struct tagC2C_TRANS_CTRL_GW_DEV_CONN{
    unsigned int channel;
    unsigned int idx;
    char subdid[64];
}C2C_TRANS_CTRL_GW_DEV_CONN;

typedef struct tagC2C_TRANS_CTRL_PB_SET_SPEED_GW{
    char devid[64];
    unsigned int channel;
    unsigned int speed;
}C2C_TRANS_CTRL_PB_SET_SPEED_GW;


// 回放数据删除 按天request
typedef struct tagC2C_TRANS_CTRL_PB_DELDATA_BYDAY_GW_REQ{
    char subdid[64];
    unsigned int channel;
    unsigned int year;                                          // 要删除的年份
    unsigned int month;                                         // 要删除的月份
    unsigned int day;                                           // 要删除的天数
}C2C_TRANS_CTRL_PB_DELDATA_BYDAY_GW_REQ;

typedef struct tagC2C_CMD_PROTOCOL_VERSION{
    unsigned int version;       //高位主版本号，低16位次版本号
}C2C_CMD_PROTOCOL_VERSION;
typedef struct{
    char subid[64];
    unsigned int channel;
    unsigned int year;
    unsigned int month;
    unsigned int day;
    int page_id;
}C2C_TRANS_QUERY_PB_DAY_V2_REQ,C2C_TRRANS_QUERY_EVENT_PB_DAY_REQ;
#pragma pack(4)
typedef struct{
    char subid[64];
    unsigned int channel;
    unsigned int year;
    unsigned int month;
    unsigned int day;
    int page_id;
    int total_cnt;
    int page_size;
    PLAY_BACK_ALARM_INFO_ARR  *alarm_arr;
    int idx;//会话索引
}C2C_TRANS_QUERY_PB_DAY_V2_RESP;
typedef struct {
    unsigned int start_timestamp; /* start timestamp in second of playback */
    unsigned int end_timestamp;
    int type;
    char pic_id[20];
}C2C_PB_EVENT_INFO_S;
typedef struct {
    int version;
    int event_cnt;
    C2C_PB_EVENT_INFO_S event_info_arr[0];
}C2C_PB_EVENT_INFO_ARR_S;
typedef struct{
    char subid[64];
    unsigned int channel;
    unsigned int year;
    unsigned int month;
    unsigned int day;
    int page_id;
    int total_cnt;
    int page_size;
    C2C_PB_EVENT_INFO_ARR_S * event_arr;
    int idx;
}C2C_TRANS_QUERY_EVENT_PB_DAY_RESP;
#pragma pack()
typedef struct tagC2C_TRANS_CTRL_GW_DL_STOP{
    char devid[64];
    unsigned int channel;
}C2C_TRANS_CTRL_GW_DL_STOP,C2C_TRANS_CTRL_GW_DL_PAUSE,C2C_TRANS_CTRL_GW_DL_RESUME,C2C_TRANS_CTRL_GW_DL_CANCLE;
typedef struct tagC2C_TRANS_CTRL_GW_DL_START{
    char devid[64];
    unsigned int channel;
    unsigned int downloadStartTime;
    unsigned int downloadEndTime;
}C2C_TRANS_CTRL_GW_DL_START;
/***********************************xvr  protocol end ****************************************/





/**************************function define***************************************/

OPERATE_RET tuya_ipc_media_stream_register_event_cb(MEDIA_STREAM_EVENT_CB event_cb);

OPERATE_RET tuya_ipc_media_stream_event_call( INT_T device, INT_T channel, MEDIA_STREAM_EVENT_E event, PVOID_T args);


#ifdef __cplusplus
}
#endif

#endif /*_TUYA_IPC_MEDIA_STREAM_EVENT_H_*/
