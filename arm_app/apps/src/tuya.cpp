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



#include "tuya_robot.h"
#include "tuya_utils.h"
#include "voice.h"

#include "tuya_album_sweeper_api.h"
#include "tuya_error_code.h"
#include "tuya_iot_com_api.h"
#include "tuya_iot_config.h"
#include "tuya_iot_wifi_api.h"
// #include "tuya_sdk_common.h"
#include "tuya_wifi_netcfg.h"
#include "tuyaos_sweeper_api.h"
#include "utilities/uni_log.h"
#include "tuya_robot_sdk_init.h"
#include "tuya_bt.h"
#if defined(TY_ROBOT_MEDIA_ENABLE) && (TY_ROBOT_MEDIA_ENABLE == 1)
#include "tuya_ipc_cloud_storage.h"
#include "tuya_ipc_stream_storage.h"
#include "tuya_sdk_media_demo.h"
#include "tuya_sdk_sd_card_demo.h"
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
/**
 * @brief  iot sdk 初始化
 * @param  [in] GW_WF_START_MODE 配网模式
 * @param  [in] p_token 配网 token
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
static OPERATE_RET ty_iot_sdk_init(GW_WF_START_MODE connect_mode, CHAR_T* p_token)
{
    OPERATE_RET rt = OPRT_OK;
    STATIC BOOL_T s_ty_iot_sdk_started = FALSE;

    if (TRUE == s_ty_iot_sdk_started) { //这里只是保护 SDK 不再次初始化
        PR_DEBUG("iot sdk has started");
        return rt;
    }
    PR_DEBUG("SDK Version:%s", tuya_iot_get_sdk_info()); //获取 sdk 版本
    PR_DEBUG("system reset reason:[%d]", tal_system_get_reset_reason(NULL)); //测试 demo 中实现不了系统重启原因的打印，开发者自己去实现系统重启原因

    tuya_robot_event_init(); //机器人相关事件初始化
    /*以下准备文件系统读写存储路径，真实的设备需要填写设备本地路径*/
    TY_INIT_PARAMS_S init_param = { 0 };
    init_param.init_db = TRUE;
    strcpy(init_param.sys_env, TARGET_PLATFORM);
    strcpy(init_param.log_seq_path, "/tmp/tuya"); // SDK 在线日志保存的路径
    TUYA_CALL_ERR_LOG(tuya_iot_init_params("tuya", &init_param)); // SDK db 文件存储路径
    /*以上准备文件系统读写存储路径，真实的设备需要填写设备本地路径*/
#if defined(TY_BT_MOD) && TY_BT_MOD == 1
    tuya_set_bt_device_name("RVC");   //如果需要修改蓝牙广播名称，可以在这里做，注意支持的名称最大长度是 5 字节
