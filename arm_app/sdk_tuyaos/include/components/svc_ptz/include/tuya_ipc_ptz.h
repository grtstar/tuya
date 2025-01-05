/**
* @file      tuya_ipc_ptz.h
* @brief     tuya ipc ptz api
* @version   1.0
* @date      2021-11-18
*
* copyright  Copyright (c) tuya.inc 2021
*/
#ifndef __TUYA_IPC_PTZ_H__
#define __TUYA_IPC_PTZ_H__
#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define  MAX_PRESET_NUM 6

/**
* @struct S_PRESET_PTZ_T
* @brief  ptz preset
*/
typedef struct {
    INT_T   pan;
    INT_T   tilt;
    INT_T   zoom;
}S_PRESET_PTZ_T;

/**
* @struct S_PRESET_POSITION_T
* @brief  ptz preset postion
*/
typedef struct {
    CHAR_T          id[32];    /** id in server */
    CHAR_T          name[32];  /** preset point name */
    INT_T           mpId;      /** index ID */
    S_PRESET_PTZ_T  ptz;       /** ptz for preset position */
}S_PRESET_POSITION_T;

/**
* @struct S_PRESET_CFG_T
* @brief  ptz preset config
*/
typedef struct {
    INT_T  num;
    S_PRESET_POSITION_T position[MAX_PRESET_NUM];
}S_PRESET_CFG_T;

/**
* @brief      add one preset point for ipc
* @param[in]  preset_pos    preset position
* @return     OPERATE_RET
* - OPRT_OK   success
* - Others    failed
*/
OPERATE_RET tuya_ipc_preset_add(IN S_PRESET_POSITION_T *preset_pos);

/**
* @brief      del one preset point for ipc
* @param[in]  preset_id    preset_id is the "devId field of the JSON of DP TUYA_DP_PRESET_SET"
* @return     OPERATE_RET
* - OPRT_OK   success
* - Others    failed
*/
OPERATE_RET tuya_ipc_preset_del(IN CHAR_T *preset_id);

/**
* @brief      get all preset positions stored in tuya cloud for ipc
* @param[out] preset_cfg    preset config
* @return     OPERATE_RET
* - OPRT_OK   success
* - Others    failed
*/
OPERATE_RET tuya_ipc_preset_get(OUT S_PRESET_CFG_T *preset_cfg);

/**
* @brief      upload a snapshort for current preset position for ipc
* @param[in]  addr    address of the picture to be uploaded
* @param[in]  size    size of the picture to be uploaded
* @return     OPERATE_RET
* - OPRT_OK   success
* - Others    failed
*/
OPERATE_RET tuya_ipc_preset_add_pic(IN CHAR_T *addr, IN UINT_T size);

/**
* @brief      add one preset point for ipc
* @param[in]  devid      device id
* @param[in]  index      preset index
* @param[in]  preset_pos preset position
* @return     OPERATE_RET
* - OPRT_OK   success
* - Others    failed
*/
OPERATE_RET tuya_ipc_preset_common_add(IN CHAR_T *devid, IN INT_T index, IN S_PRESET_POSITION_T *preset_pos);

/**
* @brief      upload a snapshort for current preset position for xvr
* @param[in]  node_id xvr node id
* @param[in]  index   preset index
* @param[in]  addr    address of the picture to be uploaded
* @param[in]  size    size of the picture to be uploaded
* @return     OPERATE_RET
* - OPRT_OK   success
* - Others    failed
*/
OPERATE_RET tuya_xvr_preset_pic_common_add(IN CHAR_T *node_id, IN INT_T index, IN CHAR_T *addr, IN UINT_T size);

/**
* @brief      delete one preset point for xvr
* @param[in]  node_id    xvr node id
* @param[in]  preset_id  preset position id
* @return     OPERATE_RET
* - OPRT_OK   success
* - Others    failed
*/
OPERATE_RET tuya_xvr_preset_common_delete(IN CHAR_T *node_id, IN CHAR_T *preset_id);

/**
* @brief      add one preset point for xvr
* @param[in]  node_id     xvr node id
* @param[in]  index       preset index
* @param[in]  preset_pos  preset position
* @return     OPERATE_RET
* - OPRT_OK   success
* - Others    failed
*/
OPERATE_RET tuya_xvr_preset_common_add(IN CHAR_T *node_id, IN INT_T index, IN S_PRESET_POSITION_T *preset_pos);

/**
* @brief      get all preset positions stored in tuya cloud for xvr
* @param[in]  node_id    xvr node id
* @param[out] preset_cfg preset config
* @return     OPERATE_RET
* - OPRT_OK   success
* - Others    failed
*/
OPERATE_RET tuya_xvr_preset_common_get(IN CHAR_T *node_id, OUT S_PRESET_CFG_T *preset_cfg);

#ifdef __cplusplus
}
#endif

#endif

