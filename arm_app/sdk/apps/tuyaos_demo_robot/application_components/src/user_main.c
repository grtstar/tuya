/*********************************************************************************
 *Copyright(C),2015-2020, TUYA www.tuya.comm
 *FileName:    user_main
 *brief：1.The initialization process of the laser vacuum cleaner includes: 
         2.synchronization with the cloud time, real-time map and path reporting, 
           reporting of floor maps and cleaning records, and other functional 
           demonstrations. 
         3.enable the TY_ROBOT_MEDIA_ENABLE macro definition to support audio and 
           video functionality and demonstrations.
 **********************************************************************************/
#include <libgen.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>
#include <unistd.h>
#include "utilities/uni_log.h"

#include "tuya_iot_config.h"
#include "tuya_sdk_common.h"
#include "tuya_sdk_net_sync_demo.h"
#include "tuya_sdk_floor_map_demo.h"
#include "tuya_sdk_rt_map_demo.h"
#include "tuya_sdk_simple_start.h"
#include "tuya_sdk_voice_demo.h"
#if defined(TY_ROBOT_MEDIA_ENABLE) && (TY_ROBOT_MEDIA_ENABLE == 1)
#include "tuya_sdk_storage_demo.h"
#include "tuya_sdk_av_demo.h"
#endif

CHAR_T s_raw_path[128] = { 0 }; //文件路径保存缓存
/**
 * @brief  应用程序主入口
 * @param  [int] argc
 * @param  [char] *argv
 * @return OPERATE_RET
 */
int main(int argc, char** argv)
{
    INT_T ret = -1;
    CHAR_T s_token[30] = { 0 };
    CHAR_T test_input[64] = { 0 };    //测试用，手动输入
    GW_WF_START_MODE mode = WF_START_AP_ONLY; //开发者使用AP配网模式，
    /*********以下代码只是模拟文件路径测试使用，正常设备端有真实存储的路径，不需要该代码*********/
    while ((ret = getopt(argc, argv, "?m:t:s:r:c:h")) != -1) {
        switch (ret) {
        case 'm':
            mode = atoi(optarg); //这里为了测试会手动输入
            break;
        case 't':
            strcpy(s_token, optarg); //手动输入token
            break;
        case 'r':
            strcpy(s_raw_path, optarg); //手动输入路径
            break;
        default:
            return -1;
        }
    }
    /*********以上代码只是模拟文件路径测试使用，正常设备端有真实存储的路径，不需要该代码*********/
    ret = ty_sys_start(mode, s_token);  //设备SDK相关接口初始化
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
    //开启音频存储，视频录制时长2秒以上
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

    ret = robotics_svc_init_voice(); // voice_init处理初始化
    if (OPRT_OK != ret) {
        PR_ERR("__user_sweeper_voice_init failed\n");
        return ret;
    }
    while (1) {
        static uint16_t sys_loop_cnt = 0;
        sys_loop_cnt++;

        if (sys_loop_cnt >= 100) { // 10s
            sys_loop_cnt = 0;
            //开发者可以在这里做些内存检查的事情，按照自身需求来
            PR_DEBUG("sys loop run");
        }
        /**
        楼层地图上传
        1. demo中输入 floor_map
        2. 看到打印提示后输入 1
        楼层地图更新
        1. demo中输入 floor_map
        2. 看到打印提示后输入 2
        楼层地图删除
        1. demo中输入 floor_map
        2. 看到打印提示后输入 3
        楼层地图下载
        1. demo中输入 floor_map
        2. 看到打印提示后输入 4
        楼层地图获取所有地图信息
        1. demo中输入 floor_map
        2. 看到打印提示后输入 5
        楼层地图清扫记录上报
        1. demo中输入 floor_map
        2. 看到打印提示后输入 6
        注意点：开发者在调用楼层地图接口，要建个独立线程去处理，因为楼层地图的接口都是同步执行，执行的时候会堵塞应用线程情况。
        */
        scanf("%s", test_input); //这里只是测试用，开发者可根据业务需求来实现
        if (0 == strcmp(test_input, "floor_map")) { // 楼层地图上报测试
            multi_map_oper();
        }

        /*
        语音下载测试
        1. demo中输入 download_demo
        */
        else if (0 == strcmp(test_input, "download_demo")) { //语音通过http下载测试
            voice_download_oper();
        }
    #if defined(TY_ROBOT_MEDIA_ENABLE) && (TY_ROBOT_MEDIA_ENABLE == 1)
        /**
        存储功能测试
        开启存储
        1. demo中输入 storage_enable
        2. 看到打印提示后输入 1
        关闭存储
        1. demo中输入 storage_enable
        2. 看到打印提示后输入 0
        **/
        else if (0 == strcmp(test_input, "storage_enable")) {//存储测试 
            tuya_storage_enable_oper();
        }
    #endif
        usleep(100 * 1000);
    }

    return 0;
}
