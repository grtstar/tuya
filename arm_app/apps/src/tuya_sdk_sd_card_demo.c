/*********************************************************************************
 *Copyright(C),2015-2020, TUYA www.tuya.comm
 *FileName:    tuya_sdk_sd_card_demo
 *brief：1.The TUYA SD card storage and playback functionality is adopted.
         2.Developers need to implement the following related interfaces.
           VOID tuya_ipc_sd_format(VOID); VOID tuya_ipc_sd_remount(VOID);
           STREAM_STORAGE_WRITE_MODE_E tuya_ipc_sd_get_mode_config(VOID);
           SD_STATUS_E tuya_ipc_sd_get_status(VOID);
           VOID tuya_ipc_sd_get_capacity(UINT_T *p_total, UINT_T *p_used,
           UINT_T *p_free);CHAR_T *tuya_ipc_get_sd_mount_path(VOID);
         3.The interface names cannot be changed, as these interfaces are
           declared and used in the SDK.
         4.This demonstration file provides a management implementation for SD card
           operations in typical embedded devices under the Linux system. Developers
           can modify it according to their actual applications.
 **********************************************************************************/
#include "utilities/uni_log.h"
#include <libgen.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/statfs.h>
#include <unistd.h>

#include "tuya_iot_config.h"
#include "tuya_ipc_api.h"
#include "tuya_ipc_media.h"
#include "tuya_ipc_media_stream_event.h"
#include "tuya_ipc_p2p.h"
#include "tuya_ipc_stream_storage.h"
#include "tuya_p2p_api.h"
#include "tuya_ring_buffer.h"
#include "tuya_sdk_dp_demo.h"

/**
 * @brief  SD卡信息
 */
#define MAX_MMC_NAME_LEN 16 //存储卡名称长度
#define MAX_MOUNTS_INFO_LEN 1024 //加载文件系统信息长度
#define LINUX_SD_DEV_FILE "/dev/mmcblk0" //挂载的SD卡文件
#define LINUX_MOUNT_INFO_FILE "/proc/mounts" //系统上挂载的文件系统的详细信息
#define FORMAT_CMD "mkfs.vfat" // SD卡格式化VFAT文件系统的指令

/**
 * @brief  SD卡状态
 */
typedef struct {
    BOOL_T sd_record_switch; // SD卡录制开关
    STREAM_STORAGE_WRITE_MODE_E sd_record_mode; //录制模式
    char s_mounts_info[MAX_MOUNTS_INFO_LEN]; //加载文件系统信息
    char s_mmcblk_name[MAX_MMC_NAME_LEN]; //存储卡名称
    char s_sd_mount_path[SS_BASE_PATH_LEN]; //文件路径
} ROBOT_SD_S;

STATIC ROBOT_SD_S robot_sd_set = { 0 }; // sd卡资源全局变量

/**
 * @brief  格式化操作实现接口
 * @param  [*]
 * @return [*]
 */
VOID tuya_ipc_sd_format(VOID)
{
    CHAR_T format_cmd[256] = { 0 };
    char buffer[512] = { 0 };
    PR_DEBUG("sd format begin\n");

    snprintf(format_cmd, 256, "umount %s;%s %s;mkdir -p /mnt/sdcard;mount -t auto %s /mnt/sdcard;",
        robot_sd_set.s_mmcblk_name, FORMAT_CMD, robot_sd_set.s_mmcblk_name, robot_sd_set.s_mmcblk_name); //格式化指令
    PR_DEBUG("execute: %s\n", format_cmd);
    FILE* pp = popen(format_cmd, "r");
    if (NULL != pp) {
        fgets(buffer, sizeof(buffer), pp);
        PR_INFO("%s\n", buffer);
        pclose(pp);
    } else {
        PR_DEBUG("format_sd_card failed\n");
    }
    PR_DEBUG("sd format end\n");
}

/**
 * @brief  重挂载的实现接口
 * @param  [*]
 * @return [*]
 */
VOID tuya_ipc_sd_remount(VOID)
{
    CHAR_T format_cmd[128] = { 0 };
    char buffer[512] = { 0 };
    SD_STATUS_E status = SD_STATUS_UNKNOWN;

    status = tuya_ipc_sd_get_status(); //获取SD卡状态
    if (SD_STATUS_NORMAL == status) {
        PR_DEBUG("sd don't need to remount!\n");
        return;
    }
    PR_DEBUG("remount_sd_card ..... \n");

    snprintf(format_cmd, 128, "umount %s;sleep 1;mount -t auto %s /mnt/sdcard;", robot_sd_set.s_mmcblk_name, robot_sd_set.s_mmcblk_name); //挂载指令
    FILE* pp = popen(format_cmd, "r");
    if (NULL != pp) {
        fgets(buffer, sizeof(buffer), pp);
        PR_INFO("%s\n", buffer);
        pclose(pp);
    } else {
        PR_INFO("remount_sd_card failed\n");
    }
}
/**
 * @brief  SD卡录像开关状态
 * @param  [BOOL_T]  sd_record_on_off true为打开，false为关闭
 * @return [*]
 */
