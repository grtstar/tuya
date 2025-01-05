/*********************************************************************************
  *Copyright(C),2015-2020, TUYA www.tuya.comm
  *FileName:    tuya_sdk_rt_map_demo
**********************************************************************************/

#ifndef INCLUDE_TUYA_SDK_RT_MAP_DEMO_H_
#define INCLUDE_TUYA_SDK_RT_MAP_DEMO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include<stdbool.h>

/**
 * @brief  用户设置扫地机自定义名称
 * @param  [*]
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET ty_user_sweeper_set_customize_name(VOID);

/**
 * @brief  扫地机实时地图上传服务启动
 * @param  [*]
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET ty_user_sweeper_rt_map_init(void);

/**
 * @brief  实时地图及路径上传 event 回调具体处理
 * @param  [IN CONST channel] 连接哪里客户端
 * @param  [IN CONST SWEEPER_TRANSFER_EVENT_E] event 事件
 * @param  [IN CONST args] 回调数据
 * @return [INT]结果
 */
INT_T tuya_sweeper_event_cb(INT_T channel, IN CONST SWEEPER_TRANSFER_EVENT_E event, IN CONST PVOID_T args);

#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_TUYA_SDK_RT_MAP_DEMO_H_ */
