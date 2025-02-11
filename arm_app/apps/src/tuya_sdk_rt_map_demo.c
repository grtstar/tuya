/*********************************************************************************
 *Copyright(C),2015-2020, TUYA www.tuya.comm
 *FileName:    tuya_sdk_rt_map_demo
 *brief：1.This article introduces the implementation of real-time map and path
           reporting features based on the Ubuntu environment.
         2.It provides fixed support for interfaces that report maps and paths,
           allowing developers to focus solely on two modes file download and
           cancel download.
         3.You can customize the filenames for real-time reporting,
           such as for AI object recognition, and you can adjust the reporting
           frequency of maps and paths according to your needs.
         4.You can build your application development based on the following demo.
 **********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <uni_thread.h>
#include <unistd.h>

#include "mem_pool.h"
#include "tuya_album_sweeper_api.h"
#include "tuya_hal_fs.h"
#include "tuya_hal_semaphore.h"
#include "tuya_sdk_common.h"
#include "tuya_sdk_rt_map_demo.h"
#include "tuyaos_sweeper_api.h"
#include "uni_time.h"

#define TUYA_SWEEPER_ALBUM_FILE_NUM_MAX 2 //固定上报的最大文件数
//自定义文件名上报的最大文件数，默认关闭，如需使用到该功能，就打开红定义，目前可支持最大数量为 48 个自定义文件名
//#define TUYA_SWEEPER_ALBUM_CUSTOMIZE_FILE_NUM_MAX   2
#define RT_MAP_TRANS_SLEEP_TIME 2000 /*默认每隔 2000mS 上报一次路径数据。              
这个上报周期，客户可以根据地图大小和网络情况去调整上报频率；                          
按照涂鸦扫地机协议地图数据格式上到涂鸦智能 APP 上，以 4K 数据到面板上解析显示的为列， 
面板绘图耗时在 500ms，加上面板在通道上获取数据的耗时，合理的上报频率在 1000ms 以上。    
在弱网下，发送数据容易卡住，最大卡住周期时 30S 的 RTC 超时时间。*/
#define RT_MAP_TRANS_TIMEOUT 3000 // 3s    //实时地图发送接口超时时间
#define RT_MAP_SEND_TIMEOUT_CNT 10 // 实时地图发送数据超时最大次数，大概 20S 时间

/**
 * @brief  扫地机实时地图下载的状态
 */
typedef enum {
    SWEEPER_DOWNLOAD_STATUS_NULL, //初始状态
    SWEEPER_DOWNLOAD_STATUS_START, //开始
    SWEEPER_DOWNLOAD_STATUS_STOP, //结束
} SWEEPER_DOWNLOAD_STATUS_E;

/**
 * @brief  扫地机实时地图上传的数据长度及内容
 */
typedef struct {
    INT_T len;
    CHAR_T* buff;
} SWEEPER_FILE_INFO;

/**
 * @brief  扫地机实时地图上传获取 SDK 内部状态机
 */
typedef struct _sweeper_ctrl {
    int session_id; //扫地机实时地图客户端 id
    SWEEPER_DOWNLOAD_STATUS_E album_status; //扫地机实时地图推送状态
    char rt_map_customize_fileName[48]; // 实时地图自定义文件名称
    int send_timeout_cnt; //发送超时累加
} SWEEPER_CTRL;

/**
 * @brief  扫地机实时地图上传资源管理结构体
 */
typedef struct {
    int used_num; //使用是总数
    SWEEPER_CTRL sweeper_ctrl[TY_SDK_P2P_NUM_MAX]; //每一路的参数
    void* rt_map_deal_sem; /* 实时地图发送任务处理信号量 */
} SWEEPER_RT_MAP_CTRL;

/**
 * @brief  扫地机实时地图上传资源全局变量
 */
SWEEPER_RT_MAP_CTRL g_sweeper_rt_map_ctrl = { 0 }; 

char g_file_path_name[10][256] = { 0 }; //测试文件的路径缓存

/**
 * @brief  获取实时地图上传通道资源
 * @param  [int] session_id 会话 id
 * @return [*]
 */
