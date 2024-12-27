 /*============================================================================
 *                                                                            *
 * Copyright (C) by Tuya Inc                                                  *
 * All rights reserved                                                        *
 *                                                                            *
 * @author  :   Linch                                                         *
 * @date    :   2020-07-07                                                    *
 * @brief   :                                                                 *
 * @log     :                                                                 *
 =============================================================================*/

#ifndef __WEBSOCKET_H__
#define __WEBSOCKET_H__

#ifdef __cplusplus
    extern "C" {
#endif

/*============================ INCLUDES ======================================*/
#include "tuya_iot_config.h"
#include "tuya_transporter.h"
#include "tal_thread.h"
#include "tuya_tls.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
typedef enum {
    WEBSOCKET_OK,
    WEBSOCKET_COM_ERROR,
    WEBSOCKET_MALLOC_FAILED,
    WEBSOCKET_INVALID_PARM,
} websocket_error_t;

typedef enum {
    WEBSOCKET_RECV_DATA_EVENT,
    WEBSOCKET_CONNECTED_EVENT,
    WEBSOCKET_DISCONNECT_EVENT,
} websocket_client_event_t;

typedef struct {
    websocket_client_event_t    event;
    uint8_t                    *data;
    uint32_t                    len;
} websocket_client_msg_t;


typedef void (*websocket_client_event_cb_t)(websocket_client_msg_t *msg, void *priv_data);

typedef struct {
    char                           *uri;        //! <ws/wss>://host[:port]/path
    char*                           scheme;
    char*                           host;
    int                             port;
    tuya_tcp_config_t               tcpConfig;
    void                           *priv_data;
    websocket_client_event_cb_t     event_cb;
} websocket_client_cfg_t;

typedef enum {
    WEBSOCKET_INIT,
    WEBSOCKET_CONNECT,
    WEBSOCKET_RECEIVE,
    WEBSOCKET_TIMEOUT,
    WEBSOCKET_PING,
    WEBSOCKET_WAIT,
} websocket_client_state_t;

#define WEBSOCKET_BUFFER_SIZE           1024

typedef struct {
    websocket_client_state_t        state;
    uint8_t                         run;
    uint8_t                         connected;
    uint8_t                         tls_enable;
    tuya_tls_hander                 tls_hander;
    websocket_client_cfg_t          cfg;
    char                           *hostname;
    char                           *path;
    uint16_t                        port;
    uint8_t                         buffer[WEBSOCKET_BUFFER_SIZE];
    uint16_t                        ping;
    uint16_t                        ping_tick;
    THREAD_HANDLE                   task;
    tuya_transporter_t              transporter;
} websocket_client_t;

typedef void *websocket_client_handle_t;

/*============================ PROTOTYPES ====================================*/
int websocket_client_init       (websocket_client_handle_t *handle, websocket_client_cfg_t *cfg);
int websocket_client_start      (websocket_client_handle_t handle, char *name);


int websocket_client_set_path(websocket_client_handle_t handle, char *path);

int websocket_client_send_bin   (websocket_client_handle_t handle, uint8_t *data, uint32_t data_len);
int websocket_client_send_text  (websocket_client_handle_t handle, uint8_t *data, uint32_t data_len);

int websocket_client_disconnect (websocket_client_handle_t handle);

int websocket_client_open(websocket_client_handle_t client, int connect_timeout_ms);

int websocket_client_poll(websocket_client_handle_t client, int timeout_ms);
int websocket_client_read(websocket_client_handle_t client, uint8_t *buffer, int len);    
void websocket_client_close(websocket_client_handle_t client);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