#endif
    //设置日志等级
    tuya_iot_set_log_attr(TY_LOG_LEVEL_DEBUG); // 设置打印级别，调试时设置 TY_LOG_LEVEL_DEBUG 等级，正常版本可以设置 TY_LOG_LEVEL_NOTICE
    /* Initialize TuyaOS product information */
    TY_IOT_CBS_S iot_cbs = { 0 };
    iot_cbs.gw_status_cb = ty_sdk_dev_status_changed_cb; //设备激活状态
    iot_cbs.gw_ug_cb = ty_user_upgrade_inform_cb; //注册 OTA 升级接口
    iot_cbs.pre_gw_ug_cb = ty_dev_upgrade_pre_check_cb; //注册 OTA 升级检查接口
    iot_cbs.gw_reset_cb = ty_sdk_app_reset_cb; // APP 移除配网回调
    iot_cbs.dev_obj_dp_cb = ty_cmd_handle_dp_cmd_objs; //注册 obj dp 接收通道接口
    iot_cbs.dev_raw_dp_cb = ty_cmd_handle_dp_raw_objs; //注册 raw dp 接收通道接口
    iot_cbs.dev_dp_query_cb = ty_cmd_handle_dp_query_objs; //注册查询 dp 接口

    /*以下代码是双固件（主固件+MCU 固件）的情况下使用*/
    GW_ATTACH_ATTR_T arrt;
    arrt.tp = GP_DEV_MCU; // MCU 通道固定为 9
    strcpy(arrt.ver, TY_MCU_VERSION);
    /*以上代码是双固件（主固件+MCU 固件）的情况下使用*/
    WF_GW_PROD_INFO_S prod_info = { s_ty_uuid, s_ty_authkey, NULL, NULL };
    TUYA_CALL_ERR_RETURN(tuya_iot_set_wf_gw_prod_info(&prod_info)); //获取 uuid 和 authkey 传给 SDK，注意：开发者在内存中获取 prod_info 参数做好校验，要真实且有效
    //扫地机 wifi 配置选择 GWCM_OLD_PROD；传入的通道为 DEV_NM_ATH_SNGL，不要改变。
    TUYA_CALL_ERR_RETURN(tuya_iot_wf_dev_init(GWCM_OLD_PROD, connect_mode, &iot_cbs, NULL, s_ty_pid, TY_APP_VERSION, DEV_NM_ATH_SNGL, &arrt, 1));
    // TUYA_CALL_ERR_RETURN(tuya_iot_wf_soc_dev_init(GWCM_OLD_PROD, connect_mode, &iot_cbs, s_ty_pid, TY_APP_VERSION));
    //注意：开发者如有双固件的需求，请使用 tuya_iot_wf_dev_init 接口；开发者如只需要单固件的需求，请使用 tuya_iot_wf_soc_dev_init 接口
    TUYA_CALL_ERR_RETURN(tuya_iot_reg_get_wf_nw_stat_cb(ty_sdk_net_status_change_cb)); // wifi 状态回调
    tuya_wifi_user_cfg("tuya", "tuya", p_token); //测试使用直接填入 p_token，实际开发不需要该接口，SDK 会处理

    s_ty_iot_sdk_started = true;
    PR_DEBUG("tuya iot sdk start is complete");
    return rt;
}

