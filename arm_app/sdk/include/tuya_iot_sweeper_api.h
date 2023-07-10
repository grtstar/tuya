/*
tuya_iot_com_api.h
Copyright(C),2018-2020, 涂鸦科技 www.tuya.comm
*/

#ifndef __TUYA_IOT_SWEEPER_API_H
#define __TUYA_IOT_SWEEPER_API_H

#include "tuya_cloud_types.h"
#include "tuya_cloud_com_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_M_MAP_INFO_NUM 5

#define DESCRIPT_STR_LMT  64
#define BITMAP_FILE_STR_LMT  64
#define DATAMAP_FILE_STR_LMT  64
typedef struct {
    UINT_T map_id;
    UINT_T time;
    CHAR_T descript[DESCRIPT_STR_LMT];
    CHAR_T bitmap_file[BITMAP_FILE_STR_LMT];
    CHAR_T datamap_file[DATAMAP_FILE_STR_LMT];
} M_MAP_INFO;

/***********************************************************
*  Function: tuya_iot_map_upload_files
*  Desc:  sweeper function. upload cleaner map info
*  Input: bitmap & data map
*  output: map_id
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_iot_map_upload_files(IN CONST CHAR_T *bitmap_file, IN CONST CHAR_T *datamap_file, IN CONST CHAR_T *descript, OUT CONST UINT_T *map_id);

/***********************************************************
*  Function: tuya_iot_map_upload_buffer_subpack
*  Desc:  sweeper function. upload cleaner map from buffer by subpack
*  Input: bitmap & data map
*  output: map_id
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_iot_map_upload_buffer_subpack(IN CONST UINT_T bitmap_total_len,
                                                    IN CONST CHAR_T *bitmap,
                                                    IN CONST UINT_T bitmap_len,
                                                    IN CONST UINT_T datamap_total_len,
                                                    IN CONST CHAR_T *datamap,
                                                    IN CONST UINT_T datamap_len,
                                                    IN CONST CHAR_T *descript,
                                                    OUT CONST UINT_T *map_id);

/***********************************************************
*  Function: tuya_iot_map_update_files
*  Desc:  sweeper function. update cleaner map info
*  Input: map_id
*  Input: bitmap & data map
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_iot_map_update_files(IN CONST UINT_T map_id, IN CONST CHAR_T *bitmap_file, IN CONST CHAR_T *data_map_file);

/***********************************************************
*  Function: tuya_iot_map_update_buffer_subpack
*  Desc:  sweeper function. update cleaner map from buffer by subpack
*  Input: map_id
*  Input: bitmap & data map
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_iot_map_update_buffer_subpack(IN CONST UINT_T map_id,
                                                    IN CONST UINT_T bitmap_total_len,
                                                    IN CONST CHAR_T *bitmap,
                                                    IN CONST UINT_T bitmap_len,
                                                    IN CONST UINT_T datamap_total_len,
                                                    IN CONST CHAR_T *datamap,
                                                    IN CONST UINT_T datamap_len);

/***********************************************************
*  Function: tuya_iot_map_delete
*  Desc:  sweeper function. delete map files in cloud
*  Input: map_id
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_iot_map_delete(IN CONST UINT_T map_id);

/***********************************************************
*  Function: tuya_iot_get_map_files
*  Desc:  sweeper function. get map files
*  Input: map_id
*  Input: map_path
*  Output: map files under map_path path.
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_iot_get_map_files(IN CONST UINT_T map_id, IN CONST CHAR_T *map_path);


/***********************************************************
*  Function: tuya_iot_get_all_maps_info
*  Desc:  sweeper function. get map files
*  Input: map_info
*  Input: info_len
*  Output: get all map info (Maximum 5 sets of data)
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_iot_get_all_maps_info(OUT M_MAP_INFO *map_info, INOUT UINT8_T *info_len);


OPERATE_RET tuya_sweeper_upload_file(IN CONST CHAR_T *file, IN CONST CHAR_T *cloud_name, IN CONST CHAR_T *descript);


OPERATE_RET tuya_sweeper_init_sdk();


#define DEVIDE_MEDIA_INFO_DEVID                     "devId"
#define DEVIDE_MEDIA_INFO_START_ROW                 "startRow"
#define DEVIDE_MEDIA_INFO_DATA_TYPE                 "datatype"
#define DEVIDE_MEDIA_INFO_DATA_LIST                 "dataList"
#define DEVIDE_MEDIA_INFO_MAPID                     "mapId"
#define DEVIDE_MEDIA_INFO_START_TIME                "startTime"
#define DEVIDE_MEDIA_INFO_HAS_NEXT                  "hasNext"
#define DEVIDE_MEDIA_INFO_SUB_RECORD_ID             "subRecordId"
#define DEVIDE_MEDIA_INFO_END_TIME                  "endTime"
#define DEVIDE_MEDIA_INFO_STATUS                    "status"

/**
* @brief query the map upload info
*
* @param[in] sub_record_id:sub record id
* @param[in] map_id:map id
* @param[in] start:the key of the start (start can be NULL when first call the function)
* @param[in] size:query the return size (suggset not over 500)
* @param[out] result:the result of the query

* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tuya_iot_sweeper_get_device_media_detail(IN INT_T sub_record_id, IN INT_T map_id, IN CHAR_T *start, IN INT_T size, OUT ty_cJSON **result);


/**
* @brief query the latest map upload info
*
* @param[in] start:the key of the start (start can be NULL when first call the function)
* @param[in] size:query the return size (suggset not over 500)
* @param[out] result:the result of the query

* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tuya_iot_sweeper_get_device_media_latest(IN CHAR_T *start, IN INT_T size, OUT ty_cJSON **result);


#ifdef __cplusplus
}
#endif

#endif
