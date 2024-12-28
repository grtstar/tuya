#ifndef __TUYA_GATT_H__
#define __TUYA_GATT_H__

#include "tuya_bluez_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief init gatt
 * 
 * @return 0: success, other: fail
 */
int tuya_gatt_init(void);

/**
 * @brief register gatt service
 * 
 * @param uuid service uuid
 * 
 * @return 0: success, other: fail
 */
int tuya_gatt_register_service(uint16_t uuid);

/**
 * @brief register gatt characteristic
 * 
 * @param svc_uuid   service uuid
 * @param chr_uuid   characteristic uuid
 * @param props      characteristic properties
 * @param value      characteristic value
 * @param vlen       characteristic value length
 * @param desc_uuid  descriptor uuid
 * @param desc_props descriptor properties
 * 
 * @return 0: success, other: fail
 */
int tuya_gatt_register_characteristic(uint16_t svc_uuid, const uint8_t *chr_uuid, uint8_t props, uint16_t desc_uuid, uint8_t desc_props);

/**
 * @brief notify an attribute value
 * 
 * @param uuid       characteristic uuid
 * @param data       data
 * @param len        data length
 *
 * @return 0: success, other: fail
 */
int tuya_gatt_server_send_characteristic_notification(uint16_t uuid, uint8_t *data, uint8_t len);

/**
 * @brief connect event
 */
void tuya_gatt_register_connect_event(void(*cb)(int status));

/**
 * @brief gatt write request event
 */
void tuya_gatt_register_write_req_event(void(*cb)(uint16_t uuid, uint8_t *data, uint8_t len));

#ifdef __cplusplus
}
#endif

#endif