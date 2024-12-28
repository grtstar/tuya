#ifndef __TUYA_BLUEZ_DEF_H
#define __TUYA_BLUEZ_DEF_H

#include <stdint.h>
#include <stdbool.h>
#include "tkl_bluetooth.h"
#ifdef __cplusplus
extern "C" {
#endif

/* LE Error Code */
#define LE_SUCCESS       0x00
#define LE_INVALID_PARAM -0x01
#define LE_COM_ERROR     -0x02
#define LE_MALLOC_ERROR  -0x03

#define LE_OPEN_ERROR    -0x10
#define LE_READ_ERROR    -0x11
#define LE_WRITE_ERROR   -0x12

#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void *)0)
#endif
#endif

typedef enum {
    LE_SERVICE_UNKNOWN                    = 0x0000,   /**< Unknown Type */
    LE_SERVICE_PRIMARY                    = 0x2800,   /**< Primary Service */
    LE_SERVICE_SECONDARY                  = 0x2801,   /**< Secondary Service */
    LE_SERVICE_INCLUDE                    = 0x2802,   /**< Include */
    LE_SERVICE_CHARACTERISTIC             = 0x2803,   /**< Characteristic */
} le_service_type_e;

typedef enum {
    LE_GATT_CHR_PROP_BROADCAST            = 0x01, /**< If set, permits broadcasts of the Characteristic Value using Server Characteristic Configuration Descriptor. */
    LE_GATT_CHR_PROP_READ                 = 0x02, /**< If set, permits reads of the Characteristic Value */
    LE_GATT_CHR_PROP_WRITE_NO_RSP         = 0x04, /**< If set, permit writes of the Characteristic Value without response */
    LE_GATT_CHR_PROP_WRITE                = 0x08, /**< If set, permits writes of the Characteristic Value with response */
    LE_GATT_CHR_PROP_NOTIFY               = 0x10, /**< If set, permits notifications of a Characteristic Value without acknowledgment */
    LE_GATT_CHR_PROP_INDICATE             = 0x20, /**< If set, permits indications of a Characteristic Value with acknowledgment */
    LE_GATT_CHR_PROP_WRITE_AUTHEN_SIGNED  = 0x40, /**< If set, permits signed writes to the Characteristic Value */
    LE_GATT_CHR_PROP_EXT_PROP             = 0x80, /**< If set, additional characteristic properties are defined in the Characteristic */
} le_gatt_chr_prop_e;

typedef struct {
    uint16_t min_interval;
    uint16_t max_interval;
    uint8_t  advtype;
    uint8_t  own_bdaddr_type;
    uint8_t  direct_bdaddr_type;
    uint8_t  direct_bdaddr[6];
    uint8_t  channel_map;
    uint8_t  filter;
} le_set_adv_params_t;

typedef struct {
    uint8_t uuid[40];
    uint8_t property;
} le_gatt_characteristic_t;

typedef struct {
    uint16_t                  uuid;
    le_service_type_e         type;
    uint8_t                   chr_num;
    le_gatt_characteristic_t  *chr;
} le_gatt_service_t;

#ifdef __cplusplus
}
#endif

#endif