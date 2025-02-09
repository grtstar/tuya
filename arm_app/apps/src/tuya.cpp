#include <lcm/lcm-cpp.hpp>
#include <fstream>


#include "utils/log_.h"
#include "utils/stacktrace.hpp"
#include "utils/shell.hpp"

#include "mars_message/DevicePower.hpp"
#include "mars_message/Debug.hpp"
#include "mars_message/GridMap.hpp"
#include "mars_message/AppPath.hpp"
#include "mars_message/RobotStatus.hpp"
#include "mars_message/KeyValue.hpp"

using namespace mars_message;

#include "tuya_cloud_base_defs.h"
#include "tuya_iot_config.h"

#include "tuya_ipc_media_demo.h"
#include "tuya_ipc_sweeper_demo.h"
#include "tuya.h"
#include "tuya_robot.h"
#include "tuya_utils.h"
#include "voice.h"

#if defined(TY_BT_MOD) && TY_BT_MOD == 1
#include "tuya_os_adapt_bt.h"
#endif


#define TAG "MAN"

#define PRODUCT_ID          "cnyiygalqpueeiwf"
#define DEVICE_UUID         "uuid3658996f48293db1"
#define PRODUCT_AUTHKEY     "BG15bQZz92o2iPUR7X1LDJYLXOlGpLwe"

#define APP_VERSION         "1.0.0"
#define APP_STORAGE_PATH    "./tuya/"
#define APP_DEFAULT_KEY     "./tuya/tuya.key"
#define APP_UPGRADE_PATH    "../version/"
#define APP_UPGRADE_FILE    "../version/upgrade.zip"
#define APP_VERSION_FILE    "../version/soft_version.json"

extern "C" int tuya_wifi_init(void);


IPC_MGR_INFO_S s_mgr_info = {0};
CHAR_T s_raw_path[128] = {0};

STATIC VOID __IPC_APP_Get_Net_Status_cb(IN CONST BYTE_T stat)
{
    LOGD(TAG, "Net status change to:{}", stat);
    switch(stat)
    {
#if defined(WIFI_GW) && (WIFI_GW==1)
        case STAT_CLOUD_CONN:        //for wifi ipc
            break;
        case STAT_MQTT_ONLINE:       //for low-power wifi ipc
#endif
#if defined(WIFI_GW) && (WIFI_GW==0)
        case GB_STAT_CLOUD_CONN:     //for wired ipc
#endif
        {
            IPC_APP_Notify_LED_Sound_Status_CB(IPC_MQTT_ONLINE);
            PlayVoice(V_SERVER_CONNECTED, PLAY_QUEUE);
            tuya_message::Request req;
            tuya_message::Result res = {};
            TuyaComm::Get()->Send("ty_wifi_connect", &req, &res);
            TuyaComm::Get()->ReportAll();
            //strcpy(NULL, "SigFault Test");
            break;
        }
        case STAT_STA_DISC:
            break;
        case STAT_OFFLINE:
            break;
        default:
        {
            break;
        }
    }
}

/*certification information(essential)*/
std::string id = PRODUCT_ID;
std::string uuid = DEVICE_UUID;
std::string authKey = PRODUCT_AUTHKEY;

//app version
std::string soft_version;
std::string MCU_version;
std::string system_version;

