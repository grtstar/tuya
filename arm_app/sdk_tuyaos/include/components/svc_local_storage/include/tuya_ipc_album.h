/**
 * @file tuya_ipc_album.h
 * @brief This is tuya ipc album file
 * @version 1.0
 * @date 2021-11-17
 *
 * @copyright Copyright 2021-2031 Tuya Inc. All Rights Reserved.
 *
 */

#ifndef __TUYA_IPC_ALBUM_H__
#define __TUYA_IPC_ALBUM_H__

#include "tuya_error_code.h"
#include "tuya_cloud_types.h"
#include "tuya_ipc_media.h"
#include "tuya_ipc_stream_storage.h"


#ifdef __cplusplus
extern "C" {
#endif


#define TUYA_IPC_ALBUM_EMERAGE_FILE "ipc_emergency_record"
#define TUYA_IPC_ALBUM_DEFAULT      "ipc_default"
#define TUYA_IPC_ALBUM_CAPTURE_IMAGE "CAPTURE_IMAGE"

#define SS_ALBUM_MAX_FILE_NAME_LEN (48)

typedef enum {
    SS_DATA_TYPE_PIC = 1,
    SS_DATA_TYPE_VIDOE = 2,
    SS_DATA_TYPE_PANORAMA = 3,
} SS_DATA_TYPE_E;

typedef enum {
    SS_ALBUM_DL_IDLE = 0,
    SS_ALBUM_DL_START = 1,
    SS_ALBUM_DL_CANCLE = 5,
} SS_ALBUM_DOWNLOAD_STATUS_E; ///< try keep same to SS_DOWNLOAD_STATUS_E


typedef struct {
    CHAR_T file_name[SS_ALBUM_MAX_FILE_NAME_LEN];
} SS_FILE_PATH_T;

typedef struct {
    CHAR_T channel;
    SS_DATA_TYPE_E type;    ///< 1 pic; 2 mp4; 3 Panorama
    CHAR_T file_name[SS_ALBUM_MAX_FILE_NAME_LEN]; ///< 123456789_1.mp4 123456789_1.jpg  xxx.xxx
    INT_T create_time;
    SHORT_T duration;       ///< mp4 file duration
    INT_T param;            ///< thumbnail if type=3(Panorama)
} ALBUM_FILE_INFO_T;

/*
    data format:
    SS_ALBUM_INDEX_HEAD_T
    SS_ALBUM_INDEX_ITEM_T
    SS_ALBUM_INDEX_ITEM_T
    SS_ALBUM_INDEX_ITEM_T
    SS_ALBUM_INDEX_ITEM_T
    ...
*/

typedef struct {
    INT_T idx;      ///< unique
    CHAR_T valid;   ///< 0 invalid; 1 valid
    CHAR_T channel;
    CHAR_T type;    ///< 0 reserved; 1 pic; 2 mp4
    CHAR_T dir;     ///< 0 file; 1 dir
    CHAR_T file_name[SS_ALBUM_MAX_FILE_NAME_LEN]; ///< 123456789_1.mp4 123456789_1.jpg  xxx.xxx
    INT_T create_time;
    SHORT_T duration; // mp4 file duration
    CHAR_T padding[18];
} SS_ALBUM_INDEX_ITEM_T;

typedef struct {
    UINT_T crc;
    INT_T version;
    CHAR_T magic[16];
    UINT64_T min_idx;
    UINT64_T max_idx;
    CHAR_T reserved[512 - 44];
    INT_T item_count; ///< include invalid items
    SS_ALBUM_INDEX_ITEM_T item_arr[0];
} SS_ALBUM_INDEX_HEAD_T; ///< index header, 512Byte

typedef struct {
    UINT_T session_id; ///< album operation session
    CHAR_T album_name[SS_ALBUM_MAX_FILE_NAME_LEN];
    INT_T thumbnail;
    INT_T file_count; ///< max 50
    SS_FILE_PATH_T* p_file_info_arr;
} SS_ALBUM_DOWNLOAD_START_INFO_T; ///< start download album


/**
 * @brief initialize album storage
 * 
 * @param[in] album_info: album info
 * @param[in] mount_path: storage mount path
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_album_ss_init(IN TUYA_IPC_ALBUM_INFO_T* album_info, IN CHAR_T* mount_path);

/**
 * @brief get album path
 * 
 * @param[in] album_name: album name
 * @param[out] filePath: path stores video and pic
 * @param[out] thumbnailPath: path stores thumbnail of video and pic
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_album_get_path(IN CHAR_T* album_name, OUT CHAR_T* filePath, OUT CHAR_T* thumbnailPath);

/**
 * @brief write info of newly added file
 * 
 * @param[in] album_name: album name
 * @param[in] pInfo: newly added file info
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 * 
 * @warning: not thread safe
 */
OPERATE_RET tuya_ipc_album_write_file_info(IN CHAR_T* album_name, IN ALBUM_FILE_INFO_T* pInfo);

/**
 * @brief get album index info by album name
 * 
 * @param[in] album_name: album name
 * @param[in] chan: ipc chan num, start from 0
 * @param[out] len: len of p_index_file (SS_ALBUM_INDEX_HEAD_T + SS_ALBUM_INDEX_ITEM_T)
 * @param[out] p_index_file: SS_ALBUM_INDEX_HEAD_T index header, followed SS_ALBUM_INDEX_ITEM_T
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_album_query_by_name(IN CHAR_T* album_name, IN INT_T chan, OUT INT_T* len, OUT SS_ALBUM_INDEX_HEAD_T** p_index_file);

/**
 * @brief set album download status, start or cancel
 * 
 * @param[in] new_status: start or cancel download
 * @param[in] p_info: dowload file count and info
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_album_set_download_status(IN SS_ALBUM_DOWNLOAD_STATUS_E new_status, IN SS_ALBUM_DOWNLOAD_START_INFO_T* p_info);

/**
 * @brief delete file info
 * 
 * @param[in] sessionId: p2p sesssion id
 * @param[in] album_name: album name
 * @param[in] cnt: file num to delete
 * @param[in] file_info_arr: file infos to delete
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_album_delete_by_file_info(IN INT_T sessionId, IN CHAR_T* album_name, IN INT_T cnt, IN SS_FILE_PATH_T* file_info_arr);

/**
 * @brief start send file to app by p2p
 * 
 * @param[in] ablum_name: album name
 * @param[in] p_brief_info: brief file infomation
 * 
 * @return a handle(> 0) if success. OPERATE_RET if less than 0, please refer to tuya_error_code.h 
 */
INT_T tuya_ipc_album_write_file_start(IN CONST CHAR_T* ablum_name, IN CONST ALBUM_FILE_INFO_T* p_brief_info);

/**
 * @brief send file to app by p2p
 * 
 * @param[in] handle: get from tuya_ipc_album_write_file_start
 * @param[in] image_buf: image buffer
 * @param[in] len: buffer length
 * @param[in] file_info: album file info
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_album_write_file_do(IN CONST INT_T handle, IN CONST CHAR_T* image_buf, IN INT_T len, IN CONST ALBUM_FILE_INFO_T* file_info);

/**
 * @brief stop send file to app by p2p
 * 
 * @param[in] handle: get from tuya_ipc_album_write_file_start
 * @param[in] p_update_info: album info
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_album_write_file_stop(IN CONST INT_T handle, IN CONST ALBUM_FILE_INFO_T* p_update_info);

/**
 * @brief file name use .MP4 .JPG instead of .mp4 .jpg
 * 
 * @param VOID 
 * 
 * @return VOID 
 */
VOID tuya_ipc_album_set_suffix_upper_case(VOID);

/**
 * @brief show write info
 * 
 * @param VOID 
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_album_write_file_debug_info(VOID);

/**
 * @brief delete oldest file and info in album
 * 
 * @param[in] album_name: which album you want operate
 * @param[in] count: how many files you want delete
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_album_delete_oldest_file(IN CONST CHAR_T *album_name);

/**
 * @brief get file number in album
 * 
 * @param[in] album_name: which album you want operate
 * 
 * @return >=0 on success. Others on error
 */
INT_T tuya_ipc_album_get_file_count(IN CONST CHAR_T *album_name);

#ifdef __cplusplus
}
#endif

#endif
