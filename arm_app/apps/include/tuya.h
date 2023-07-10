#pragma once

#include <stdio.h>
#include "tuya_cloud_types.h"
#include "tuya_ipc_api.h"
#include "uni_log.h"
#include "tuya_ipc_media.h"
#include "tuya_ipc_p2p.h"
#include "tuya_ipc_video_msg.h"
//#include "tuya_ipc_chromecast.h"
//#include "tuya_ipc_echo_show.h"
#include "tuya_cloud_com_defs.h"
#include "aes_inf.h"


#if defined(WIFI_GW) && (WIFI_GW==1)
#include "tuya_cloud_wifi_defs.h"
#endif

#include "tuya_cloud_types.h"
#include "tuya_cloud_error_code.h"
#include "tuya_cloud_com_defs.h"

#include "tuya_os_adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    CHAR_T storage_path[IPC_STORAGE_PATH_LEN + 1];/**Path to save sdk cfg ,need to read and write, doesn't loss when poweroff */
    CHAR_T upgrade_file_path[IPC_STORAGE_PATH_LEN + 1];/*Path to save upgrade file when OTA upgrading*/
    CHAR_T sd_base_path[IPC_STORAGE_PATH_LEN + 1];/**SD Card Mount Directory */
    CHAR_T product_key[IPC_PRODUCT_KEY_LEN + 1]; /**< product key */
    CHAR_T uuid[IPC_UUID_LEN + 1]; /*UUID is the unique identification of each device */
    CHAR_T auth_key[IPC_AUTH_KEY_LEN + 1]; /*AUTHKEY is the authentication codes corresponding to UUID, one machine one code, paired with UUID.*/
    CHAR_T p2p_id[IPC_P2P_ID_LEN + 1]; /*p2p_id is no need to provide*/
    CHAR_T dev_sw_version[IPC_SW_VER_LEN + 1]; /*version of the software */
    UINT_T max_p2p_user;/*max num of P2P supports*/
}IPC_MGR_INFO_S;


typedef enum
{
    TUYA_IPC_LOW_POWER_MODE,
    TUYA_IPC_INVALID_MODE
}TUYA_IPC_SDK_STREAM_MODE_E;

typedef VOID (*TUYA_RST_INFORM_CB)(GW_RESET_TYPE_E from);
typedef VOID (*TUYA_RESTART_INFORM_CB)(VOID);

typedef struct
{
	CHAR_T product_key[IPC_PRODUCT_KEY_LEN + 1]; /* one product key(PID) for one product  */
	CHAR_T uuid[IPC_UUID_LEN + 1]; /* one uuid-authkey pair for one device, to register on TUYA coud */
	CHAR_T auth_key[IPC_AUTH_KEY_LEN + 1];
	CHAR_T dev_sw_version[IPC_SW_VER_LEN + 1]; /* software version with format x1.x2.x3.x4.x5.x6, five dots maximum, zero dots minimum */
	CHAR_T cfg_storage_path[IPC_STORAGE_PATH_LEN + 1];
    TUYA_RST_INFORM_CB gw_reset_cb;/* reset callback fucntion, triggered when user unbind device from a account */
    TUYA_RESTART_INFORM_CB gw_restart_cb;/* restart callback function */
    TUYA_IPC_DEV_TYPE_E   dev_type;/*dev type ,eg low power device*/
}TUYA_IPC_SDK_BASE_S;



typedef VOID (*TUYA_IPC_SDK_NET_STATUS_CHANGE_CB)(IN CONST BYTE_T stat);

typedef struct
{
	WIFI_INIT_MODE_E connect_mode;
	TUYA_IPC_SDK_NET_STATUS_CHANGE_CB net_status_change_cb;
}TUYA_IPC_SDK_NET_S;




typedef struct
{
	BOOL_T enable; /*enable == false ,other var is invalid*/
	UINT_T max_p2p_client; /* p2p clinet max connect number*/
    TRANS_DEFAULT_QUALITY_E live_mode;  /* for multi-streaming ipc, the default quality for live preview */
	TRANSFER_EVENT_CB transfer_event_cb; /*transfer event callback*/;
	TRANSFER_REV_AUDIO_CB rev_audio_cb;/*app to dev audio callback*/
}TUYA_IPC_SDK_P2P_S;