SWEEPER_CTRL* __get_sweeper_handle(int session_id)
{
    if (session_id < 0 || session_id >= TY_SDK_P2P_NUM_MAX) { //通道最大支持 5 路
        PR_ERR("session_id %d err\n", session_id);
        return NULL;
    }
    return &g_sweeper_rt_map_ctrl.sweeper_ctrl[session_id];
}

/**
 * @brief  实时地图及路径上传 event 回调具体处理
 * @param  [IN CONST channel] 连接哪里客户端
 * @param  [IN CONST SWEEPER_TRANSFER_EVENT_E] event 事件
 * @param  [IN CONST args] 回调数据
 * @return [INT] 结果
 */
INT_T tuya_sweeper_event_cb(INT_T channel, IN CONST SWEEPER_TRANSFER_EVENT_E event, IN CONST PVOID_T args)
{
    int ret = 0;
    SWEEPER_CTRL* pSweepHandle = NULL;
    PR_DEBUG(" sweeper rev event cb=[%d] ", event);
    switch (event) {

    case SWEEPER_FILE_DOWNLOAD_START: /* start download album */
    {
        pSweepHandle = __get_sweeper_handle(channel); //获取通道资源
        if (NULL == pSweepHandle) {
            break;
        }
#if defined(TUYA_SWEEPER_ALBUM_CUSTOMIZE_FILE_NUM_MAX) && (TUYA_SWEEPER_ALBUM_CUSTOMIZE_FILE_NUM_MAX > 0)
        C2C_CMD_IO_CTRL_ALBUM_DOWNLOAD_START* pSrcType = (C2C_CMD_IO_CTRL_ALBUM_DOWNLOAD_START*)args;
        C2C_CMD_IO_CTRL_ALBUM_fileInfo* pFileInfo = pSrcType->pFileInfoArr;

        for (int i = 0; i < (TUYA_SWEEPER_ALBUM_FILE_NUM_MAX + TUYA_SWEEPER_ALBUM_CUSTOMIZE_FILE_NUM_MAX); i++) {
            PR_DEBUG("rt_map_customize_fileName:[%s]\n", pFileInfo->filename); //获取到需要下载的 AI 原始图片
            if (0 == strncmp(pFileInfo->filename, "aiHD_", 0x05)) { //对比 aiHD_表示高清图片的前缀
                //获取下载的文件名
                strncpy(pSweepHandle->rt_map_customize_fileName, pFileInfo->filename, sizeof(pSweepHandle->rt_map_customize_fileName));
            }
            pFileInfo++;
        }
#endif

        pSweepHandle->session_id = channel; //通道号赋值
        if (pSweepHandle->album_status == SWEEPER_DOWNLOAD_STATUS_START) { //本次连接已经存在的 session_id，且已经开始下载
            PR_DEBUG("session id already existed");
            break;
        }
        pSweepHandle->album_status = SWEEPER_DOWNLOAD_STATUS_START; //开始下载标志置位
        pSweepHandle->send_timeout_cnt = 0; //累计数据就清 0
        g_sweeper_rt_map_ctrl.used_num++;
        PR_DEBUG("session_id %d", channel);
        ret = tuya_hal_semaphore_post(g_sweeper_rt_map_ctrl.rt_map_deal_sem); //释放信号量
        if (ret != 0) {
            PR_ERR("post ctrl deal sem err %d", ret);
            return OPRT_COM_ERROR;
        }
        break;
    }
    case SWEEPER_FILE_DOWNLOAD_CANCEL: // cancel download
    {
        pSweepHandle = __get_sweeper_handle(channel); //获取通道资源
        if (NULL == pSweepHandle) {
            break;
        }
        pSweepHandle->album_status = SWEEPER_DOWNLOAD_STATUS_STOP; //下载状态置暂停
        ret = tuya_hal_semaphore_post(g_sweeper_rt_map_ctrl.rt_map_deal_sem); //释放信号量
        if (ret != 0) {
            PR_ERR("post ctrl deal sem err %d\n", ret);
            return OPRT_COM_ERROR;
        }
        break;
    }
    default:
        ret = OPRT_COM_ERROR;
        break;
    }
    return ret;
}
/**
 * @brief  读取文件
 * @param  [int] fileType 文件对应的类型
 * @param  [SWEEPER_FILE_INFO *] pStrFileInfo
 * @return [INT] 结果
 */
