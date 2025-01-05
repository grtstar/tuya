/**
* @file      tuya_ipc_moto_mqtt.h
* @brief     ipc moto mqtt api
* @version   1.0
* @date      2021-11-19
*
* copyright  Copyright (c) tuya.inc 2021
*/
#ifndef __TUYA_MOTO_MQTT_H__
#define __TUYA_MOTO_MQTT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "tuya_cloud_com_defs.h"
#include "tuya_cloud_types.h"
#include "cJSON.h"

#define PUBLIC
#define PRIVATE

#define TOPIC_LEN 64

#define MODULE_NAME_LEN 16
#define MAX_MODULE 2

/**
* @brief      process moto message callback
* @param[in]  msg    moto message
* @return     VOID
*/
typedef VOID (*PROCESS_MOTO_MSG_CB)(IN ty_cJSON *msg);

/**
* @struct MOTO_MODULE_CB_T
* @brief  moto module callback
*/
typedef struct {
    CHAR_T module_name[MODULE_NAME_LEN];
    PROCESS_MOTO_MSG_CB cb;
}MOTO_MODULE_CB_T;

typedef struct {
PRIVATE
    BOOL_T      inited;
    BOOL_T      booked;
    CHAR_T      dev_id[GW_ID_LEN+1];
    CHAR_T      topic[TOPIC_LEN];

PUBLIC
    OPERATE_RET (*book_topic)(VOID);
    OPERATE_RET (*book_topic_force)(VOID);
    OPERATE_RET (*publish_mqtt_msg)(IN CONST CHAR_T *moto_id, IN CONST UINT_T protocol, IN CONST BYTE_T *p_data);
}TUYA_IPC_MOTO_MQTT_T;

/**
* @brief      get moto mqtt instance
* @return     TUYA_IPC_MOTO_MQTT_T*
*/
TUYA_IPC_MOTO_MQTT_T* tuya_ipc_get_moto_mqtt_instance(VOID);

/**
* @brief      ipc moto mqtt init
* @return     OPERATE_RET
* - OPRT_OK   success
* - Others    failed
*/
OPERATE_RET tuya_ipc_moto_mqtt_init(VOID);

#ifdef __cplusplus
}
#endif

#endif

