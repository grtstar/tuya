#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string>
#include <fstream>
#include <lcm/lcm-cpp.hpp>

#include <mars_message/OtaMsg.hpp>
#include <mars_message/OtaAck.hpp>
#include <mars_message/Event.hpp>

#include "tuya_ipc_media.h"
#include "tuya_cloud_com_defs.h"
#include "tuya_cloud_types.h"
#include "tuya_ipc_p2p.h"
#include "tuya_iot_sweeper_api.h"

#include "tuya_ipc_media_demo.h"
#include "tuya_ipc_sweeper_demo.h"
#include "tuya.h"
#include "tuya_robot.h"
#include "utils/json.hpp"
#include "utils/shell.hpp"
#include "voice.h"
#include "tuya_utils.h"

extern void TuyaHandleDPQuery(int dpId);
extern void TuyaHandleRawDPCmd(int dpId, uint8_t *data, int len);
extern void TuyaHandleDPCmd(TY_OBJ_DP_S *dp);
using json = nlohmann::json;
#define UPGRADE_SCRIPT   "./ota/upgrade.sh"
#define APP_OTA_SCRIPT   "./ota/app_ota.sh"
#define MCU_OTA_SCRIPT   "./ota/mcu_ota.sh"
#define SYSTEM_OTA_SCRIPT   "./ota/system_ota.sh"
#define APP_VERSION_FILE "./version/soft_version.json"
#define APP_UPGRADE_VERSION_FILE "./version/%s/soft_version.json"

VOID IPC_APP_Notify_LED_Sound_Status_CB(IPC_APP_NOTIFY_EVENT_E notify_event)
{
    printf("curr event:%d \r\n", notify_event);
    switch (notify_event)
    {
        case IPC_BOOTUP_FINISH: /* Startup success */
        {
            break;
        }
        case IPC_START_WIFI_CFG: /* Start configuring the network */
        {
            break;
        }
        case IPC_REV_WIFI_CFG: /* Receive network configuration information */
        {
            break;
        }
        case IPC_CONNECTING_WIFI: /* Start Connecting WIFI */
        {
            break;
        }
        case IPC_MQTT_ONLINE: /* MQTT on-line */
        {
            break;
        }
        case IPC_RESET_SUCCESS: /* Reset completed */
        {
            break;
        }
        default:
        {
            break;
        }
    }
}

VOID IPC_APP_Reset_System_CB(GW_RESET_TYPE_E type)
{
    printf("reset ipc success. please restart the ipc %d\n", type);
    IPC_APP_Notify_LED_Sound_Status_CB(IPC_RESET_SUCCESS);
    //TODO
    /* Developers need to restart IPC operations */
    switch(type)
    {
        case GW_LOCAL_RESET_FACTORY:
        {
            //system("./reset_factory.sh &");
            break;
        }
        case GW_REMOTE_UNACTIVE:
        {
            mars_message::Event evt = {0, 0};
            if(!TuyaComm::Get()->Send("ty_unactive", &evt, 200))
            {
                system("./reset_wifi.sh &");
            }
            break;
        }
        case GW_LOCAL_UNACTIVE:
        {
            mars_message::Event evt = {0, 0};
            if(!TuyaComm::Get()->Send("ty_unactive", &evt, 200))
            {
                system("./reset_wifi.sh &");
            }
            break;
        }
        case GW_REMOTE_RESET_FACTORY:
        {
            system("./reset_factory.sh &");
            break;
        }
        case GW_RESET_DATA_FACTORY:
        {
            system("./reset_data.sh &");
            break;
        }
    }
}


VOID IPC_APP_Restart_Process_CB(VOID)
{
    printf("sdk internal restart request. please restart the ipc\n");
    //TODO
    /* Developers need to implement restart operations. Restart the process or restart the device. */
}