int sweeper_read_file(int fileType, SWEEPER_FILE_INFO* pStrFileInfo)
{
    char fullpath[256] = { 0 };

    strcpy(fullpath, g_file_path_name[fileType]);

    PR_DEBUG(" read file :[%s]\n", fullpath);

    TUYA_FILE* fp = tuya_hal_fopen(fullpath, "rb");
    if (fp == NULL) {
        PR_ERR("File Open Failed:%s", fullpath);
        return -1;
    }

    tuya_hal_fseek(fp, 0, SEEK_END);
    int len = tuya_hal_ftell(fp);
    if (len <= 0) {
        PR_ERR("file len is :%d", len);
        tuya_hal_fclose(fp);
        return -1;
    }
    tuya_hal_fseek(fp, 0, SEEK_SET);

    pStrFileInfo->buff = (char*)Malloc(len);
    if (pStrFileInfo->buff == NULL) {
        PR_ERR("malloc failed :%d", len);
        tuya_hal_fclose(fp);
        return -1;
    }

    int read_len = tuya_hal_fread(pStrFileInfo->buff, len, fp);
    if (read_len != len) {
        PR_ERR("tuya_hal_fread failed!");
        tuya_hal_fclose(fp);
        Free(pStrFileInfo->buff);
        return -1;
    }

    tuya_hal_fclose(fp);
    fp = NULL;

    pStrFileInfo->len = len;
    PR_DEBUG("pStrFileInfo->len %d\n", pStrFileInfo->len);
    return 0;
    //根据固定的文件类型获取对应的文件内容，只是 demo 测试用，开发者可以根据自己业务的需求实现
}

/**
 * @brief  业务实时地图上传启动的逻辑
 * @param  [void*] arg
 * @return [*]
 */