void tuya_robot_set_sd_record_onoff(BOOL_T sd_record_on_off)
{
    PR_DEBUG("curr sd_record_on_off:%d \r\n", sd_record_on_off);
    robot_sd_set.sd_record_switch = sd_record_on_off; //这里一般是dp点控制
}

/**
 * @brief  SD卡录像模式获取
 * @param  [*]
 * @return [STREAM_STORAGE_WRITE_MODE_E] 录制模式
 */
STREAM_STORAGE_WRITE_MODE_E tuya_ipc_sd_get_mode_config(VOID)
{
    if (robot_sd_set.sd_record_switch) { //录制开关打开
        STREAM_STORAGE_WRITE_MODE_E sd_mode = robot_sd_set.sd_record_mode;
        if ((sd_mode >= SS_WRITE_MODE_NONE) && (sd_mode < SS_WRITE_MODE_MAX)) { //根据有效条件获取模式
            return sd_mode;
        }
    }
    return SS_WRITE_MODE_NONE;
}

/**
 * @brief  实现获取SD卡状态接口
 * @param  [*]
 * @return [SD_STATUS_E]
 */
SD_STATUS_E tuya_ipc_sd_get_status(VOID)
{
    return SD_STATUS_NORMAL; // ubuntu测试，先return掉

    // 有硬件支持，可以参考下面代码
    FILE* fp = fopen(LINUX_SD_DEV_FILE, "rb");
    if (!fp) {
        return SD_STATUS_NOT_EXIST;
    }
    fclose(fp);

    strcpy(robot_sd_set.s_mmcblk_name, LINUX_SD_DEV_FILE);
    if (0 == access(LINUX_SD_DEV_FILE "p1", F_OK)) { //默认节点名称信息
        strcat(robot_sd_set.s_mmcblk_name, "p1");
    }

    fp = fopen(LINUX_MOUNT_INFO_FILE, "rb");
    if (fp) {
        memset(robot_sd_set.s_mounts_info, 0, sizeof(robot_sd_set.s_mounts_info));
        fread(robot_sd_set.s_mounts_info, 1, MAX_MOUNTS_INFO_LEN, fp);
        fclose(fp);
        CHAR_T* mmcblk_name_start = strstr(robot_sd_set.s_mounts_info, "/dev/mmcblk"); //设备中确认的节点名称
        CHAR_T* mmcblk_name_end = strstr(robot_sd_set.s_mounts_info, " /mnt/sdcard");
        if (mmcblk_name_start && mmcblk_name_end) { //判断节点名称
            int mmcblk_name_len = mmcblk_name_end - mmcblk_name_start;
            if (mmcblk_name_len >= MAX_MMC_NAME_LEN) {
                return SD_STATUS_ABNORMAL;
            }
            strncpy(robot_sd_set.s_mmcblk_name, mmcblk_name_start, mmcblk_name_len);
            robot_sd_set.s_mmcblk_name[mmcblk_name_len] = '\0';
        } else { //有设备节点，但没有挂载信息。一般情况下，卡格式不正确，上报异常。
            return SD_STATUS_ABNORMAL;
        }
        //如果SD卡的挂载信息不在最后，后面有一个ro挂载，就会出现问题。
        if (NULL != strstr(mmcblk_name_start, "ro,")) {
            return SD_STATUS_ABNORMAL;
        }
        if (NULL == strstr(mmcblk_name_start, "vfat")) {
            return SD_STATUS_ABNORMAL;
        }
        if (access(robot_sd_set.s_mmcblk_name, 0)) {
            return SD_STATUS_ABNORMAL;
        }
        return SD_STATUS_NORMAL;
    } else {
        return SD_STATUS_UNKNOWN;
    }
}

/**
 * @brief  获取SD卡存储容量 单位 KB
 * @param  [int*] p_total 总容量
 * @param  [int*] p_used 使用容量
 * @param  [int*] p_empty 空闲容量
 * @return [*]
 */
VOID tuya_ipc_sd_get_capacity(UINT_T* p_total, UINT_T* p_used, UINT_T* p_free)
{
    *p_total = 1024 * 1024;
    *p_used = 1024 * 20;
    *p_free = *p_total - *p_used;
    return;

    struct statfs sd_fs;
    if (statfs("/mnt/sdcard", &sd_fs) != 0) {
        PR_ERR("statfs failed!/n");
        return;
    }

    *p_total = (UINT_T)(((UINT64_T)sd_fs.f_blocks * (UINT64_T)sd_fs.f_bsize) >> 10);
    *p_used = (UINT_T)((((UINT64_T)sd_fs.f_blocks - (UINT64_T)sd_fs.f_bfree) * (UINT64_T)sd_fs.f_bsize) >> 10);
    *p_free = (UINT_T)(((UINT64_T)sd_fs.f_bavail * (UINT64_T)sd_fs.f_bsize) >> 10);
    PR_DEBUG("sd capacity: total: %d KB, used %d KB, free %d KB\n", *p_total, *p_used, *p_free);
    return;
}