/* Callback functions for transporting events */
INT_T __TUYA_APP_p2p_event_cb(IN CONST TRANSFER_EVENT_E event, IN CONST PVOID_T args)
{
    int ret = 0;
    PR_DEBUG("p2p rev event cb=[%d] ", event);
    switch (event)
    {
        case TRANS_ALBUM_QUERY: /* query album */
        {
            C2C_QUERY_ALBUM_REQ* pSrcType = (C2C_QUERY_ALBUM_REQ*)args;
            if (0 == strcmp(IPC_SWEEPER_ROBOT, pSrcType->albumName)) {
                ret = tuya_ipc_sweeper_album_cb(event, args);
            }
            break;
        }
        case TRANS_ALBUM_DOWNLOAD_START: /* start download album */
        {
            C2C_CMD_IO_CTRL_ALBUM_DOWNLOAD_START* pSrcType = (C2C_CMD_IO_CTRL_ALBUM_DOWNLOAD_START*)args;
           if (0 == strcmp(IPC_SWEEPER_ROBOT, pSrcType->albumName)) {
                ret = tuya_ipc_sweeper_album_cb(event, args);
            }
            break;
        }
        case TRANS_ALBUM_DOWNLOAD_CANCEL: // cancel album
        {
            C2C_ALBUM_DOWNLOAD_CANCEL* pSrcType = (C2C_ALBUM_DOWNLOAD_CANCEL*)args;
            printf("%s downlaod cancle\n", pSrcType->albumName);
            if (0 == strcmp(IPC_SWEEPER_ROBOT, pSrcType->albumName)) {
                ret = tuya_ipc_sweeper_album_cb(event, args);
            }
            break;
        }
        case TRANS_ALBUM_DELETE: //delete
        {
            C2C_CMD_IO_CTRL_ALBUM_DELETE* pSrcType = (C2C_CMD_IO_CTRL_ALBUM_DELETE*)args;
           if (0 == strcmp(IPC_SWEEPER_ROBOT, pSrcType->albumName)) {
                ret = tuya_ipc_sweeper_album_cb(event, args);
            }
            break;
        }
        default:
            break;
    }
    return ret;
}

VOID __TUYA_APP_rev_audio_cb(IN CONST TRANSFER_AUDIO_FRAME_S *p_audio_frame, IN CONST TRANSFER_SOURCE_TYPE_E src_type)
{
    #if 0
    MEDIA_FRAME_S audio_frame = {0};
    audio_frame.p_buf = p_audio_frame->p_audio_buf;
    audio_frame.size = p_audio_frame->buf_len;

    PR_TRACE("Rev Audio. size:%u audio_codec:%d audio_sample:%d audio_databits:%d audio_channel:%d",p_audio_frame->buf_len,
             p_audio_frame->audio_codec, p_audio_frame->audio_sample, p_audio_frame->audio_databits, p_audio_frame->audio_channel);

    TUYA_APP_Rev_Audio_CB( &audio_frame, TUYA_AUDIO_SAMPLE_8K, TUYA_AUDIO_DATABITS_16, TUYA_AUDIO_CHANNEL_MONO);
    #endif
}

typedef VOID (*TUYA_DP_HANDLER)(IN TY_OBJ_DP_S *p_obj_dp);
typedef struct
{
    BYTE_T dp_id;
    TUYA_DP_HANDLER handler;
}TUYA_DP_INFO_S;

STATIC TUYA_DP_INFO_S s_dp_table[] =
{

};


VOID IPC_APP_handle_dp_query_objs(IN CONST TY_DP_QUERY_S *dp_query)
{
    INT_T table_idx = 0;
    INT_T table_count = ( sizeof(s_dp_table) / sizeof(s_dp_table[0]) );
    INT_T index = 0;

    
    for(index = 0; index < dp_query->cnt; index++)
    {
        TuyaHandleDPQuery(dp_query->dpid[index]);
        // if(dp_simulation_filter)
        // {
        //     if(dp_simulation_filter(dp_query->dpid[index]) == 0)
        //     {
        //         continue;
        //     }
        // }
        for(table_idx = 0; table_idx < table_count; table_idx++)
        {
            if(s_dp_table[table_idx].dp_id == dp_query->dpid[index])
            {
                s_dp_table[table_idx].handler(NULL);
                break;
            }
        }
    }  
}

typedef VOID (*TUYA_RAW_DP_HANDLER)(IN CONST TY_RECV_RAW_DP_S *p_obj_dp);
typedef struct
{
    BYTE_T dp_id;
    TUYA_RAW_DP_HANDLER handler;
}TUYA_RAW_DP_INFO_S;

STATIC TUYA_RAW_DP_INFO_S s_raw_dp_table[] =
{
};


VOID IPC_APP_handle_raw_dp_cmd_objs(IN CONST TY_RECV_RAW_DP_S *dp_rev)
{

    INT_T table_idx = 0;
    INT_T table_count = ( sizeof(s_raw_dp_table) / sizeof(s_raw_dp_table[0]) );
    INT_T index = 0;
    TuyaHandleRawDPCmd(dp_rev->dpid, (uint8_t *)dp_rev->data, dp_rev->len);
    for(table_idx = 0; table_idx < table_count; table_idx++)
    {
        if(s_raw_dp_table[table_idx].dp_id == dp_rev->dpid)
        {
            s_raw_dp_table[table_idx].handler(dp_rev);
            break;
        }
    }
}