void* thread_album_send(void* arg)
{
    int ret = 0;
    int i = 0;
    PR_DEBUG("THREAD_ALBUM_SEND start...\n");
    while (1) {
        ret = tuya_hal_semaphore_waittimeout(g_sweeper_rt_map_ctrl.rt_map_deal_sem, RT_MAP_TRANS_SLEEP_TIME); //默认最长等待 2s
        if (OPRT_OK == ret) { // 等待信号量成功
            PR_DEBUG("recv sem goto rt map send!\r\n");
        } else {
            PR_DEBUG("real-time map sleep time ok goto send!\r\n");
        }

        PR_DEBUG("real-time map client num %d\r\n", g_sweeper_rt_map_ctrl.used_num);
        if (g_sweeper_rt_map_ctrl.used_num <= 0) { //没有客户端接入，不执行
            continue;
        }
        for (i = 0; i < TY_SDK_P2P_NUM_MAX; i++) {
            SWEEPER_CTRL* pSweeper = __get_sweeper_handle(i);
            if (SWEEPER_DOWNLOAD_STATUS_NULL == pSweeper->album_status) { //未开始下载，不执行
                continue;
            }
            if (SWEEPER_DOWNLOAD_STATUS_STOP == pSweeper->album_status) { // 收到暂停下载之后，给 APP 上报一个传输结束
                PR_DEBUG("session_id %d tuya_sweeper_stop_send_data_to_app %d", pSweeper->session_id, g_sweeper_rt_map_ctrl.used_num);
                /*该接口用户通知 APP 文件传输结束，入参是当前的会话 ID*/
                tuya_sweeper_stop_send_data_to_app(pSweeper->session_id);
                pSweeper->album_status = SWEEPER_DOWNLOAD_STATUS_NULL; //下载状态初始化
                if (g_sweeper_rt_map_ctrl.used_num) {
                    g_sweeper_rt_map_ctrl.used_num--;
                }
            } else if (SWEEPER_DOWNLOAD_STATUS_START == pSweeper->album_status) { // 收到开始下载之后，直接上报地图及路径数据

            #if defined(TUYA_SWEEPER_ALBUM_CUSTOMIZE_FILE_NUM_MAX) && (TUYA_SWEEPER_ALBUM_CUSTOMIZE_FILE_NUM_MAX > 0)
                int total_fileType = TUYA_SWEEPER_ALBUM_FILE_NUM_MAX + TUYA_SWEEPER_ALBUM_CUSTOMIZE_FILE_NUM_MAX; //最多需要传输的地图文件数据
            #else
                int total_fileType = TUYA_SWEEPER_ALBUM_FILE_NUM_MAX; //最多需要传输的地图文件数据
            #endif
                for (int fileType = 0; fileType < total_fileType; fileType++) {
 
                    SWEEPER_FILE_INFO strFileInfo = { 0 };
                    ret = sweeper_read_file(fileType, &strFileInfo); //获取文件数据，准备上报
                    if (0 != ret) {
                        PR_DEBUG("__get_sweeper_file_bufferr %d\n", ret);
                        if (strFileInfo.buff != NULL) {  //释放申请的 buff
                            free(strFileInfo.buff);
                            strFileInfo.buff = NULL;
                        }
                        continue;
                    }
                    /*
                    该接口通过 P2P 通道将地图数据发送给 APP，
                    第一个参数是 P2P 通道的会话句柄，在下载地图时，从回调函数中能拿到；
                    第二个参数是地图文件的长度
                    第三个参数是地图文件的内容
                    第四个参数是 P2P 发送接口超时时间
                    */
                    /*默认每隔 2000mS 上报一次路径数据。这个上报周期，客户可以根据地图大小和网络情况去调整上报频率；
                      按照涂鸦扫地机协议地图数据格式上到涂鸦智能 APP 上，以 4K 数据到面板上解析显示的为列，
                      面板绘图耗时在 500ms，加上面板在通道上获取数据的耗时，合理的上报频率在 1000ms 以上。
                      在弱网下，发送数据容易卡住，最大超时时间 RT_MAP_TRANS_TIMEOUT，可以设置。
                    */
                    if (0 == fileType) {
                        ret = tuya_sweeper_send_map_data_with_buff(pSweeper->session_id, strFileInfo.len, strFileInfo.buff, RT_MAP_TRANS_TIMEOUT);  //地图上报接口
                    } else if (1 == fileType) {
                        ret = tuya_sweeper_send_cleanpath_data_with_buff(pSweeper->session_id, strFileInfo.len, strFileInfo.buff, RT_MAP_TRANS_TIMEOUT);//路径上报接口
                    }
                    /*注意：如果您需要增加自定义的文件名上报，
                           可以放在这里调用 tuya_sweeper_send_customize_data_with_buff 接口上报，
                           上报频率可参考地图及路径的方法处理。
                           自定义的文件名需要在 SDK 初始化完成后调用 tuya_sweeper_set_customize_name 接口填入，
                           且文件名称及文件数据格式，需要与面板对齐后。*/
                #if defined(TUYA_SWEEPER_ALBUM_CUSTOMIZE_FILE_NUM_MAX) && (TUYA_SWEEPER_ALBUM_CUSTOMIZE_FILE_NUM_MAX > 0)
                    else if (2 == fileType) {
                        ret = tuya_sweeper_send_customize_data_with_buff(pSweeper->session_id, "ai.bin.stream", strFileInfo.len, strFileInfo.buff, RT_MAP_TRANS_TIMEOUT);
                    } else if (3 == fileType) {
                        //判断文件坐标是否有效，填入对应坐标的原图进行上报，如下 aiHD_005b_0120.bin 名字一致就上报原始图片
                        if (0 == strncmp(pSweeper->rt_map_customize_fileName, "aiHD_005b_0120.bin", sizeof(pSweeper->rt_map_customize_fileName))) {
                            ret = tuya_sweeper_send_customize_data_with_buff(pSweeper->session_id, pSweeper->rt_map_customize_fileName, strFileInfo.len, strFileInfo.buff, RT_MAP_TRANS_TIMEOUT);
                            PR_DEBUG("session_id %d rt_map_customize_fileName[%s] \n", pSweeper->session_id, pSweeper->rt_map_customize_fileName);
                            memset(pSweeper->rt_map_customize_fileName, 0x00, sizeof(pSweeper->rt_map_customize_fileName));
                        }
                    }
                #endif
                    else {
                        ret = OPRT_NOT_FOUND;
                        PR_DEBUG("real-time map nav data don't support! %d", ret);
                    }
                    if (strFileInfo.buff != NULL) {  //释放申请的 buff
                        free(strFileInfo.buff);
                        strFileInfo.buff = NULL;
                    }
                }

                if (OPRT_OK == ret) { //发送完成
                    PR_DEBUG("real-time map send out finish session %d", pSweeper->session_id);
                    pSweeper->send_timeout_cnt = 0; //能发送数据就清 0
                } else {
                    if (ret != OPRT_NOT_FOUND) { //数据发送连续失败
                        pSweeper->send_timeout_cnt++;
                        if (pSweeper->send_timeout_cnt >= RT_MAP_SEND_TIMEOUT_CNT) {  //连续发送数据失败到 RT_MAP_SEND_TIMEOUT_CNT 次（开发者可自行定义），就主动关闭 P2P 链路。
                            pSweeper->send_timeout_cnt = 0;
                            tuya_sweeper_p2p_alone_stream_close(pSweeper->session_id); //连接被设备主动关闭
                            PR_NOTICE("close p2p send session %d", pSweeper->session_id);
                        }
                    }
                }
            } else {
                ;
            }
        }
    }
    PR_DEBUG("thread_sweeper_proc exit...\n");
}

