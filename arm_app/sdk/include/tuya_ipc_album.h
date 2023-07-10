/*********************************************************************************
  *Copyright(C),2018, 涂鸦科技 www.tuya.comm
  *FileName:    tuya_ipc_stream_storage.h
**********************************************************************************/

#ifndef __TUYA_IPC_ALBUM_H__
#define __TUYA_IPC_ALBUM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "tuya_cloud_error_code.h"
#include "tuya_cloud_types.h"
#include "tuya_ipc_media.h"
#include "tuya_ipc_stream_storage.h"

/* common album start */
#define TUYA_IPC_ALBUM_EMERAGE_FILE "ipc_emergency_record" // 行车记录仪的紧急抓拍
#define TUYA_IPC_ALBUM_DEFAULT "ipc_default" // 默认的本地相册

#define SS_ALBUM_MAX_FILE_NAME_LEN (48) //相册文件名最大长度限制

typedef struct
{
    CHAR_T filename[SS_ALBUM_MAX_FILE_NAME_LEN];
} SS_FILE_PATH;

typedef enum {
    SS_DATA_TYPE_PIC = 1,  //普通图片
    SS_DATA_TYPE_VIDOE = 2,//录像文件
    SS_DATA_TYPE_PANORAMA = 3,//全景拼接图
} SS_DATA_TYPE;

typedef struct {
    CHAR_T channel; // 相机通道号，从0开始
    SS_DATA_TYPE type; // 数据类型，index 1 pic 2 mp4 3Panorama
    CHAR_T fileName[SS_ALBUM_MAX_FILE_NAME_LEN]; // 文件名123456789_1.mp4 123456789_1.jpg  xxx.xxx
    INT_T createTime; // 文件创建时间
    short duration; // mp4文件时长
    int param; // type:3 Panorama时，表示该图作为缩略图
} ALBUM_FILE_INFO;

/*
    data format:
    SS_ALBUM_INDEX_HEAD
    SS_ALBUM_INDEX_ITEM
    SS_ALBUM_INDEX_ITEM
    SS_ALBUM_INDEX_ITEM
    SS_ALBUM_INDEX_ITEM
    ...
*/

typedef struct {
    INT_T idx; // 设备提供，保证唯一性
    CHAR_T valid; // 0 invalid 1 valid
    CHAR_T channel; // 相机通道号，从0开始
    CHAR_T type; // 0 保留 1 pic 2 mp4
    CHAR_T dir; // 0 file 1 dir
    CHAR_T filename[SS_ALBUM_MAX_FILE_NAME_LEN]; // 123456789_1.mp4 123456789_1.jpg  xxx.xxx
    INT_T createTime; // 文件创建时间
    short duration; // mp4 文件时长
    CHAR_T padding[18]; // 扩展
} SS_ALBUM_INDEX_ITEM;

typedef struct {
    UINT_T crc;
    INT_T version;
    CHAR_T magic[16];
    CHAR_T reserved[512 - 28];
    INT_T itemCount; // include invalid items
    SS_ALBUM_INDEX_ITEM itemArr[0];
} SS_ALBUM_INDEX_HEAD; // 索引文件头,512Byte

OPERATE_RET tuya_ipc_album_ss_init(TUYA_IPC_ALBUM_INFO_S* album_info, char* mount_path);

/**
 * \fn OPERATE_RET tuya_ipc_album_get_path(IN CHAR_T* albumName, OUT CHAR_T *filePath, OUT CHAR_T *thumbnailPath)
 * \brief get album path
 * \param[in] albumName: album name
 * \param[out] filePath: path stores video and pic
 * \param[out] thumbnailPath: path stores thumbnail of video and pic
 * \return OPERATE_RET
 */
OPERATE_RET tuya_ipc_album_get_path(IN CHAR_T* albumName, OUT CHAR_T* filePath, OUT CHAR_T* thumbnailPath);

/**
 * \fn OPERATE_RET tuya_ipc_album_write_file_info(IN CHAR_T * albumName, IN ALBUM_FILE_INFO* pInfo)
 * \brief write info of newly added file
 * \param[in] albumName: album name
 * \param[in] pInfo: newly added file info
 * \return OPERATE_RET
 * \warning: not thread safe
 */
OPERATE_RET tuya_ipc_album_write_file_info(IN CHAR_T* albumName, IN ALBUM_FILE_INFO* pInfo);