VOID IPC_APP_handle_dp_cmd_objs(IN CONST TY_RECV_OBJ_DP_S *dp_rev)
{
    TY_OBJ_DP_S *dp_data = (TY_OBJ_DP_S *)(dp_rev->dps);
    UINT_T cnt = dp_rev->dps_cnt;
    INT_T table_idx = 0;
    INT_T table_count = ( sizeof(s_dp_table) / sizeof(s_dp_table[0]) );
    INT_T index = 0;
    printf("recv dpid: count=[%d]\r\n", cnt);
    // 倒序处理，保证最后发出启动指令
    for(index = cnt-1; index >= 0; index--)
    {
        TY_OBJ_DP_S *p_dp_obj = dp_data + index;
        TuyaHandleDPCmd(p_dp_obj);
        for(table_idx = 0; table_idx < table_count; table_idx++)
        {
            if(s_dp_table[table_idx].dp_id == p_dp_obj->dpid)
            {
                s_dp_table[table_idx].handler(p_dp_obj);
                break;
            }
        }
    }
}

extern IPC_MGR_INFO_S s_mgr_info;

/* OTA */
//Callback after downloading OTA files
VOID __IPC_APP_upgrade_notify_cb(IN CONST FW_UG_S *fw, IN CONST INT_T download_result, IN PVOID_T pri_data)
{
    FILE *p_upgrade_fd = (FILE *)pri_data;
    fclose(p_upgrade_fd);

    PR_DEBUG("Upgrade Finish");
    PR_DEBUG("download_result:%d fw_url:%s", download_result, fw->fw_url);
    if(download_result == 0)
    {
        system("./upgrade.sh > ../upgrade.log");
    }
}

//To collect OTA files in fragments and write them to local files
OPERATE_RET __IPC_APP_get_file_data_cb(IN CONST FW_UG_S *fw, IN CONST UINT_T total_len,IN CONST UINT_T offset,
                             IN CONST BYTE_T *data,IN CONST UINT_T len,OUT UINT_T *remain_len, IN PVOID_T pri_data)
{
    PR_DEBUG("Rev File Data");
    PR_DEBUG("total_len:%d  fw_url:%s", total_len, fw->fw_url);
    PR_DEBUG("Offset:%d Len:%d", offset, len);

    //report UPGRADE process, NOT only download percent, consider flash-write time
    //APP will report overtime fail, if uprgade process is not updated within 60 seconds

    int download_percent = offset * 1.0 / (total_len + 1) * 100;
    int report_percent = download_percent / 2; // as an example, download 100% = 50%  upgrade work finished
    tuya_ipc_upgrade_progress_report(report_percent);

    FILE *p_upgrade_fd = (FILE *)pri_data;
    fwrite(data, 1, len, p_upgrade_fd);

    if (offset == total_len ||
        offset + len == total_len) // finished downloading
    {
      *remain_len = 0;

      // finish 1st part
      report_percent += 10;
      tuya_ipc_upgrade_progress_report(report_percent);
      // finish 2nd part

      // todo
      // sleep(5);

      report_percent += 10;
      tuya_ipc_upgrade_progress_report(report_percent);
      // finish all parts, set to 90% for example
      report_percent = 99;
      tuya_ipc_upgrade_progress_report(report_percent);
    }

    //APP will report "uprage success" after reboot and new FW version is reported inside SDK automaticlly

    return OPRT_OK;
}

INT_T IPC_APP_Upgrade_Inform_cb(IN CONST FW_UG_S *fw)
{
    PR_DEBUG("Rev Upgrade Info");
    PR_DEBUG("fw->fw_url:%s", fw->fw_url);
    //PR_DEBUG("fw->fw_md5:%s", fw->fw_md5);
    PR_DEBUG("fw->sw_ver:%s", fw->sw_ver);
    PR_DEBUG("fw->file_size:%u", fw->file_size);
    PlayVoice(V_START_UPDATE, 0);
    FILE *p_upgrade_fd = fopen(s_mgr_info.upgrade_file_path, "w+b");
    return tuya_ipc_upgrade_sdk(fw, __IPC_APP_get_file_data_cb, __IPC_APP_upgrade_notify_cb, p_upgrade_fd);
}



// simple start

typedef struct
{
	TUYA_IPC_SDK_RUN_VAR_S sdk_run_info;
	pthread_t mqtt_status_change_handle;
}TUYA_IPC_SDK_RUN_HANDLE;

