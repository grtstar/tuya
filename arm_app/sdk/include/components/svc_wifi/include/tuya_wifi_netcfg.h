#ifndef __TUYA_WIFI_NETCFG_H__
#define __TUYA_WIFI_NETCFG_H__
#include "tuya_cloud_wifi_defs.h"
#include "tuya_wifi_link.h"
#include "tuya_devos_data.h"

#if defined ENABLE_DEVICE_DATA_COLLECT && (ENABLE_DEVICE_DATA_COLLECT==1)
#define CAPTURE_NETCFG_DATA(type, step)                                                                                                                                                        \
    do                                                                                                                                                                                         \
    {                                                                                                                                                                                          \
        OPERATE_RET rt = OPRT_OK;                                                                                                                                                              \
        TUYA_CALL_ERR_LOG(ty_devos_update_netcfg_data(tal_time_get_posix(), TY_DEVOS_NET_CONFIG_DATA, (uint8_t)type, get_wifi_config_params()->start_mode, get_wifi_config_params()->mthd, step, tal_time_get_posix())); \
    } while (0)
#else
#define CAPTURE_NETCFG_DATA(type, step) do{}while(0)
#endif

/**
 * @brief Set device AP config
 *
 * @param[out] ssid SSID to connect
 * @param[out] passwd Password of AP

 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET set_user_def_ap_if(IN CONST CHAR_T *ssid, IN CONST CHAR_T *passwd);

/**
 * @brief Get device AP config
 *
 * @param[out] ssid SSID to connect
 * @param[out] passwd Password of AP

 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET get_dev_ap_if(OUT CHAR_T* ssid, OUT CHAR_T* passwd);

/**
 * @brief Set maximum of clients that are allowed to connect this device
 *
 * @param[in] max_conn Max count
 */
VOID set_max_sta_conn(IN CONST UCHAR_T max_conn);

/**
 * @brief Set device network info
 *
 * @param[in] ip IP addr.
 * @param[in] gw Gateway addr.
 * @param[in] mask Network mask
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
VOID set_nw_ip(IN CONST CHAR_T* ip, IN CONST CHAR_T* mask, IN CONST CHAR_T* gw);


/**
 * @brief Set wifi netcfg timeout value in seconds
 *
 * @param[in] timeout_s time out value of netcfg.
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */

VOID set_wf_netcfg_timeout(UINT_T timeout_s);

/**
 * @brief Configure network with user defined params
 *
 * @param[in] ssid SSID to connect
 * @param[in] passwd Password of AP
 * @param[in] token The token
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_wifi_user_cfg(IN CONST CHAR_T *ssid, IN CONST CHAR_T *passwd, IN CONST CHAR_T *token);

/**
 * @brief change wifi ssid&passwd
 *
 * @param[in] ssid SSID to connect
 * @param[in] passwd Password of AP
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_wifi_modify_and_conn(IN CONST CHAR_T *ssid, IN CONST CHAR_T *passwd);

/**
 * @brief get nc_type for application
 *
 * @param[out] nc_type nc_type for wifi netcfg
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_wifi_fast_get_nc_type(GW_WF_NWC_FAST_STAT_T *nc_type);

/**
 * @brief set ap cfg infomation
 *
 * @param[in] cfg ap cfg infomation
 *
 * @return void
 */
VOID set_ap_cfg_info(WF_AP_CFG_IF_S *cfg);

/**
 * @brief get wifi scan list
 *
 * @param[in/out] wifi_list: wifi_list buffer for scan result
 * @param[out] wifi_list_size: wifi_list buffer size
 * @param[out] max_cnt: max count for wifi list
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_get_wifi_scan_list(CHAR_T* wifi_list, uint16_t wifi_list_size, uint16_t max_cnt);

/**
 * @brief check if need start ap or not
 *
 * @return TRUE: need to start ap. FALSE: no need to start ap
 */
BOOL_T tuya_need_start_ap_by_netcfg(VOID_T);

/**
 * @brief report netcfg status
 *
 * @param[in] type: 0-report 1-query
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET  tuya_ap_rept_netcfg_stat(IN UINT_T type);

/**
 * @brief start device ap
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_wf_start_device_softap(VOID_T);

#endif

