/*********************************************************************************
  *Copyright(C),2015-2020, TUYA www.tuya.comm
  *FileName:    tuya_sdk_floor_map_demo
**********************************************************************************/

#ifndef INCLUDE_TUYA_SDK_FLOOR_MAP_DEMO_H_
#define INCLUDE_TUYA_SDK_FLOOR_MAP_DEMO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include<stdbool.h>
/**
 * @brief  清扫记录所支持的清扫模式
 */
typedef enum {
    CLEAN_MODE_SMART = 0,       // 全屋清扫
    CLEAN_MODE_SELECT_ROOM,     // 选区清扫
    CLEAN_MODE_POSE,            // 定点清扫
    CLEAN_MODE_ZONE,            // 划区清扫
    CLEAN_MODE_STAND_POSE,      // 原地定点清扫
    CLEAN_MODE_SMART_PART,      // 智能分区清扫  
    CLEAN_MODE_SMART_PART_AREA, // 智能分区区域清扫
    CLEAN_MODE_DEPTH_GLOBAL,    // 深度全局清扫
    CLEAN_MODE_EDGE,            // 沿边清扫
} ROBOT_RECORDS_CLRAN_MODE_E;

/**
 * @brief  清扫记录标识的清扫结束原因
 */
typedef enum {
    CLEAN_RESULT_ERR = 0, // 异常
    CLEAN_RESULT_OK,      // 正常
    CLEAN_RESULT_MAX
} ROBOT_CLEAN_RESULT_E;

/**
 * @brief  机器启动方式
 */
typedef enum {
    START_METHOD_REMOTE = 0, // 物理遥控器启动
    START_METHOD_APP,        // APP启动
    START_METHOD_RESERVED,   // 预约任务启动
    START_METHOD_BUTTON,     // 按键启动
    START_METHOD_SELF,       // 自启动（二次回充等）
    START_METHOD_MAX,
} ROBOT_START_METHOD_E;

/**
 * @brief  楼层地图的功能测试入口
 * @param  [*]
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET multi_map_oper(VOID);

/**
 * @brief  扫地机楼层地图上报服务启动
 * @param  [*]
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET ty_user_sweeper_floor_map_init(void);
#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_TUYA_SDK_FLOOR_MAP_DEMO_H_ */