static TUYA_IPC_SDK_RUN_HANDLE s_ipc_sdk_run_handler = {0};


STATIC VOID tuya_ipc_sdk_p2p_init()
{
    TUYA_IPC_TRANSFER_VAR_S p2p_var = {0};
   
    p2p_var.online_cb = NULL;//TODO:此接口似乎对外没有用途
    p2p_var.on_rev_audio_cb = s_ipc_sdk_run_handler.sdk_run_info.p2p_info.rev_audio_cb;
    p2p_var.on_event_cb = s_ipc_sdk_run_handler.sdk_run_info.p2p_info.transfer_event_cb;
    p2p_var.max_client_num = s_ipc_sdk_run_handler.sdk_run_info.p2p_info.max_p2p_client;

#if 0
    //TODO 这些变量是否可以使用 media_info。从 demo 上来看，是可以不同的
    p2p_var.rev_audio_codec = s_ipc_sdk_run_handler.sdk_run_info.media_info.media_info.audio_codec[E_CHANNEL_AUDIO];
    p2p_var.audio_sample =  s_ipc_sdk_run_handler.sdk_run_info.media_info.media_info.audio_sample[E_CHANNEL_AUDIO];
    p2p_var.audio_databits =  s_ipc_sdk_run_handler.sdk_run_info.media_info.media_info.audio_databits[E_CHANNEL_AUDIO];
    p2p_var.audio_channel = s_ipc_sdk_run_handler.sdk_run_info.media_info.media_info.audio_channel[E_CHANNEL_AUDIO];
    p2p_var.defLiveMode = s_ipc_sdk_run_handler.sdk_run_info.p2p_info.live_mode;
    memcpy(&p2p_var.AVInfo,&s_ipc_sdk_run_handler.sdk_run_info.media_info.media_info,sizeof(IPC_MEDIA_INFO_S));
#endif

    #if 1
    //TODO 这些变量是否可以使用 media_info。从 demo 上来看，是可以不同的
    p2p_var.rev_audio_codec = TUYA_CODEC_VIDEO_H264;
    p2p_var.audio_sample =  TUYA_AUDIO_SAMPLE_8K;
    p2p_var.audio_databits =  TUYA_AUDIO_DATABITS_16;
    p2p_var.audio_channel = TUYA_AUDIO_CHANNEL_MONO;
    p2p_var.defLiveMode = TRANS_DEFAULT_STANDARD;
        extern IPC_MEDIA_INFO_S s_media_info;
    memcpy(&p2p_var.AVInfo,&s_media_info,sizeof(IPC_MEDIA_INFO_S));
#endif
    

    int ret = tuya_ipc_tranfser_init(&p2p_var);
    PR_DEBUG("p2p transfer init result is %d\n",ret);

}


BOOL_T s_mqtt_online_status = FALSE;
STATIC VOID tuya_ipc_sdk_net_status_change_cb(IN CONST BYTE_T stat)
{

    PR_DEBUG("Net status change to:%d", stat);
    switch(stat)
    {
#if defined(WIFI_GW) && (WIFI_GW==1)
        case STAT_CLOUD_CONN:        //for wifi ipc
        PR_DEBUG("WIFI_GW is opend:%d", stat);
#endif
#if defined(WIFI_GW) && (WIFI_GW==0)
        case GB_STAT_CLOUD_CONN:     //for wired ipc
#endif
          //  break; CI 上：上线是 7.ipc 本地是 11。所以要注释掉 break
        case STAT_MQTT_ONLINE:{
            PR_DEBUG("mqtt is online %d\r\n",stat);
            if(s_mqtt_online_status == FALSE){
                s_mqtt_online_status = TRUE;
            }
            break;
        }
        default:{
            break;
        }
    }
    //TODO 网络上线后，需要用户去做一些设备声音和灯光处理。可能存在服务还未开启的情况
    if(s_ipc_sdk_run_handler.sdk_run_info.net_info.net_status_change_cb){
    	s_ipc_sdk_run_handler.sdk_run_info.net_info.net_status_change_cb(stat);
    }

}