/**
 * @brief  robot media sdk 初始化
 * @param  [*]
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
static OPERATE_RET ty_robot_media_sdk_init(void)
{
    OPERATE_RET ret = OPRT_OK;
#if defined(TY_ROBOT_MEDIA_ENABLE) && (TY_ROBOT_MEDIA_ENABLE == 1)
    //适配多媒体接口
    TUYA_SDK_MEDIA_ADAPTER_S p_media_adatper_info = { 0 }; //音视频接口回调
    DEVICE_SDK_MEDIA_INFO_T p_media_infos = { 0 }; //音视频编解码参数
    /* 媒体信息（必需）*/
    /* 主码流（高清），视频配置*/
    /* 注意：首先：如果主码流支持多个视频流配置，将每个项目设置为允许配置的上限。
       其次：E_IPC_STREAM_VIDEO_MAIN 必须存在。这是 SDK 的数据来源。
    */
    p_media_infos.av_encode_info.stream_enable[E_IPC_STREAM_VIDEO_MAIN] = TRUE; /* 是否启用本地高清录像流 */
    p_media_infos.av_encode_info.video_fps[E_IPC_STREAM_VIDEO_MAIN] = 30; /* FPS */ //一秒钟内连续播放的 30 帧
    p_media_infos.av_encode_info.video_gop[E_IPC_STREAM_VIDEO_MAIN] = 30; /* GOP */ //关键帧，该值的大小影响视频解密的效率及质量
    p_media_infos.av_encode_info.video_bitrate[E_IPC_STREAM_VIDEO_MAIN] = TUYA_VIDEO_BITRATE_1M; /* 传输速率 */
    p_media_infos.av_encode_info.video_width[E_IPC_STREAM_VIDEO_MAIN] = 640; /* 单帧分辨率的宽度*/
    p_media_infos.av_encode_info.video_height[E_IPC_STREAM_VIDEO_MAIN] = 360; /* 单帧分辨率的高度 */
    p_media_infos.av_encode_info.video_freq[E_IPC_STREAM_VIDEO_MAIN] = 90000; /* 摄像头的时钟频率 */
    p_media_infos.av_encode_info.video_codec[E_IPC_STREAM_VIDEO_MAIN] = TUYA_CODEC_VIDEO_H264; /* 编码方式 */

    /*
    子流 (标清)，视频配置。请注意，如果子流支持多个视频流配置，请将每个项设置为允许配置的上限。
    */
    p_media_infos.av_encode_info.stream_enable[E_IPC_STREAM_VIDEO_SUB] = TRUE; /* 是否启用本地标清录像流 */
    p_media_infos.av_encode_info.video_fps[E_IPC_STREAM_VIDEO_SUB] = 30; /* FPS */ //一秒钟内连续播放的 30 帧
    p_media_infos.av_encode_info.video_gop[E_IPC_STREAM_VIDEO_SUB] = 30; /* GOP */ //关键帧，该值的大小影响视频解密的效率及质量
    p_media_infos.av_encode_info.video_bitrate[E_IPC_STREAM_VIDEO_SUB] = TUYA_VIDEO_BITRATE_512K; /* 传输速率 */
    p_media_infos.av_encode_info.video_width[E_IPC_STREAM_VIDEO_SUB] = 640; /* 单帧分辨率的宽度 */
    p_media_infos.av_encode_info.video_height[E_IPC_STREAM_VIDEO_SUB] = 360; /* 单帧分辨率的高度 */
    p_media_infos.av_encode_info.video_freq[E_IPC_STREAM_VIDEO_SUB] = 90000; /* 摄像头的时钟频率 */
    p_media_infos.av_encode_info.video_codec[E_IPC_STREAM_VIDEO_SUB] = TUYA_CODEC_VIDEO_H264; /* 编码方式 */

    /*
    音频流编解码配置。
    注:SDK 的内部 P2P 预览、云存储、本地存储均使用 E_IPC_STREAM_AUDIO_MAIN 数据。
    */
    p_media_infos.av_encode_info.stream_enable[E_IPC_STREAM_AUDIO_MAIN] = TRUE; /* 是否启用本地声音采集 */
    p_media_infos.av_encode_info.audio_codec[E_IPC_STREAM_AUDIO_MAIN] = TUYA_CODEC_AUDIO_PCM; /* 编码方式 */
    p_media_infos.av_encode_info.audio_sample[E_IPC_STREAM_AUDIO_MAIN] = TUYA_AUDIO_SAMPLE_8K; /* 音频采样率 */
    p_media_infos.av_encode_info.audio_databits[E_IPC_STREAM_AUDIO_MAIN] = TUYA_AUDIO_DATABITS_16; /* 音频位宽 */
    p_media_infos.av_encode_info.audio_channel[E_IPC_STREAM_AUDIO_MAIN] = TUYA_AUDIO_CHANNEL_MONO; /* 音频通道 */
    p_media_infos.av_encode_info.audio_fps[E_IPC_STREAM_AUDIO_MAIN] = 25; /* 一秒钟内连续播放的 25 帧 */

    p_media_infos.audio_decode_info.enable = TRUE;
    p_media_infos.audio_decode_info.audio_codec = TUYA_CODEC_AUDIO_G711U; /*解码方式*/
    p_media_infos.audio_decode_info.audio_sample = TUYA_AUDIO_SAMPLE_8K; /* 音频采样率 */
    p_media_infos.audio_decode_info.audio_databits = TUYA_AUDIO_DATABITS_16; /* 音频位宽 */
    p_media_infos.audio_decode_info.audio_channel = TUYA_AUDIO_CHANNEL_MONO; /* 音频通道 */

    p_media_adatper_info.rev_audio_cb = tuya_sweeper_app_rev_audio_cb; //接收音频参数
    p_media_adatper_info.rev_video_cb = tuya_sweeper_app_rev_video_cb; //接收视频参数
    p_media_adatper_info.rev_file_cb = tuya_sweeper_app_rev_file_cb; // APP 下载文件到设备端
    p_media_adatper_info.get_snapshot_cb = tuya_sweeper_app_get_snapshot_cb; //音视频数据通过该接口上传，有 SDK 实现本地或者云端的存储
    p_media_adatper_info.max_stream_client = TY_SDK_P2P_NUM_MAX; //默认最大支持 5 路

    tuya_sdk_media_adapter_init(&p_media_adatper_info, &p_media_infos); //带音视频扫地机，初始化媒体抽象层

    ret = tuya_media_ring_buffer_init(&p_media_infos, 0, 0); //音视频 ring buffer 初始化
    if (OPRT_OK != ret) {
        PR_ERR("create ring buffer is error\n");
        return ret;
    }
    ret = tuya_media_av_event_init(tuya_sweeper_av_event_cb); //注册音视频上报 P2P 事件
    if (OPRT_OK != ret) {
        PR_ERR("tuya sweeper event failed\n");
        return ret;
    }
