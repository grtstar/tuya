/*********************************************************************************
 *Copyright(C),2015-2020, TUYA www.tuya.comm
 *FileName:    tuya_sdk_voice_demo
  *brief：This article introduces a demo for the voice file download feature.
          Developers can directly build upon this demo for further development
          if needed.
 **********************************************************************************/
#include "uni_log.h"
#include "uni_thread.h"
#include "uni_time.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "gw_intf.h"
#include "iot_httpc.h"
#include "tuya_cloud_types.h"
#include "tuya_hal_semaphore.h"
#include "tuya_iot_internal_api.h"
#include "tuya_sdk_common.h"
#include "ty_cJSON.h"

typedef struct {
    THRD_HANDLE thread_handler; //任务句柄
    void* sem; //发送任务处理信号量
    char* url; // url下载路径
    uint32_t url_len; // url长度
    FILE* fp; //下载的语音保存到文件
} VOICE_DOWNLOAD_T;

static VOICE_DOWNLOAD_T svc_voice_download_handler = { 0 }; //下载语音资源全面变量

/**
 * @brief http 文件下载回调，用于进度上报
 * @param pri_data （未使用）
 * @param total_len 总长度
 * @param offset 下载进度
 * @param data 下载数据
 * @param len 数据长度
 * @param remain_len 剩余长度
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
STATIC OPERATE_RET __get_download_file_data_cb(IN PVOID_T pri_data,
    IN CONST UINT_T total_len,
    IN CONST UINT_T offset,
    IN CONST BYTE_T* data,
    IN CONST UINT_T len,
    OUT UINT_T* remain_len)
{
    if (svc_voice_download_handler.fp) { //判断文件是否存在
        fwrite(data, sizeof(char), len, svc_voice_download_handler.fp); //有数据就写入fp
    } else {
        PR_ERR("file not opened");
    }
    //开发者在这里可以根据下载的进度和下载的结果做dp同步，注意不要在该接口中处理，可以通过任务事件传送的方式。
    return OPRT_OK;
}

/**
 * @brief 下载指令处理
 * @param url url数据
 * @param url_len url长度
 * @param language_id 语言id
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET robotics_svs_voice_download_req(char* url, unsigned int url_len, unsigned int language_id)
{
    OPERATE_RET ret = OPRT_OK;

    if (NULL == url || (url_len == 0)) { //判断url是否存在
        PR_ERR("recv download url null");
        return OPRT_INVALID_PARM;
    }

    PR_DEBUG("recv download url: %s", url);
    if (NULL != svc_voice_download_handler.url) { //释放之前url的buff
        free(svc_voice_download_handler.url);
        svc_voice_download_handler.url = NULL;
    }
    svc_voice_download_handler.url = malloc(url_len + 1); //根据url的长度重新申请buff
    if (NULL == svc_voice_download_handler.url) {
        PR_ERR("malloc svc voice download url handler failed");
        return OPRT_MALLOC_FAILED;
    }

    memset(svc_voice_download_handler.url, 0, url_len + 1);
    strncpy(svc_voice_download_handler.url, url, url_len);
    svc_voice_download_handler.url_len = url_len;

    PR_DEBUG("recv download url: %s", svc_voice_download_handler.url);
    tuya_hal_semaphore_post(svc_voice_download_handler.sem); //释放信号量
    return ret;
}

/**
 * @brief  语音下载的功能测试入口
 * @param  [*]
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET voice_download_oper(VOID)
{
    OPERATE_RET ret = OPRT_OK;
    /**********对文件打开及读取的流程，测试用****************/
    char local_name[128] = { 0 };
    UINT_T actual_read = 0;
    struct stat statbuff;
    extern char s_raw_path[];

    sprintf(local_name, "%s%s", s_raw_path, "/resource/tuya_sweeper_robot/download_demo.txt"); //文件中读取url链接，这里是测试的临时文件
    FILE* fp = fopen(local_name, "rb");
    if (fp == NULL) {
        PR_ERR("File Open Fails:%s\r\n", local_name);
        return -1;
    }
    if (stat(local_name, &statbuff) < 0) {
        PR_ERR("File Not Exist:%s\r\n", local_name);
        fclose(fp);
    }
    UINT_T size = statbuff.st_size;
    CHAR_T* buf = (CHAR_T*)malloc(size);
    if (buf == NULL) {
        PR_ERR("Malloc Fail %d\r\n", size);
        fclose(fp);
    }
    actual_read = fread(buf, 1, size, fp);
    /**********对文件打开及读取的流程，测试用****************/
    //开发者只需要将自己要下载的url链接及长度传给下面接口即可。
    ret = robotics_svs_voice_download_req(buf, actual_read, 0);
    if (ret != OPRT_OK) {
        PR_ERR("download url failed");
    }
    if (buf) {
        free(buf);
    }
    return ret;
}

