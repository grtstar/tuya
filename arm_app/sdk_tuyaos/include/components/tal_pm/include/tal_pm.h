#ifndef _TAL_PM_H_
#define _TAL_PM_H_
#include <stddef.h>
#include <stdint.h>
#include "tuya_cloud_types.h"
#include "tkl_pm.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 取消一个已经注册到tal层一个电源管理设备的对象
 *
 * @param devname 设备名称

 * @return 0 设置成功 其它 设置失败
 */
OPERATE_RET tal_pm_dev_unregistor(CONST CHAR_T *devname);


/**
 * @brief 获取指定设备的描述信息。
 *
 * @param devname 设备名称
 *
 * @return TUYA_PM_DEV_DESC_T 设备描述对象
 */
TUYA_PM_DEV_DESC_T *tal_pm_get_dev_info(CONST CHAR_T *devname);


/**
 * @brief 获取在注册到tal框架中的pmu设备列表。
 *
 * @param 无
 *
 * @return tal_pmu_dev pmu设备列表头指针
 */
TUYA_PM_DEV_DESC_T *tal_pm_get_dev_list_head(void);


/**
 * @brief 设置设备电源输出电压。
 *
 * @param devname 设备名称
 * @param mV 输出电压，单位毫伏
 *
 * @return 0 设置成功 其它 设置失败
 */
OPERATE_RET tal_pm_set_voltage(CONST CHAR_T *devname, INT_T mV);


/**
 * @brief 获取设备电源当前输出电压。
 *
 * @param devname 设备名称
 *
 * @return 电压，单位毫伏
 */
OPERATE_RET tal_pm_get_voltage(CONST CHAR_T *devname);

/**
 * @brief 设置设备电源输出电流。
 *
 * @param devname 设备名称
 * @param mA 输出电压，单位毫安
 *
 * @return 0 设置成功 其它 设置失败
 */
OPERATE_RET tal_pm_set_current(CONST CHAR_T *devname,INT_T mA);

/**
 * @brief 获取设备电源当前输出电流。
 *
 * @param devname 设备名称
 *
 * @return 电压，单位毫安
 */
OPERATE_RET tal_pm_get_current(CONST CHAR_T *devname);

/**
 * @brief 使能设备电源功能。
 *
 * @param devname 设备名称
 * @param lp_en 低功耗是否关闭
 *
 * @return 0 设置成功 其它 设置失败
 */
OPERATE_RET tal_pm_enable(CONST CHAR_T *devname,INT_T lp_en);

/**
 * @brief 关闭设备电源功能。
 *
 * @param devname 设备名称
 *
 * @return 0 设置成功 其它 设置失败
 */
OPERATE_RET tal_pm_disable(CONST CHAR_T *devname);

/**
 * @brief 获取设备电源功能是否使能。
 *
 * @param devname 设备名称
 * @param status 返回状态，TRUE enable,FALSE disable
 *
 * @return 1 打开；0 关闭
 */
OPERATE_RET tal_pm_is_enable(CONST CHAR_T *devname,BOOL_T *status);


/**
 * @brief 关闭系统电源，设备关机。
 *
 * @param devname 设备名称
 *
 * @return 1 打开；0 关闭
 */
OPERATE_RET tal_pm_power_off(CONST CHAR_T *devname);


/**
 * @brief 重启设备，该设备可以是整个系统，也可以是某个单独管理的设备。
 *
 * @param devname 设备名称
 *
 * @return 0 设置成功 其它 设置失败
 */
OPERATE_RET tal_pm_reset(CONST CHAR_T *devname);

/**
 * @brief 设备的ioctl接口，这个接口是为了给设备提供了扩展的接口
 *
 * @param devname  设备名称
 * @param ctl_cmd  控制命令
 * @param param    输入参数指针
 * @return 0 设置成功 其它 设置失败
 */
OPERATE_RET tal_pm_ioctl(CONST CHAR_T *devname,INT_T ctl_cmd,void *param);

/**
 * @brief 打印出当前平台支持的电源管理设备名称
 *
 * @param 无
 * @return 无
 */
VOID tal_pm_devname_dump(VOID);

#ifdef __cplusplus
}
#endif

#endif