/**
 * @brief  实现获取SD挂载路径
 * @param  [*]
 * @return [CHAR_T *]
 */
CHAR_T* tuya_ipc_get_sd_mount_path(VOID)
{
    PR_DEBUG("s_sd_mount_path = %s", robot_sd_set.s_sd_mount_path);
    return robot_sd_set.s_sd_mount_path;
}

/**
 * @brief  获取SD状态回调
 * @param  [int] status
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
int tuya_robot_sd_status_upload(int status)
{
#ifdef TUYA_DP_SD_STATUS_ONLY_GET
    dp_handle_sd_status_response(status); // dp上报
#endif
    return 0;
}

/**
 * @brief  获取SD卡存储容量
 * @param  [int*] p_total 总容量
 * @param  [int*] p_used 使用容量
 * @param  [int*] p_empty 空闲容量
 * @return [*]
 */
void tuya_robot_get_sd_storage(int* p_total, int* p_used, int* p_empty)
{
    /* 开发者根据实际情况返回本地SD卡存储状态，单位是 kb */
    *p_total = 128 * 1000 * 1000;
    *p_used = 32 * 1000 * 1000;
    *p_empty = *p_total - *p_used;

    PR_DEBUG("curr sd total:%u used:%u empty:%u \r\n", *p_total, *p_used, *p_empty);
}

/**
 * @brief  设置录像模式
 * @param  [STREAM_STORAGE_WRITE_MODE_E]  sd_record_mode 1为事件录像，2为连续录像
 * @return [*]
 */
void tuya_robot_set_sd_record_mode(STREAM_STORAGE_WRITE_MODE_E sd_record_mode)
{
    PR_DEBUG("set sd_record_mode:%d \r\n", sd_record_mode);
    if ((sd_record_mode >= SS_WRITE_MODE_NONE) && (sd_record_mode < SS_WRITE_MODE_MAX)) {
        tuya_ipc_ss_set_write_mode(sd_record_mode); //根据模式来写数据
    }

    robot_sd_set.sd_record_mode = sd_record_mode; //录制模式
}

/**
 * @brief  sd卡任务
 * @param  [STREAM_STORAGE_WRITE_MODE_E]  sd_record_mode 1为事件录像，2为连续录像
 * @return [*]
 */
static void* thread_sd_format(void* arg)
{
    /* First notify to app, progress 0% */
#ifdef TUYA_DP_SD_FORMAT_STATUS_ONLY_GET
    dp_handle_report_sd_format_status(0); //格式化状态上报
#endif
    sleep(1);
    /* Stop local SD card recording and playback, progress 10%*/
#ifdef TUYA_DP_SD_FORMAT_STATUS_ONLY_GET
    dp_handle_report_sd_format_status(10); //格式化状态上报
#endif
    tuya_ipc_ss_set_write_mode(SS_WRITE_MODE_NONE); //不写数据模式
    tuya_ipc_ss_pb_stop_all(); //停止所有正在进行的播放
    sleep(1);

    /* Delete the media files in the SD card, the progress is 30% */
#ifdef TUYA_DP_SD_FORMAT_STATUS_ONLY_GET
    dp_handle_report_sd_format_status(30); //格式化状态上报
#endif
    // tuya_ipc_ss_delete_all_files();   //可以删除tuya SDK存储的所有视频/音频数据
    sleep(1);

    tuya_ipc_sd_format(); //执行SD卡格式化操作

#ifdef TUYA_DP_SD_FORMAT_STATUS_ONLY_GET
    dp_handle_report_sd_format_status(80); //格式化状态上报
#endif
    tuya_ipc_ss_set_write_mode(SS_WRITE_MODE_ALL); //始终写数据模式

    sleep(1);
#ifdef TUYA_DP_SD_STORAGE_ONLY_GET
    dp_handle_sd_storage_response(); // SD卡容量上报
#endif
    /* progress 100% */
#ifdef TUYA_DP_SD_FORMAT_STATUS_ONLY_GET
    dp_handle_report_sd_format_status(100); //格式化状态上报
#endif
    pthread_exit(0);
}

/**
 * @brief  创建SD格式化任务
 * @param
 * @return [*]
 */
void tuya_robot_format_sd_card(void)
{
    PR_DEBUG("start to format sd_card \r\n");
    /* SD卡格式化。
       SDK已经完成了部分代码的编写，
       开发人员只需要实现格式化操作。*/

    pthread_t sd_format_thread;
    pthread_create(&sd_format_thread, NULL, thread_sd_format, NULL);
    pthread_detach(sd_format_thread);
}