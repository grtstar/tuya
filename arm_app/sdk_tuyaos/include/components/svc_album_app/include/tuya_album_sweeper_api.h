/**
 * @file tuya_album_sweeper_api.h
 * @brief This is sweeper album deal
 * @version 1.0
 * @date 2024-02-07
 *
 * @copyright Copyright 2021-2031 Tuya Inc. All Rights Reserved.
 *
 */

#ifndef __TUYA_ALBUM_SWEEPER_API_H__
#define __TUYA_ALBUM_SWEEPER_API_H__

#include <stdio.h>
#include "tuya_cloud_types.h"
#include "tuya_ipc_media_stream_event.h"
#ifdef __cplusplus
extern "C" {
#endif

// 地图及路径传输相关事件
typedef enum
{
    SWEEPER_FILE_NULL = 0,    
    SWEEPER_FILE_QUERY = 1,    //查询
    SWEEPER_FILE_DOWNLOAD_START = 2,   //开始下载
    SWEEPER_FILE_DOWNLOAD_CANCEL = 3,  //取消下载
    SWEEPER_FILE_DELETE = 4,           //删除
}SWEEPER_TRANSFER_EVENT_E;
typedef struct {
    char filename[48]; //文件名，不带绝对路径
} TUYA_SWEEPER_FILEINFO;

typedef struct {
    int itemCount;       //文件名数量
    TUYA_SWEEPER_FILEINFO itemArr[48];  
} TUYA_SWEEPER_CUSTOMIZE_FILEINFO; //自定义文件名

typedef INT_T (*SWEEPER_FILE_STREAM_EVENT_CB)(IN CONST INT_T channel, IN CONST SWEEPER_TRANSFER_EVENT_E event, IN PVOID_T args);

typedef INT_T (*SWEEPER_AV_STREAM_EVENT_CB)(IN CONST INT_T channel, IN CONST MEDIA_STREAM_EVENT_E event, IN PVOID_T args);

#ifdef __cplusplus
}
#endif

#endif