/**
 * \fn OPERATE_RET tuya_ipc_album_query_by_name(IN CHAR_T* albumName, IN INT_T chan,OUT INT_T* len, OUT SS_ALBUM_INDEX_HEAD** ppIndexFile)
 * \brief get album index info by album name
 * \param[in] albumName: album name
 * \param[in] chan: ipc chan num, start from 0
 * \param[out] len: len of ppIndexFile (SS_ALBUM_INDEX_HEAD + SS_ALBUM_INDEX_ITEM)
 * \param[out] ppIndexFile: SS_ALBUM_INDEX_HEAD index header, followed SS_ALBUM_INDEX_ITEM
 * \return OPERATE_RET
 */
OPERATE_RET tuya_ipc_album_query_by_name(IN CHAR_T* albumName, IN INT_T chan, OUT INT_T* len, OUT SS_ALBUM_INDEX_HEAD** ppIndexFile);

typedef struct tagALBUM_DOWNLOAD_START_INFO {
    UINT_T session_id; // album operation session
    CHAR_T albumName[SS_ALBUM_MAX_FILE_NAME_LEN]; // album name
    INT_T thumbnail; // 1 thumbnail
    INT_T fileTotalCnt; // max 50
    SS_FILE_PATH* pFileInfoArr;
} SS_ALBUM_DOWNLOAD_START_INFO; //start download album

typedef enum {
    SS_ALBUM_DL_IDLE = 0,
    SS_ALBUM_DL_START = 1,
    SS_ALBUM_DL_CANCLE = 5,
} SS_ALBUM_DOWNLOAD_STATUS_E; // try keep same to SS_DOWNLOAD_STATUS_E

/**
 * \fn OPERATE_RET tuya_ipc_ss_download_set_status(IN UINT_T pb_idx, IN SS_ALBUM_DOWNLOAD_STATUS_E new_status)
 * \brief set album download status, start or cancel
 * \param[in] new_status: start or cancel download
 * \param[in] pInfo: dowload file count and info
 * \return OPERATE_RET`
 */
OPERATE_RET tuya_ipc_album_set_download_status(IN SS_ALBUM_DOWNLOAD_STATUS_E new_status, IN SS_ALBUM_DOWNLOAD_START_INFO* pInfo);

/**
 * \fn OPERATE_RET tuya_ipc_album_delete_by_file_info(IN INT_T sessionId, IN CHAR_T * albumName, IN INT_T cnt, IN SS_FILE_PATH* fileInfoArr)
 * \brief delete file info
 * \param[in] sessionId: p2p sesssion id
 * \param[in] albumName: album name
 * \param[in] cnt:  file num to delete
 * \param[in] fileInfoArr: file infos to delete
 * \return OPERATE_RET
 */
OPERATE_RET tuya_ipc_album_delete_by_file_info(IN INT_T sessionId, IN CHAR_T* albumName, IN INT_T cnt, IN SS_FILE_PATH* fileInfoArr);

/**
 * \fn tuya_ipc_album_write_file_start
 * \brief start send file to app by p2p
 * \param[in] strBriefInfo: brief file infomation
 * \return handle , >=0 valid, -1 err
 */
INT_T tuya_ipc_album_write_file_start(IN CONST CHAR_T* ablumName, IN CONST ALBUM_FILE_INFO* pBriefInfo);

/**
 * \fn tuya_ipc_album_write_file_do
 * \brief start send file to app by p2p
 * \param[in] handle: handle
 * \param[in] strfileInfo: file infomation
 * \param[in] timeOut_s: suggest 30s, 0 no_block (current not support),
 * \return ret
 */
OPERATE_RET tuya_ipc_album_write_file_do(IN CONST INT_T handle, IN CONST CHAR_T* image_buf, IN INT_T len, IN CONST ALBUM_FILE_INFO* fileInfo);

/**
 * \fn tuya_ipc_album_write_file_stop
 * \brief stop send file to app by p2p
 * \param[in] handle
 * \return ret
 */
OPERATE_RET tuya_ipc_album_write_file_stop(IN CONST INT_T handle, IN CONST ALBUM_FILE_INFO* pUpdateInfo);

/**
 * \fn tuya_ipc_album_write_file_debug_info
 * \brief show write info
 * \param[in]
 * \return ret
 */
OPERATE_RET tuya_ipc_album_write_file_debug_info();

#ifdef __cplusplus
}
#endif

#endif