typedef VOID (* TUYA_IPC_SDK_DP_QUERY)(IN CONST TY_DP_QUERY_S *dp_query);
typedef VOID (*TUYA_IPC_SDK_RAW_DP_CMD_PROC)(IN CONST TY_RECV_RAW_DP_S *dp);
typedef VOID (*TUYA_IPC_SDK_COMMON_DP_CMD_PROC)(IN CONST TY_RECV_OBJ_DP_S *dp);

typedef struct
{
	TUYA_IPC_SDK_DP_QUERY dp_query;
	TUYA_IPC_SDK_RAW_DP_CMD_PROC  raw_dp_cmd_proc;
	TUYA_IPC_SDK_COMMON_DP_CMD_PROC common_dp_cmd_proc;
}TUYA_IPC_SDK_DP_S;



typedef int (*TUYA_IPC_SDK_DEV_UPGRADE_INFORM_CB)(IN CONST FW_UG_S *fw);

typedef struct
{
    BOOL_T enable ;
    TUYA_IPC_SDK_DEV_UPGRADE_INFORM_CB upgrade_cb;
}TUYA_IPC_SDK_DEMO_UPGRADE_S;



typedef struct
{
    INT_T log_level;/*set log level default: error level*/
    CHAR_T qrcode_token[16]; //connect_method !=2 ,qrcode_token invalid
}TUYA_IPC_SDK_DEBUG_S;



typedef struct
{
	TUYA_IPC_SDK_BASE_S  iot_info;/*sdk base configure information*/
//	TUYA_IPC_SDK_MEDIA_STREAM_S media_info;
	TUYA_IPC_SDK_NET_S net_info;/*net work info*/
	TUYA_IPC_SDK_DEMO_UPGRADE_S upgrade_info;/*fireware upgrade information*/
	TUYA_IPC_SDK_DP_S dp_info;/*date point information*/
	TUYA_IPC_SDK_P2P_S p2p_info;/*p2p information*/
	TUYA_IPC_SDK_DEBUG_S debug_info;/*debug info sets*/
}TUYA_IPC_SDK_RUN_VAR_S;

OPERATE_RET tuya_ipc_sdk_start(IN CONST TUYA_IPC_SDK_RUN_VAR_S * pRunInfo);


typedef enum
{
    IPC_BOOTUP_FINISH,
    IPC_START_WIFI_CFG,
    IPC_REV_WIFI_CFG,
    IPC_CONNECTING_WIFI,
    IPC_MQTT_ONLINE,
    IPC_RESET_SUCCESS,
}IPC_APP_NOTIFY_EVENT_E;

VOID IPC_APP_Reset_System_CB(GW_RESET_TYPE_E reboot_type);

VOID IPC_APP_Restart_Process_CB(VOID);

VOID IPC_APP_Notify_LED_Sound_Status_CB(IPC_APP_NOTIFY_EVENT_E notify_event);


VOID __TUYA_APP_rev_audio_cb(IN CONST TRANSFER_AUDIO_FRAME_S *p_audio_frame, IN CONST TRANSFER_SOURCE_TYPE_E frame_no);
INT_T __TUYA_APP_p2p_event_cb(IN CONST TRANSFER_EVENT_E event, IN CONST PVOID_T args);

/* basic API */
VOID IPC_APP_handle_dp_cmd_objs(IN CONST TY_RECV_OBJ_DP_S *dp_rev);
/* API */
VOID IPC_APP_handle_dp_query_objs(IN CONST TY_DP_QUERY_S *dp_query);
VOID IPC_APP_handle_raw_dp_cmd_objs(IN CONST TY_RECV_RAW_DP_S *dp_rev);

INT_T IPC_APP_Upgrade_Inform_cb(IN CONST FW_UG_S *fw);

void PlayVoice(int v, int mode);

#ifdef __cplusplus
}
#endif