#else
    tuya_sdk_media_adapter_init(NULL, NULL); //不带音视频的扫地机，初始化媒体抽象层
#endif
    ret = tuya_media_event_init(tuya_sweeper_event_cb); //注册实时地图及路径上报事件
    if (OPRT_OK != ret) {
        PR_ERR("tuya sweeper event failed\n");
        return ret;
    }
    return ret;
}

#if defined(TY_ROBOT_MEDIA_ENABLE) && (TY_ROBOT_MEDIA_ENABLE == 1)
/**
 * @brief  ty media 本地存储处理
 * @param  [*]
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET ty_media_local_storage(void)
{
#define MAX_EVENT_PER_DAY 500 //最大存储 500 天，用户根据自己的 SD 大小设置
    OPERATE_RET ret = OPRT_OK;
    STATIC BOOL_T s_stream_storage_inited = FALSE;
    char* mount_path = tuya_ipc_get_sd_mount_path(); //获取 sd 卡存储路径

    if (s_stream_storage_inited == TRUE) {
        PR_DEBUG("The Stream Storage Is Already Inited");
        return OPRT_OK;
    }
    TUYA_IPC_STORAGE_VAR_T stg_var;
    memset(&stg_var, 0, sizeof(TUYA_IPC_STORAGE_VAR_T));
    strncpy(stg_var.base_path, TY_APP_STORAGE_PATH, SS_BASE_PATH_LEN); //路径
    strncpy(mount_path, TY_APP_STORAGE_PATH, SS_BASE_PATH_LEN);
    stg_var.max_event_per_day = MAX_EVENT_PER_DAY; //录制天数
    stg_var.sd_status_changed_cb = tuya_robot_sd_status_upload; //状态回调
    stg_var.skills = 0; //默认回放倍速全部支持
    // stg_var.skills = TUYA_IPC_SKILL_BASIC | TUYA_IPC_SKILL_DELETE_BY_DAY | TUYA_IPC_SKILL_SPEED_PLAY_0Point5 | TUYA_IPC_SKILL_SPEED_PLAY_2 | TUYA_IPC_SKILL_SPEED_PLAY_4 | TUYA_IPC_SKILL_SPEED_PLAY_8 ;
    PR_DEBUG("Init Stream_Storage SD:%s", TY_APP_STORAGE_PATH);
    ret = tuya_ipc_ss_init(&stg_var); // sd 卡初始化
    if (ret != OPRT_OK) {
        PR_ERR("Init Main Video Stream_Storage Fail. %d", ret);
        return OPRT_COM_ERROR;
    }
    s_stream_storage_inited = TRUE;
    return OPRT_OK;
}

/**
 * @brief  云端存储初始化
 * @param  [INT_T] en_audio_mode 设置音频开关
 * @param  [INT_T] pre_recode_time  录制时长
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET ty_media_cloud_storage(INT_T en_audio_mode, INT_T pre_recode_time)
{
    OPERATE_RET ret = 0;
    ret = tuya_ipc_cloud_storage_init(); //云存储初始化
    if (ret != OPRT_OK) {
        PR_DEBUG("Cloud Storage Init Err! ret :%d", ret);
        return ret;
    }

    if (en_audio_mode == TRUE) {
        tuya_ipc_cloud_storage_set_audio_stat(en_audio_mode); //设置云端音频录制标识
        PR_DEBUG("Enable audio record");
    }

    if (pre_recode_time >= 0) {
        ret = tuya_ipc_cloud_storage_set_pre_record_time(pre_recode_time); //设置预录时间，默认预录时间：2 秒。
        PR_DEBUG("Set pre-record time to [%d], [%s]", pre_recode_time, ret == OPRT_OK ? "success" : "failure");
    }
    return OPRT_OK;
}
#endif

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

/**
 * @brief  扫地机 SDK 初始化接口
 * @param  [int] mode
 * @param  [char *] token 该 token 只是测试当中使用，正常代码无效使用
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET ty_sys_start(GW_WF_START_MODE connect_mode, CHAR_T* p_token)
{
    OPERATE_RET ret = OPRT_OK;
    PR_DEBUG("sys start ");

    ret = ty_iot_sdk_init(connect_mode, p_token); // IOT SDK 初始化
    if (OPRT_OK != ret) {
        PR_ERR("tuya iot sdk init failed\n");
        return ret;
    }
#if defined(TY_BT_MOD) && TY_BT_MOD == 1 //能力值打开的情况下，须在 IOT SDK 初始化之后调用
    tuya_ble_set_serv_switch(true);    //蓝牙能力开关
#endif
    ret = ty_robot_media_sdk_init(); //机器人媒体 SDK 初始化
    if (OPRT_OK != ret) {
        PR_ERR("ty robot media sdk init failed\n");
        return ret;
    }
    ret = ty_user_sweeper_set_customize_name(); //用户设置实时地图传输的自定义文件名称，默认支持地图和路径名称，其它名称需要开发者和面板将名称对齐。
    if (OPRT_OK != ret) {
        PR_ERR("ty user set customize name failed\n");
        return ret;
    }
    /***开发者业务上需要初始化的接口可以在此完成***/

    /***开发者业务上需要初始化的接口可以在此完成***/
    return ret;
}

