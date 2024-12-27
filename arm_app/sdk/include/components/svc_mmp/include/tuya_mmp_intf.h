/**
 * @file tuya_mmp_intf.h
 * @author dante (houcl@tuya.com)
 * @brief 
 * @version 0.1
 * @date 2022-04-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef __TUYA_MMP_INTF_H__
#define __TUYA_MMP_INTF_H__

#include "tuya_cloud_types.h"
#include "tuya_list.h"
#include "tuya_ipc_media.h"
#include "tal_network.h"
#include "tuya_mmp.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef enum{
    TUYA_MMP_RES_INVALID = 0,
    TUYA_MMP_RES_LIVE_VIDEO = 1, 
    TUYA_MMP_RES_LIVE_AUDIO, //2
    TUYA_MMP_RES_FILE, //3
    TUYA_MMP_RES_TYPE_MAX
}TUYA_MMP_RES_TYPE_E;


typedef struct{
    INT_T dev;
    INT_T chn;
    IPC_STREAM_E stream;
}TUYA_MMP_LIVE_PARAM_T;

typedef struct{
    CHAR_T *file_buf;
    INT_T file_size;
    CHAR_T file_type[8];
    CHAR_T file_name[32];
    OPERATE_RET (* release)(CHAR_T *file_buf);
}TUYA_MMP_FILE_PARAM_T;

typedef union{
    TUYA_MMP_LIVE_PARAM_T live_param;
    TUYA_MMP_FILE_PARAM_T file_param;
}TUYA_MMP_RES_PARAM_U;

typedef struct
{
    BOOL_T is_finish;
    MEDIA_FRAME_TYPE_E frame_type;
    UINT64_T pts;
    TUYA_MMP_RES_TYPE_E res_type;
    UINT_T length;
}TUYA_MMP_PKT_INFO_T;

typedef struct {
    OPERATE_RET (*on_start)(void* priv);
    OPERATE_RET (*on_data)(TUYA_MMP_PKT_INFO_T* pkt_info, CHAR_T* data, INT_T len, void* priv);
    OPERATE_RET (*on_stop)(void* priv);
    OPERATE_RET (*on_error)(TUYA_MMP_ERROR_CODE_E err_code, void* priv);
    void* priv;
} TUYA_MMP_DATA_SINK_T;

typedef void* TUYA_MMP_RES_HANDLE_T;
typedef void* TUYA_MMP_CUSTOMER_HANDLE_T;

TUYA_MMP_RES_HANDLE_T tuya_mmp_add_resource(TUYA_MMP_RES_TYPE_E res_type, TUYA_MMP_RES_PARAM_U res_param, TIME_T expire_time);
OPERATE_RET tuya_mmp_del_resource(TUYA_MMP_RES_HANDLE_T handle);

OPERATE_RET tuya_mmp_resource_mgt_init();

TUYA_MMP_CUSTOMER_HANDLE_T tuya_mmp_start_get_uri(TUYA_IP_ADDR_T ip, UINT_T port,UINT_T prio,UINT_T buffer_size, CHAR_T *uri, TUYA_MMP_DATA_SINK_T* psink);

OPERATE_RET tuya_mmp_stop_get_uri(TUYA_MMP_CUSTOMER_HANDLE_T handle);

OPERATE_RET tuya_mmp_client_init();

#ifdef __cplusplus
}
#endif


#endif