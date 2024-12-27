/*********************************************************************************
 *Copyright(C),2015-2020, TUYA www.tuya.comm
 *FileName:    tuya_sdk_log_upload_demo
  *brief：The device local log reporting function is to upload the logs stored 
          locally to the cloud, allowing users to download the corresponding logs 
	  from the backend.
 **********************************************************************************/
#include "tuya_sdk_common.h"
#include "tuya_iot_com_api.h"
#include "uni_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

/**
 * @brief  设备本地日志上报的处理
 * @param  [char*] path 上报的日志路径
 * @param  [int] len
 * @return [*]
 */
void cloud_log_path_cb(OUT char* path, IN CONST int len)
{
    char cmd[128] = { 0 };
    char buffer[128] = { 0 };
    FILE* fp = NULL;

    PR_DEBUG("tar log goto");

    fp = fopen("/tmp/local_log.log", "r+");    //判断本地文件是否存在
    if (NULL == fp) {
        snprintf(cmd, sizeof(cmd), "tar -cvf /tmp/log_all.tar /tmp/robotics_main.log /tmp/backtrace.log");    //开发者将本地的log日志压缩打包,需要注意本地的log必现存在
    } else {
        fgets(buffer, sizeof(buffer), fp);     //判断文件里面是否存在对应的log
        if (strcmp(buffer, "robotics_main.log") == 0) {
            snprintf(cmd, sizeof(cmd), "tar -cvf /tmp/log_all.tar /tmp/robotics_main.log /tmp/backtrace.log");
        } else if (strcmp(buffer, "run_log") == 0) {    //可上报文件夹
            snprintf(cmd, sizeof(cmd), "tar -cvf /tmp/log_all.tar /tmp/run_log");
        }else {
            snprintf(cmd, sizeof(cmd), "tar -cvf /tmp/log_all.tar /tmp/robotics_main.log");  //默认上报文件
            // 将文件指针移到文件开头
            fseek(fp, 0, SEEK_SET);
            // 截断文件为零长度
            ftruncate(fileno(fp), 0);
        }
    }
    if (fp) {
        fclose(fp);
    }

    system(cmd);
    PR_DEBUG("tar log finish!");
    strcpy(path, "/tmp/log_all.tar");   //制定需要上报的路径
    PR_DEBUG("goto upload local log finish");

    return;
}

/**
 * @brief  设备本地日志上报之后的回调函数
 * @param  [int] result 处理的结果 0 OK
 * @return [*]
 */
void cloud_log_deal_cb(OUT INT_T result)
{
    char cmd[128] = { 0 };
    char buffer[128] = { 0 };
    FILE* fp = fopen("/tmp/local_log.log", "r+");

    PR_DEBUG("upload app log %d, goto delete data! ", result);

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "rm -rf /tmp/log_all.tar");    //删除文件
    system(cmd);

    if (OPRT_OK == result) { // 如果上报成功了，需要修改下次上报的内容
        fp = fopen("/tmp/local_log.log", "r+");
        if (NULL == fp) { // 打不开的时候，就是说明文件不存在，但是又上报成功了，那么下次的时候就强制上报log
            fp = fopen("/tmp/local_log.log", "w+");
            if (fp != NULL) {
                fputs("robotics_main.log", fp);
            }
        } else {
            fgets(buffer, sizeof(buffer), fp);
            // 将文件指针移到文件开头
            fseek(fp, 0, SEEK_SET);
            if (strcmp(buffer, "robotics_main.log") == 0) {  //循环上报log文件
                // 截断文件为零长度
                ftruncate(fileno(fp), 0);
                fputs("run_log", fp);
            } else {
                 // 截断文件为零长度
                ftruncate(fileno(fp), 0);
                fputs("robotics_main.log", fp);
            }
        }
    }
    if (fp) {
        fclose(fp);
    }
    return;
}

/**
 * @brief  设备本地日志上报初始化
 * @param  [*]
 * @return [*]
 */
void ty_log_upload_init(void)    
{
    TY_IOT_APP_CBS_S app_log_deal = {
        cloud_log_path_cb,
        cloud_log_deal_cb
    };

    tuya_iot_app_cbs_init(&app_log_deal); // 注册云端日志回调处理函数
}