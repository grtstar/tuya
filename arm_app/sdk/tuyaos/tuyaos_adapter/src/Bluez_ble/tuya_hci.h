#ifndef __TUYA_HCI_H__
#define __TUYA_HCI_H__

#include "tuya_bluez_def.h"

#ifdef __cplusplus
extern "C" {
#endif

int tuya_hci_le_set_adv_params(uint16_t min_interval, uint16_t max_interval, uint8_t  advtype);

int tuya_hci_le_set_adv_enable(bool enable);

int tuya_hci_le_set_adv_data(uint8_t *data, uint8_t len);

int tuya_hci_le_set_scan_rsp_data(uint8_t *data, uint8_t len);

#ifdef __cplusplus
}
#endif

#endif