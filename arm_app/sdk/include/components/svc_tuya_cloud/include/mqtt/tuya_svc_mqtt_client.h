/**
 * @file tuya_svc_mqtt_client.h
 * @brief
 * @version 0.1
 * @date 2022-08-18
 *
 * @copyright Copyright (c) 2021-2022 Tuya Inc. All Rights Reserved.
 *
 * Permission is hereby granted, to any person obtaining a copy of this software and
 * associated documentation files (the "Software"), Under the premise of complying
 * with the license of the third-party open source software contained in the software,
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software.
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 */

#ifndef __TUYA_SVC_MQTT_CLIENT_H__
#define __TUYA_SVC_MQTT_CLIENT_H__

#include "tuya_cloud_types.h"
#include "tuya_svc_netmgr.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef QOS1_PUBACK_TIMEOUT
#define QOS1_PUBACK_TIMEOUT 3 // qos1 publish respond timeout(s)
#endif

#ifndef QOS1_PUBLISH_SLOT_CNT
#define QOS1_PUBLISH_SLOT_CNT 8 // slot count waiting for puback(new publish will be refused)
#endif

#ifndef QOS1_PUBACK_TIMEOUT_LIMIT
#define QOS1_PUBACK_TIMEOUT_LIMIT 3 // continuous timeout count for qos1 publish(mqtt will close and reconnect)
#endif

#ifndef MAX_RANDOM_DELAY_BEFORE_RECONNECT
#define MAX_RANDOM_DELAY_BEFORE_RECONNECT (5*60) // max random delay when connection broken(s)
#endif

#ifndef MQTT_HEATBEAT_INTERVAL
#define MQTT_HEATBEAT_INTERVAL (60) // mqtt heartbeat interval(s)
#endif

#ifndef STACK_SIZE_MQTT_CLIENT
#define STACK_SIZE_MQTT_CLIENT (4*1024) // stack size of mqtt client(Byte)
#endif

#ifndef MQTT_RECV_BUF_SIZE
#define MQTT_RECV_BUF_SIZE (512)    // size of mqtt recv buffer(Byte)
#endif

#ifndef MQTT_MAX_MESSGAE_SIZE
#define MQTT_MAX_MESSGAE_SIZE (4096) // max size of mqtt msg(Byte, RTOS only)
#endif

#ifndef MQTT_RECV_SELECT_TIMEOUT
#define MQTT_RECV_SELECT_TIMEOUT (3000) // powser saving need consider (ms)
#endif

#define MAX_BACKOFF_COUNT    (8) //backoff array size
typedef VOID* MQTT_HANDLE; //Definition of MQTT client handler
#define MQTT_PING_ACK_RECV "mqtt.ping.recv"
/**
 * @brief Callback when recv msg from broker
 *
 * @param[in] topic Topic that msg is recved from
 * @param[in] data Buffer of msg content
 * @param[in] len Length of msg content
 */
typedef VOID (*CB_MQTT_DATA_RECV)(CHAR_T *topic, BYTE_T *data, UINT_T len);

/**
 * @brief Definition of mqtt client context
 */
typedef struct {
    /** domain of MQTT broker */
    CHAR_T *broker_domain;
    /** port of MQTT broker */
    UINT_T broker_port;

    /** default topic subscribed from MQTT broker */
    CHAR_T *subcribe_topic;
    /** client id */
    CHAR_T *client_id;
    /** user name */
    CHAR_T *user_name;
    /** password */
    CHAR_T *passwd;
    /** heartbeat interval(unit sec), 0 indicates default (MQTT_HEATBEAT_INTERVAL)*/
    UINT_T heartbeat;

    /** callback when msg recved from broker */
    CB_MQTT_DATA_RECV cb_data_recv;
} mqtt_ctx_t;

/**
 * @brief Backoff strategy for reconnection when broken
 */
typedef enum {
    BACKOFF_RANDOM,     // default(random % MAX_RANDOM_DELAY_BEFORE_RECONNECT)
    BACKOFF_LOOP_CYCLE, // (1,2,3) ==> 1->2->3->1->2->3
    BACKOFF_LOOP_LAST   // (1,2,3) ==> 1->2->3->3->3->3
} backoff_e;

typedef struct {
    backoff_e type;
    union {
        UINT_T random; //override MAX_RANDOM_DELAY_BEFORE_RECONNECT(unit s)
        struct {
            UINT_T arr[MAX_BACKOFF_COUNT]; //(unit s)
            BYTE_T count;
        }loop;
    }data;
} mqtt_backoff_t;

/**
 * @brief Definition of mqtt cfg type
 */
typedef enum {
    MQTT_CFG_LINKAGE,       // linkage used for connection (netmgr_linkage_t*) (default: current active linkage)
    MQTT_CFG_HEARTBEAT,     // heartbeat interval (UINT:s) (default: MQTT_HEATBEAT_INTERVAL)
    MQTT_CFG_SLEEP_WITHOUT_PERMIT, // (UINT:s)
    MQTT_CFG_SOCKET_FD, // get mqtt client socket fd (readonly) (INT_T)
    MQTT_CFG_DISCONNECT_WHEN_STOP, // disconnect from broker immediately when stop is called (BOOL_T) (default: true)
    MQTT_CFG_BACKOFF_STRATEGY, // backoff strategy when connection broken (mqtt_backoff_t*)

    MQTT_CFG_MAX
} mqtt_cfg_e;

/**
 * @brief  Async callback that indicates QOS1 publish success or not.
 *
 * @param[in] op_ret OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 * @param[in] ctx Private data
 */