/**
 * @brief 语音下载线程
 *
 * @param args
 * @return void*
 */
void* __voice_download_task(void* args)
{
    OPERATE_RET ret = OPRT_OK;
    UCHAR_T file_hmac[32] = { 0 };
    UINT_T file_sz = 0;
    char cmd[128] = { 0 };
    UCHAR_T md5_check_flag = FALSE;

    PR_DEBUG("__voice_download_task __begin");
    while (1) {
        tuya_hal_semaphore_wait(svc_voice_download_handler.sem); //等待信号量

        PR_DEBUG("recv download url: %s", svc_voice_download_handler.url);

        snprintf(cmd, sizeof(cmd), "/tmp/tmp.gz"); //存入临时文件中，开发者存到自己指定的文件中
        svc_voice_download_handler.fp = fopen(cmd, "wb+");
        if (NULL != svc_voice_download_handler.url) { //文件buff大小
            // 50 * 1024是http分包的大小，http分包的大小，具体需要看平台的系统内存能分配多少，推荐一般几十K。
            ret = iot_httpc_download_file(svc_voice_download_handler.url, 50 * 1024, __get_download_file_data_cb,
                NULL, file_sz, file_hmac); //开始下载文件，该接口是阻塞
            if (ret != OPRT_OK) {
                PR_ERR("voice download failed ret %d", ret);
            }
            free(svc_voice_download_handler.url); //释放buff
            svc_voice_download_handler.url = NULL;
            svc_voice_download_handler.url_len = 0;
        }
        if (svc_voice_download_handler.fp) { //关闭文件
            fclose(svc_voice_download_handler.fp);
            svc_voice_download_handler.fp = NULL;
        }

        //下载完毕由开发者自行处理，比如最后下载数据校验通过后md5_check_flag状态置TRUE.
        //计算下载到的md5的值与协议中的md5是否一致
        //如果md5一致，就把md5_check_flag=TRUE
        if (md5_check_flag == TRUE) {
            PR_DEBUG("voice download succeed");
            //做相应的业务处理
        } else {
            PR_ERR("voice check failed");
        }
    }
}

/**
 * @brief  voice_init处理初始化
 * @param  [*]
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET robotics_svc_init_voice(void)
{
    OPERATE_RET ret = 0;

    THRD_PARAM_S thrd_param;

    /* 创建线程任务 */
    thrd_param.stackDepth = 1024;
    thrd_param.priority = TRD_PRIO_2;
    thrd_param.thrdname = "svc_voice_download_thrd";
    /* 创建语音下载管理任务 */
    ret = CreateAndStart(&svc_voice_download_handler.thread_handler, NULL, NULL, (void*)__voice_download_task, NULL, &thrd_param);
    if (OPRT_OK != ret) {
        PR_ERR("robot svc voice download task create failed!");
        return ret;
    }
    ret = tuya_hal_semaphore_create_init(&svc_voice_download_handler.sem, 0, 1); //创建信号量
    if (OPRT_OK != ret) {
        PR_ERR("voice download sempaphore init failed ret %d", ret);
    }
    return ret;
}
