/*********************************************************************************
 *Copyright(C),2015-2020, TUYA www.tuya.comm
 *FileName:    tuya_sdk_app_reset_demo
 *brief：1.This article introduces the interface for removing devices through
           the APP or local calls, as well as the SDK result callbacks.
         2.It also covers the SDK interface callbacks after device activation.
 **********************************************************************************/
#include <stdio.h>
#include <string.h>

#include "tuya_error_code.h"
#include "tuya_iot_wifi_api.h"
#include "utilities/uni_log.h"

/**
 * @brief  APP或者本地移除设备SDK结果回调
 * @param  [GW_RESET_TYPE_E] type -> 重置的原因
 * @return [*]
 */
VOID ty_sdk_app_reset_cb(GW_RESET_TYPE_E type)
{
    PR_DEBUG(" gw reset success. please restart the %d\n", type);
    switch (type) {
    case GW_LOCAL_RESET_FACTORY: //本地恢复出厂设置
        //清除必要的业务数据，注意不必删除db文件
        //注意需要重启设备
        break;
    case GW_REMOTE_RESET_FACTORY: // APP下发移除设备并清除数据
        //清除必要的业务数据，注意不必删除db文件
        //注意需要重启设备
        break;
    case GW_LOCAL_UNACTIVE: //本地重置
        //取消清扫任务控制
        //开发者自行业务上做逻辑，如灯效，声音等
        //注意需要重启设备
        break;
    case GW_REMOTE_UNACTIVE: // APP重置
        //取消清扫任务控制
        //开发者自行业务上做逻辑，如灯效，声音等
        //注意需要重启设备
        break;
    case GW_RESET_DATA_FACTORY: // 激活时数据重置
        /**
         * App 执行 `解绑并清除数据` 重置时，或者重新配网的 App 账户与原 App 账户不是同一个家庭账号，则
         * 激活时会收到该类型。提醒开发者清除下本地数据，如果您在重新配网前未做过数据清除。
         */
        break;

    default:
        break;
    }
    return;
    /***开发者根据收到的重置类型，通过事件的形式发送出去，在业务上建立独立任务处理。不要在回调里做复杂事情****/
}

/**
 * @brief  本地普通重置
 * @param  [*]
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET test_local_unactive(void)
{
    OPERATE_RET rt = OPRT_OK;
    TUYA_CALL_ERR_LOG(tuya_iot_wf_gw_unactive()); //开发者可以使用按键组合的方式，将设备移除，该接口不会清除云端信息，
    return rt;                                    //再次配网后，设备id不变，清扫记录、楼层地图依然存在。
}

/**
 * @brief  本地恢复出厂重置
 * @param  [*]
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET test_local_reset(void)
{
    OPERATE_RET rt = OPRT_OK;
    TUYA_CALL_ERR_LOG(tuya_iot_wf_gw_reset()); //开发者可以使用按键组合的方式，将设备恢复初始状态，该接口会清除云端保存的该设备信息，
    return rt;                                 //如清扫记录、楼层地图、设备id等。注意本地也要清除相关信息。
}

/**
 * @brief  设备激活状态
 * @param  [in] GW_STATUS_E 激活状态
 * @return [*]
 */
VOID ty_sdk_dev_status_changed_cb(IN CONST GW_STATUS_E status)
{
    PR_DEBUG("sdk internal restart request. please restart the %d\n", status);
    if (status == GW_ACTIVED) { //激活成功
        //开发者可以根据激活状态来判断设备是否成功激活
    }
    /***开发者可以在该回调中获取设备激活状态****/
}