/**
 * @brief  用户设置扫地机自定义名称
 * @param  [*]
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET ty_user_sweeper_set_customize_name(VOID)
{
    OPERATE_RET ret = 0;
#if defined(TUYA_SWEEPER_ALBUM_CUSTOMIZE_FILE_NUM_MAX) && (TUYA_SWEEPER_ALBUM_CUSTOMIZE_FILE_NUM_MAX > 0)
    TUYA_SWEEPER_CUSTOMIZE_FILEINFO customize_name = { 0 };
    customize_name.itemCount = TUYA_SWEEPER_ALBUM_CUSTOMIZE_FILE_NUM_MAX;
    strncpy(customize_name.itemArr[0].filename, "aiHD_XXXX_YYYY.bin", strlen("aiHD_XXXX_YYYY.bin"));
    strncpy(customize_name.itemArr[1].filename, "ai.bin.stream", strlen("ai.bin.stream"));
    ret = tuya_sweeper_set_customize_name(&customize_name);
    if (OPRT_OK != ret) {
        PR_ERR("user set customize name failed!");
    }
#endif
    return ret;
}

/**
 * @brief  扫地机实时地图上传服务启动
 * @param  [*]
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET ty_user_sweeper_rt_map_init(void)
{
    OPERATE_RET ret = 0;
    THRD_PARAM_S thrd_param;
    THRD_HANDLE thrd; /* 任务句柄 */

    /* 创建实时地图上报的线程任务 */
    thrd_param.stackDepth = 1024 * 1024;
    thrd_param.thrdname = "rt_map_trans_thrd";
    thrd_param.priority = TRD_PRIO_1;

    /* 创建机器状态管理任务 */
    ret = CreateAndStart(&thrd, NULL, NULL, (void*)thread_album_send, NULL, &thrd_param);
    if (OPRT_OK != ret) {
        PR_ERR("rt_map_trans_thrd create failed!");
        return ret;
    }

    ret = tuya_hal_semaphore_create_init(&g_sweeper_rt_map_ctrl.rt_map_deal_sem, 0, 1); //创建一个实时地图上报的信号量
    if (OPRT_OK != ret) {
        PR_ERR("real-time map sem create err!\r\n");
        return ret;
    }
    extern char s_raw_path[];
    PR_DEBUG("THREAD_ALBUM_SEND start...\n");
    sprintf(g_file_path_name[0], "%s%s", s_raw_path, "/tmp/map.bin"); //测试用，创建临时的文件
    sprintf(g_file_path_name[1], "%s%s", s_raw_path, "/tmp/cleanPath.bin");
    sprintf(g_file_path_name[2], "%s%s", s_raw_path, "/tmp/ai.bin");
    sprintf(g_file_path_name[3], "%s%s", s_raw_path, "/tmp/aiHD_005b_0120.bin");
    return OPRT_OK;
}