int main(int argc, char ** argv)
{
    LOGVERSION("v1.0.14");
    //StackTrace stackTrack;
    INT_T ret = -1;
    lcm::LCM lcm;
    if(!lcm.good())
    {
        return -1;
    }  
    TuyaComm::Get()->SetLcm(&lcm);
    TestHandler testHandler;
    TuyaComm::Get()->Subscribe("Mget", &TestHandler::OnGet, &testHandler);
    TuyaComm::Get()->Subscribe("Mset", &TestHandler::OnSet, &testHandler);

    ret = ty_sys_start(WF_START_AP_ONLY, "xd");
    if (ret != OPRT_OK) {
        return ret;
    }
    ret = tuya_sdk_mqtt_online_sys_loop(); //设备首次上线处理
    if (ret != OPRT_OK) {
        return ret;
    }
#if defined(TY_ROBOT_MEDIA_ENABLE) && (TY_ROBOT_MEDIA_ENABLE == 1)
    ret = ty_media_local_storage(); //本地存储
    if (ret != OPRT_OK) {
        PR_ERR("tuya media local storage failed");
        return ret;
    }
    //开启音频存储，视频录制时长 2 秒以上
    ret = ty_media_cloud_storage(TRUE, 2); //云端存储
    if (ret != OPRT_OK) {
        PR_ERR("tuya media cloud storage failed");
        return ret;
    }

    ret = tuya_enable_storage_deal(); //音视频存储任务
    if (ret != OPRT_OK) {
        return ret;
    }
#endif
    ret = tuya_media_server_customize_init(TY_SDK_P2P_NUM_MAX); // 初始化媒体流服务
    if (ret != OPRT_OK) {
        PR_ERR("tuya media server init failed");
        return ret;
    }
#if defined(TY_ROBOT_MEDIA_ENABLE) && (TY_ROBOT_MEDIA_ENABLE == 1)
    tuya_av_start(); //开始塞音视频流数据
#endif
    ret = ty_user_sweeper_rt_map_init(); // 实时地图业务初始化
    if (ret != OPRT_OK) {
        PR_ERR("__user_sweeper_p2p_init failed");
        return ret;
    }

    ret = ty_user_sweeper_floor_map_init(); // 楼层地图业务初始化
    if (ret != OPRT_OK) {
        PR_ERR("__user_sweeper_floor_map_init failed");
        return -1;
    }

    ret = robotics_svc_init_voice(); // voice_init 处理初始化
    if (OPRT_OK != ret) {
        PR_ERR("__user_sweeper_voice_init failed\n");
        return ret;
    }
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