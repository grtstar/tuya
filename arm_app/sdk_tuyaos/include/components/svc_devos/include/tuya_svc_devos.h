/**
 * @file tuya_svc_devos.h
 * @brief DevOS initialization and schedule
 * @version 0.1
 * @date 2019-04-25
 *
 * @copyright Copyright 2019-2021 Tuya Inc. All Rights Reserved.
 */

#ifndef __TUYA_SVC_DEVOS_H__
#define __TUYA_SVC_DEVOS_H__

#include "tuya_cloud_com_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EVENT_DEVOS_STATE_CHANGE "devos.stat.chg" // devos state changed

/* netcfg state */
typedef enum {
    NW_CFG_STAGE_NETCFG,            // current in netcfg stage
    NW_CFG_STAGE_PROXY_ACTIVE,      // current in proxy activiate stage
    NW_CFG_STAGE_CONN_AP,           // current in connect ap stage
    NW_CFG_STAGE_CLOUD_ACTIVE       // current in active stage
} DEVOS_NW_CFG_STAGE_E;

/* devos state machine */
typedef enum {
    DEVOS_STATE_INIT,            // device is inited
    DEVOS_STATE_UNREGISTERED,    // device is not activated
    DEVOS_STATE_REGISTERING,     // token/meta is got, start to activate
    DEVOS_STATE_ACTIVATED,       // device is activated and full-functional
    DEVOS_STATE_UPGRADING,       // device is in OTA

    DEVOS_STATE_MAX
} DEVOS_STATE_E;

/**
 * @brief Init param of IoT device
 */
typedef struct {
    /** device need factory reset first */
    BOOL_T factory_reset;
    /** device ability*/
    GW_ABI abi;
    /** device ota channel */
    DEV_TYPE_T tp;

    /** The key of firmware, assigned by TUYA */
    CHAR_T *firmware_key;
    /** The key of product, assigned by TUYA */
    CHAR_T *product_key;
    /** The version of firmware, format is "x.y.z" */
    CHAR_T *sw_ver;

    /** The count of attach modules */
    UINT_T attr_num;
    /** The attributes of attach modules */
    GW_ATTACH_ATTR_T *attrs;
} devos_init_t;

/**
 * @brief Init device
 *
 * @param[in] cfg device configuration
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_svc_devos_init(IN devos_init_t *cfg);

/**
 * @brief Get current devos state
 *
 * @return see DEVOS_STATE_E
 */
DEVOS_STATE_E tuya_svc_devos_get_state(VOID);

/**
 * @brief Set current devos state (dispatch event)
 *
 * @param[in] state see DEVOS_STATE_E
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_svc_devos_set_state(IN DEVOS_STATE_E state);

/**
 * @brief Set current devos netcfg state
 *
 * @param[in] state see DEVOS_NW_CFG_STAGE_E
 * @param[in] status, 0-success, others-error
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_svc_devos_set_netcfg_stat(IN DEVOS_NW_CFG_STAGE_E stage, IN UINT_T status);

/**
 * @brief Get current devos netcfg state
 *
 * @return netcfg state info string
 */
CHAR_T* tuya_svc_devos_get_netcfg_stat(UINT8_T type);

#ifdef __cplusplus
}
#endif

#endif //__TUYA_SVC_DEVOS_H__