typedef VOID (*CB_MQTT_PUB_INFORM)(OPERATE_RET op_ret, VOID *ctx);

/**
 * @brief Definition of mqtt msg to publish
 */
typedef struct {
    /** topic that the msg will be published */
    CHAR_T *publish_topic;
    /** QOS level (only 0 and 1) */
    BYTE_T qos;
    /** buffer of msg content */
    BYTE_T *data;
    /** length of msg content */
    UINT_T len;
    /** timeout(unit sec) of QOS-1 ACK, 0 indicates default (QOS1_PUBACK_TIMEOUT) */
    UINT_T timeout;
    /** async callback that indicates QOS1 msg publish success or not. */
    CB_MQTT_PUB_INFORM cb_pub_inform;
    /** private data */
    VOID *ctx;
} mqtt_msg_t;

/**
 * @brief Definition of mqtt subscribe
 */
typedef struct {
    /** topics to subscribe */
    CHAR_T **topics;
    /** count of topics to subscribe */
    BYTE_T cnt;
    /** callback when MQTT msgs recv. If NULL, msgs will transfer to default cb of MQTT client */
    CB_MQTT_DATA_RECV cb_data_recv;
    /** whether or not topics need re-subscribe when mqtt broker connected */
    BOOL_T auto_resub;
} mqtt_subscribe_t;

/**
 * @brief MQTT client is connected to broker
 */
typedef VOID (*CB_MQTT_CONNECTED)(VOID);
/**
 * @brief MQTT client is disconnected from broker
 */
typedef VOID (*CB_MQTT_DISCONNECTED)(VOID);
/**
 * @brief MQTT client is not permited to connect to broker
 *
 * @param[in] deny_times How many times this MQTT client is denied by broker
 */
typedef VOID (*CB_MQTT_CONNECT_DENY)(BYTE_T deny_times);

/**
 * @brief Create mqtt client
 *
 * @param[in] ctx Context of mqtt client, see @ref mqtt_ctx_t for details
 * @param[out] handle Handle of mqtt client
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_svc_mqtt_client_create(mqtt_ctx_t *ctx, MQTT_HANDLE *handle);

/**
 * @brief Close and destroy mqtt client
 *
 * @param[in] handle Handle of mqtt client
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_svc_mqtt_client_destroy(MQTT_HANDLE *handle);

/**
 * @brief Start mqtt client (or reconnect to broker when disconnected)
 *
 * @param[in] handle Handle of mqtt client
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_svc_mqtt_client_start(MQTT_HANDLE *handle);

/**
 * @brief Stop mqtt client (disconnect from mqtt broker)
 *
 * @param[in] handle Handle of mqtt client
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_svc_mqtt_client_stop(MQTT_HANDLE *handle);

/**
 * @brief Restart mqtt client (or reconnect to broker when disconnected)
 *
 * @param[in] handle Handle of mqtt client
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_svc_mqtt_client_restart(MQTT_HANDLE *handle);

/**
 * @brief  Register event handers of MQTT client
 *
 * @param[in] handle Handler of MQTT client
 * @param[in] cb_connected Hander when MQTT client is connected to broker
 * @param[in] cb_disconnected Hander when MQTT client is disconnected to broker
 * @param[in] cb_connect_deny Hander when MQTT client is not permited to connect to broker
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_svc_mqtt_client_register_cb(MQTT_HANDLE handle, CB_MQTT_CONNECTED cb_connected,
                                             CB_MQTT_DISCONNECTED cb_disconnected, CB_MQTT_CONNECT_DENY cb_connect_deny);

/**
 * @brief Mqtt broker is connected or not
 *
 * @param[in] handle Handle of mqtt client
 *
 * @return True for connected, False for disconnected
 */
BOOL_T tuya_svc_mqtt_client_is_connected(MQTT_HANDLE *handle);

/**
 * @brief Get cfg of mqtt client
 *
 * @param[in] handle Handle of mqtt client
 * @param[in] cfg Cfg type, see @ref mqtt_cfg_e for details
 * @param[in] data Cfg data
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_svc_mqtt_client_set_cfg(MQTT_HANDLE *handle, mqtt_cfg_e cfg, VOID* data);

/**
 * @brief Get cfg of mqtt client
 *
 * @param[in] handle Handle of mqtt client
 * @param[in] cfg Cfg type, see @ref mqtt_cfg_e for details
 * @param[out] data Cfg data
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_svc_mqtt_client_get_cfg(MQTT_HANDLE *handle, mqtt_cfg_e cfg, VOID* data);

/**
 * @brief Publish mqtt msg to broker
 *
 * @param[in] handle Handle of mqtt client
 * @param[in] msg see @ref mqtt_msg_t for details
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_svc_mqtt_client_publish(MQTT_HANDLE *handle, mqtt_msg_t* msg);

/**
 * @brief Subcribe topics within one mqtt request
 *
 * @param[in] handle Handle of mqtt client
 * @param[in] sub see @ref mqtt_subscribe_t for details
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_svc_mqtt_client_subscribe(MQTT_HANDLE *handle, mqtt_subscribe_t* sub);

/**
 * @brief Unsubcribe topics within one mqtt request
 *
 * @param[in] handle Handle of mqtt client
 * @param[in] sub see @ref mqtt_subscribe_t for details
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_svc_mqtt_client_unsubscribe(MQTT_HANDLE *handle, mqtt_subscribe_t* sub);

#ifdef __cplusplus
}
#endif

#endif //__TUYA_SVC_MQTT_CLIENT_H__
