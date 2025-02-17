/*********************************************************************************
 *Copyright(C),2015-2020, TUYA www.tuya.comm
 *FileName:    tuya_sdk_upgrade_demo
 *brief：1.This article explains how to implement the OTA (Over-The-Air) upgrade feature.
 *        2.Developers can use this demo as a basis for further development.
 **********************************************************************************/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uni_time.h>
#include <unistd.h>

#include "tuya_cloud_com_defs.h"
#include "tuya_iot_com_api.h"
#include "tuya_iot_config.h"
#include "tuya_sdk_common.h"
#include "tuya_svc_upgrade.h"
#include "utilities/uni_log.h"

/**
 * @brief OTA 升级进度同步
 * @param[UINT_T] percent: 升级进度百分比，有效值 [0,100]
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_robot_upgrade_progress_report(IN UINT_T percent)
{
    if (percent < 0 || percent > 100) {
        PR_ERR("invalid percent %d.\n", percent);
        return OPRT_COM_ERROR;
    }

    STATIC UINT_T prePercent = 0;
    STATIC UINT_T prePosix = 0;
    UINT_T curPosix = uni_time_get_posix(); //获取系统时间戳
    //这里是根据 OTA 升级，SDK 内部分包大小来推算的进度值，无特殊要求，开发者无需再做修改，可直接使用。
    if ((curPosix - prePosix >= 2) || (percent - prePercent >= 8) || ((percent >= 95) && (percent > prePercent))) {
        prePercent = percent;
        prePosix = curPosix;
        PR_DEBUG("mqtt report download percent:%d \n", percent);
        OPERATE_RET op_ret = tuya_iot_dev_upgd_progress_rept(percent, NULL, DEV_NM_ATH_SNGL); //升级进度上报
        if (OPRT_OK != op_ret) {
            PR_ERR("tuya_iot_dev_upgd_progress_rept err:%d", op_ret);
        }
    }

    return OPRT_OK;
}

/**
 * @brief  OTA 进度通知回调
 * @param  [FW_UG_S] *fw
 * @param  [int] download_result 下载结果
 * @param  [void*] pri_data
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET __ty_user_upgrade_notify_cb(IN CONST FW_UG_S* fw, IN CONST INT_T download_result, IN PVOID_T pri_data)
{

    PR_DEBUG("download_result:%d fw_url:%s", download_result, fw->fw_url);

    switch (download_result) {
    case OPRT_OK:
        PR_DEBUG("Upgrade Finish");
        // 开发者需要在 OTA 文件成功下载到指定路径后，本地实现 OTA 升级文件更新的操作
        //其它比较耗时的业务逻辑不要在这边处理，可以通过事件的形式发送出去，到别的任务上出来。
        system("./upgrade.sh > ../upgrade.log"); //执行升级脚本
        break;
    default:
        // 收到升级失败指令，清空升级状态
        PR_DEBUG("upgrade https error:%d\n", download_result);
        //升级失败，开发者可以将下载到本地文件删除等动作
        break;
    }
    //注意：不管升级成功还是升级失败，最终都需要 reboot system 重启

    return OPRT_OK;
}

/**
 * @brief  OTA 升级数据回调
 * @param [FW_UG_S] *fw
 * @param total_len 总长度
 * @param offset 下载进度
 * @param data 下载数据
 * @param len 数据长度
 * @param remain_len 剩余长度
 * @param pri_data 文件
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET __ty_user_get_file_data_cb(IN CONST FW_UG_S* fw, IN CONST UINT_T total_len, IN CONST UINT_T offset,
    IN CONST BYTE_T* data, IN CONST UINT_T len, OUT UINT_T* remain_len, IN PVOID_T pri_data)
{
    static int report_percent = 0;
    PR_DEBUG("Rev File Data");

    if (NULL == data || len <= 0) {
        PR_DEBUG("__ty_user_get_file_data_cb data is NULL or len is invalid");
        return OPRT_COM_ERROR;
    }
    PR_DEBUG("total_len:%d  fw_url:%s", total_len, fw->fw_url);
    PR_DEBUG("Offset:%d Len:%d", offset, len);
    // 开始写入 OTA 文件到指定的 flash 中
    /* only for example:
    FILE *p_upgrade_fd = (FILE *)pri_data;
    fwrite(data, 1, len, p_upgrade_fd);
    *remain_len = 0;
    */
    // 报告升级过程，不仅仅是下载百分比，还要考虑写入内存的时间。
    // 如果升级过程在 60 秒（该时间可以在 IOT 后台设置，默认为 60 秒）内没有更新，应用程序将报告超时失败。
    int percent = offset * 100 / total_len;
    if (percent != report_percent) {
        report_percent = percent;
        PR_DEBUG("report percent[%u]\n", report_percent);
        if (report_percent < 100) { //设备一般不上报 100，比如 OTA 文件下载完成，本地更新固件失败，这个时候上报 100，会导致 APP 无法检测升级失败而长时间停留在升级界面。
            tuya_robot_upgrade_progress_report(report_percent);
        }
    }
    return OPRT_OK;
}

/**
 * @brief  OTA 前检查设备状态是否符合升级的回调
 * @param  [TY_SDK_FW_UG_T] *fw
 * @return [INT_T] TI_UPGRD_STAT_S
 */
INT_T ty_dev_upgrade_pre_check_cb(IN CONST FW_UG_S* fw)
{
#define BATTERY_CHECK_THREAD 30 //升级电量限制30%
    static unsigned char tus_download_error_cnt = 0;
    /*开发者可以在此处判断是否符合升级条件，如电量、机器状态等*/
    char battery_percentage = 15;
    if (battery_percentage < BATTERY_CHECK_THREAD) { //电量不足
        tus_download_error_cnt++; //这里只是测试用，将错误码循环上报一次，开发者根据自身的条件来上报对应的错误码
        if (tus_download_error_cnt >= 10) {
            tus_download_error_cnt = 0;
        }
        return TUS_DOWNLOAD_ERROR_UNKONW + tus_download_error_cnt;
    }
    PR_DEBUG("Upgrade check OK");
    /*其他升级条件由开发者自己添加*/
    return TUS_RD;
}

/**
 * @brief  OTA SDK回调函数
 * @param  [TY_SDK_FW_UG_T] *fw
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
INT_T ty_user_upgrade_inform_cb(IN CONST FW_UG_S* fw)
{
    PR_DEBUG("Rev Upgrade Info");
    PR_DEBUG("fw->fw_url:%s", fw->fw_url);
    PR_DEBUG("fw->sw_ver:%s", fw->sw_ver);
    PR_DEBUG("fw->file_size:%u", fw->file_size);

    //注册下载数据&下载结果回调
    OPERATE_RET ret = tuya_iot_upgrade_gw_notify(fw, __ty_user_get_file_data_cb, __ty_user_upgrade_notify_cb, NULL, true, 0);
    if (OPRT_OK != ret) {
        PR_DEBUG("ty_user_upgrade_inform_cb register ty_user_upgrade_inform & __ty_user_upgrade_notify %d", ret);
        return OPRT_COM_ERROR;
    }

    return 0;
}