OPERATE_RET TUYA_IPC_SDK_START(WIFI_INIT_MODE_E connect_mode, CHAR_T *p_token)
{
    printf("SDK Version:%s\r\n", tuya_ipc_get_sdk_info());
	TUYA_IPC_SDK_RUN_VAR_S ipc_sdk_run_var ={0};
	memset(&ipc_sdk_run_var,0,sizeof(ipc_sdk_run_var));
    
    TuyaLoadKey(APP_DEFAULT_KEY, id, uuid, authKey);
    std::ofstream uuid_file("/tmp/uuid.txt");
    if (uuid_file.is_open()) {
        uuid_file << uuid;
        uuid_file.close();
    } else {
        LOGD(TAG, "Unable to open file to write UUID");
    }
    SoftVersion(APP_VERSION_FILE, soft_version, MCU_version, system_version);
   
#if 0
    //对比系统版本号
    //system_release_version 获取系统版本格式：c1_rk3308_system_0.8.9.0.release_B.e4e9b21c
    std::string res = shell::valueof(std::string("system_release_version"));
    LOGD(TAG, "system_release_version:{}", res);
    auto start = res.find("system");
    auto end = res.find("release");
    if (start != std::string::npos && end != std::string::npos)
    {      
        std::string version = res.substr(start + 7, end - 1 - start - 7);
        LOGD(TAG, "current system version: {}", version);
        if (CompareVersion(version, system_version) == 1 && !MCU_version.empty())
        {
            UpdateSoftVersion(APP_VERSION_FILE, soft_version, MCU_version, version);
            PlayVoice(V_UPDATE_SUCC, 0);
        }
        else
        {   
            LOGD(TAG, "current system version {} <= last system version {} , no need to upgrade system", version, system_version);
        }
    }
#endif
    strcpy(ipc_sdk_run_var.iot_info.product_key, id.c_str());
    strcpy(ipc_sdk_run_var.iot_info.uuid, uuid.c_str());
    strcpy(ipc_sdk_run_var.iot_info.auth_key, authKey.c_str());
    strcpy(ipc_sdk_run_var.iot_info.dev_sw_version, soft_version.c_str());
    strcpy(ipc_sdk_run_var.iot_info.cfg_storage_path, APP_STORAGE_PATH);
    ipc_sdk_run_var.iot_info.gw_reset_cb = IPC_APP_Reset_System_CB;
	ipc_sdk_run_var.iot_info.gw_restart_cb = IPC_APP_Restart_Process_CB;

	/*connect mode (essential)*/
	ipc_sdk_run_var.net_info.connect_mode = connect_mode;
	ipc_sdk_run_var.net_info.net_status_change_cb = __IPC_APP_Get_Net_Status_cb;
	if(p_token){
	    strcpy(ipc_sdk_run_var.debug_info.qrcode_token,p_token);
	}

    /*p2p function (essential)*/
	ipc_sdk_run_var.p2p_info.max_p2p_client = 5;
	ipc_sdk_run_var.p2p_info.live_mode = TRANS_DEFAULT_STANDARD;
	ipc_sdk_run_var.p2p_info.transfer_event_cb = __TUYA_APP_p2p_event_cb;
	ipc_sdk_run_var.p2p_info.rev_audio_cb = __TUYA_APP_rev_audio_cb;

	/*dp function(essential)*/
	ipc_sdk_run_var.dp_info.dp_query = IPC_APP_handle_dp_query_objs;
	ipc_sdk_run_var.dp_info.raw_dp_cmd_proc = IPC_APP_handle_raw_dp_cmd_objs;
	ipc_sdk_run_var.dp_info.common_dp_cmd_proc = IPC_APP_handle_dp_cmd_objs;

	/*upgrade function(essential)*/
    strcpy(s_mgr_info.upgrade_file_path, APP_UPGRADE_FILE);
    MkdirFolder(APP_UPGRADE_PATH);
    ipc_sdk_run_var.upgrade_info.enable = true;
	ipc_sdk_run_var.upgrade_info.upgrade_cb = IPC_APP_Upgrade_Inform_cb;
    

    // 设置打印级别
    ipc_sdk_run_var.debug_info.log_level = 4;
        

	OPERATE_RET ret = tuya_ipc_sdk_start(&ipc_sdk_run_var); 
    if(ret !=0 ){
    	printf("ipc sdk v5 start fail,please check run parameter, ret=%d\n",ret);
    }
    else
    {
        printf("ipc sdk v5 start success\n");
    }
	return ret;
}

class TestHandler
{
public:
    void OnGet(const lcm::ReceiveBuffer *rbuf, const std::string &channel, const mars_message::KeyValue *msg)
    {
        KeyValue ret;
        ret.value = msg->value;
        switch(msg->value)
        {
            case 'PID':
            ret.key = id;
            break;
            case 'CVER':
            ret.key = soft_version;
            break;
            case 'MAC':
            ret.key = shell::valueof("ifconfig eth0 | grep HWaddr | awk '{print $5}'");
            break;
        }
        TuyaComm::Get()->Publish("Mgetr", &ret);
    }

    void OnSet(const lcm::ReceiveBuffer *rbuf, const std::string &channel, const mars_message::KeyValue *msg)
    {
        KeyValue ret;
        ret.value = msg->value;
        ret.key = "0";
        switch(msg->value)
        {
            case 'PID':
            // todo: 改变 pid
            ret.key = "1";
            break;
            case 'CVER':
            break;
            case 'MAC':
            break;
        }
        //TuyaComm::Get()->Publish("Msetr", &ret);
    }
};

int main(int argc, char ** argv)
{
    LOGVERSION("v1.0.14");
    //StackTrace stackTrack;
    
    lcm::LCM lcm;
    if(!lcm.good())
    {
        return -1;
    }  
    TuyaComm::Get()->SetLcm(&lcm);
    TestHandler testHandler;
    TuyaComm::Get()->Subscribe("Mget", &TestHandler::OnGet, &testHandler);
    TuyaComm::Get()->Subscribe("Mset", &TestHandler::OnSet, &testHandler);

#if defined(TY_BT_MOD) && TY_BT_MOD == 1
    tuya_bt_init(); xx
#endif
    tuya_wifi_init();
    
    TUYA_IPC_SDK_START(WIFI_INIT_AP, NULL);

    pthread_t sweeper_detect_thread;
    pthread_create(&sweeper_detect_thread, NULL, thread_album_send, NULL);
    pthread_detach(sweeper_detect_thread);

	pthread_t h264_output_thread;
    pthread_create(&h264_output_thread, NULL, thread_live_video, NULL);
    pthread_detach(h264_output_thread);

    pthread_t pcm_output_thread;
    pthread_create(&pcm_output_thread, NULL, thread_live_audio, NULL);
    pthread_detach(pcm_output_thread);

    TuyaComm::Get()->HandleForever();
   
    return 0;
}

#include "voice.h"
#include "mars_message/Event.hpp"
void PlayVoice(int v, int param)
{
    Event voice;
    voice.event = v;
    voice.param = param;
    TuyaComm::Get()->Publish("Voice", &voice);
}