//DOTO:线程处理完，则退出了。如果是设备离线，再上线，开启过的功能，是否需要重新上线。线程是否要退出？
STATIC VOID* tuya_ipc_sdk_mqtt_online_proc(PVOID_T arg)
{
    PR_DEBUG("tuya_ipc_sdk_mqtt_online_proc thread start success\n");
    while(s_mqtt_online_status == FALSE) {
        sleep(1);
    }
    PR_DEBUG("tuya_ipc_sdk_mqtt_online_proc is start run\n");
    int ret;
    //同步服务器时间
    TIME_T time_utc;
    INT_T time_zone;
    do{
        ret = tuya_ipc_get_service_time_force(&time_utc, &time_zone);
    } while(ret != OPRT_OK);

    char cmd[256] = {0};
    // 设置系统时间
    printf("time_utc:%ld, time_zone:%d\n", time_utc, time_zone);
    snprintf(cmd, 256, "date -s @%ld", time_utc + time_zone);
    system(cmd);
    // 设置系统时区


    char tz_cmd[64];
    snprintf(tz_cmd, sizeof(tz_cmd), "timedatectl set-timezone Etc/GMT%+d", -time_zone);
    system(tz_cmd);


    tuya_ipc_sdk_p2p_init();            // 初始化 p2p
//    IPC_APP_upload_all_status();        // 上传支持的 dp 点信息
    tuya_ipc_upload_skills();           // 上传设备能力
    PR_DEBUG("tuya_ipc_sdk_mqtt_online_proc is end run\n");

    return NULL;
}


OPERATE_RET tuya_ipc_sdk_start(IN CONST TUYA_IPC_SDK_RUN_VAR_S * pRunInfo)
{
    printf("sdk start\n");
	if(NULL == pRunInfo){
		printf("start sdk para is NULL\n");
		return OPRT_INVALID_PARM;
	}

    OPERATE_RET ret = 0;
    STATIC BOOL_T s_ipc_sdk_started = FALSE;
    if(TRUE == s_ipc_sdk_started ) {
        printf("IPC SDK has started\n");
        return ret;
    }

	s_ipc_sdk_run_handler.sdk_run_info = *pRunInfo;
    tuya_ipc_set_log_attr(pRunInfo->debug_info.log_level,NULL);
    
    //setup:创建等待 mqtt 上线进程，mqtt 上线后，再开启与网络相关的业务
    int op_ret = pthread_create(&s_ipc_sdk_run_handler.mqtt_status_change_handle, NULL, tuya_ipc_sdk_mqtt_online_proc, NULL);
    if(op_ret < 0){
        printf("create tuya_ipc_sdk_mqtt_online_proc  thread is error\n");
        return -1;
    }

	//setup2:init sdk
    TUYA_IPC_ENV_VAR_S env;
    memset(&env, 0, sizeof(TUYA_IPC_ENV_VAR_S));
    strcpy(env.storage_path, pRunInfo->iot_info.cfg_storage_path);
    strcpy(env.product_key,pRunInfo->iot_info.product_key);
    strcpy(env.uuid, pRunInfo->iot_info.uuid);
    strcpy(env.auth_key, pRunInfo->iot_info.auth_key);
    strcpy(env.dev_sw_version, pRunInfo->iot_info.dev_sw_version);
    strcpy(env.dev_serial_num, "tuya_ipc");
    env.status_changed_cb = tuya_ipc_sdk_net_status_change_cb;
    
    env.dev_obj_dp_cb = pRunInfo->dp_info.common_dp_cmd_proc;
    env.dev_raw_dp_cb = pRunInfo->dp_info.raw_dp_cmd_proc;
    env.dev_dp_query_cb = pRunInfo->dp_info.dp_query;
    env.upgrade_cb_info.upgrade_cb = pRunInfo->upgrade_info.upgrade_cb;
    env.gw_rst_cb = pRunInfo->iot_info.gw_reset_cb;
    env.gw_restart_cb = pRunInfo->iot_info.gw_restart_cb;

    // 初始化 SDK
    ret = tuya_ipc_init_sdk(&env);
	if(OPRT_OK != ret){
		printf("init sdk is error\n");
		return ret;
	}

	//ring buffer 创建
	IPC_APP_Set_Media_Info();
	ret = TUYA_APP_Init_Ring_Buffer();
	if(OPRT_OK != ret){
		printf("create ring buffer is error\n");
		return ret;
	}

    // 启动 SDK
	ret = tuya_ipc_start_sdk(WIFI_GWCM_OLD, pRunInfo->net_info.connect_mode, pRunInfo->debug_info.qrcode_token);
	if(OPRT_OK != ret){
		printf("start sdk is error\n");
		return ret;
	}

    // 初始化扫地机 SDK
    ret = tuya_sweeper_init_sdk();
    if(OPRT_OK != ret){
		printf("tuya_sweeper_init_sdk failed\n");
		return ret;
	}

    s_ipc_sdk_started = true;
	printf("tuya ipc sdk start is complete\n");       
	return ret;
}


