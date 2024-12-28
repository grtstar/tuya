#ifndef __TUYA_BLUEZ_API_H__
#define __TUYA_BLUEZ_API_H__

#include "tuya_bluez_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief init bluez
 * 
 * @return 0: success, other: fail
 */
int tuya_bluez_init(void);

/**
 * @brief deinit bluez
 * 
 * @return 0: success, other: fail
 */
int tuya_bluez_deinit(void);

/**
 * @brief set advertising parameters
 * 
 * @param params advertising parameters
 * 
 * @return 0: success, other: fail
 */
int tuya_bluez_le_set_adv_params(le_set_adv_params_t *params);

/**
 * @brief set advertising enable
 * 
 * @param enable true: enable, false: disable
 * 
 * @return 0: success, other: fail
 */
int tuya_bluez_le_set_adv_enable(bool enable);

/**
 * @brief set advertising data
 * 
 * @param data advertising data
 * @param len  advertising data length
 * 
 * @return 0: success, other: fail
 */
int tuya_bluez_le_set_adv_data(uint8_t *data, uint8_t len);

/**
 * @brief set scan response data
 * 
 * @param data scan response data
 * @param len  scan response data length
 * 
 * @return 0: success, other: fail
 */
int tuya_bluez_le_set_scan_rsp_data(uint8_t *data, uint8_t len);

/**
 * @brief add gatt service
 * 
 * @param service      gatt service
 * @param service_num  gatt service number
 * 
 * @return 0: success, other: fail
 */
int tuya_bluez_le_add_gatt_service(le_gatt_service_t *service, uint8_t service_num);

/**
 * @brief notify an attribute value
 * 
 * @param uuid         characteristic uuid
 * @param value        value
 * @param len          value length
 * 
 * @return 0: success, other: fail
 */
int tuya_bluez_le_gatts_value_notify(uint16_t uuid, uint8_t *value, uint16_t len);

/**
 * @brief register connect event callback
 */
void tuya_bluez_le_register_connect_event(void(*cb)(int status));

/**
 * @brief register write request event callback
 */
void tuya_bluez_le_register_write_req_event(void(*cb)(uint16_t uuid, uint8_t *data, uint8_t len));

#ifdef __cplusplus 
}
#endif

#endif