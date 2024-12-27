#ifndef __RTSP_CLIENT_H__
#define __RTSP_CLIENT_H__


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "rtsp.h"

#define RTSP_USERNAME_LEN 64
#define RTSP_PASSWORD_LEN 64
#define RTSP_NONCE_LEN    128
#define RTSP_REALM_LEN    128

typedef VOID* RTSP_CLIENT_HANDLE_T;


typedef enum {
    RTSP_CLIENT_STATUS_INIT = 0,         ///<initial status, no tcp connection yet
    RTSP_CLIENT_STATUS_CONNECTING,       ///<connecting status in async connect process
    RTSP_CLIENT_STATUS_CONNECTED,        ///<connect success
    RTSP_CLIENT_STATUS_CONNECT_FAIL,     ///<connect fail
    RTSP_CLIENT_STATUS_OPTIONS_SENT,     ///<OPTIONS request has been sent
    RTSP_CLIENT_STATUS_OPTIONS_RESP,     ///<OPTIONS response has been received
    RTSP_CLIENT_STATUS_DESCRIBE_SENT,    ///<DESCRIBE request has been sent
    RTSP_CLIENT_STATUS_DESCRIBE_RESP,    ///<DESCRIBE response has been received
    RTSP_CLIENT_STATUS_SETUP_VID_SENT,   ///<video SETUP request has been sent
    RTSP_CLIENT_STATUS_SETUP_VID_RESP,   ///<video SETUP response has been received
    RTSP_CLIENT_STATUS_SETUP_AUD_SENT,   ///<audio SETUP request has been sent
    RTSP_CLIENT_STATUS_SETUP_AUD_RESP,   ///<audio SETUP response has been received
    RTSP_CLIENT_STATUS_PLAY_SENT,        ///<PLAY request has been sent
    RTSP_CLIENT_STATUS_PLAY_RESP,        ///<PLAY response has been received
    RTSP_CLIENT_STATUS_PLAYING,          ///<streaming over rtp
    RTSP_CLIENT_STATUS_TEARDOWN_SENT,    ///<TEARDOWN request has been sent
    RTSP_CLIENT_STATUS_TEARDOWN_RESP,    ///<TEARDOWN response has been received
    RTSP_CLIENT_STATUS_STOP,             ///<the client will be removed

    RTSP_CLIENT_STATUS_NUM,
} RTSP_CLIENT_STATUS_E;

/**
 * @brief callback when status change
 * @param[in] handle returned by tuya_ipc_rtsp_client_start
 * @param[in] client status
 * @param[in] priv private data which will be put in the callback
 * @return error code
 */
typedef INT_T (*RTSP_CLIENT_STATUS_CHANGE_CB)(RTSP_CLIENT_HANDLE_T handle, RTSP_CLIENT_STATUS_E status, VOID* priv);

/**
 * @brief callback to put received data to upper application
 * @param[in] handle returned by tuya_ipc_rtsp_client_start
 * @param[in] pkt data packet
 * @param[in] priv private data which will be put in the callback
 * @return error code
 */
typedef INT_T (*RTSP_CLIENT_PUT_DATA_CB)(RTSP_CLIENT_HANDLE_T handle, RTSP_DATA_PACKET_T* pkt, VOID* priv);

typedef struct {
    CHAR_T                            url[RTSP_URL_LEN];             ///<the url of rtsp server, such as rtsp://xxx
    CHAR_T                            username[RTSP_USERNAME_LEN];   ///<username of rtsp server
    CHAR_T                            password[RTSP_PASSWORD_LEN];   ///<password of rtsp server
    RTSP_TRANSPORT_MODE_E             trans_mode;                    ///<stream in tcp or udp mode
    RTSP_CLIENT_PUT_DATA_CB           put_data_cb;                   ///<data callback
    RTSP_CLIENT_STATUS_CHANGE_CB      status_change_cb;              ///<status callback
    VOID*                             priv;                          ///<private data which will be put in the callback
} RTSP_CLIENT_PARAM_T;

typedef struct {
    UINT_T                            stack_size;        ///< KB
} RTSP_CLIENT_INIT_PARAM_T;

/**
 * @brief init rtsp client
 * @param[in] param init param
 */
OPERATE_RET tuya_ipc_rtsp_client_init(RTSP_CLIENT_INIT_PARAM_T *param);

/**
 * @brief deinit rtsp client
 */
OPERATE_RET tuya_ipc_rtsp_client_deinit(VOID);

/**
 * @brief start one rtsp client channel
 * @param[in] param start parameter
 * @return rtsp client handle
 */
RTSP_CLIENT_HANDLE_T tuya_ipc_rtsp_client_start(IN RTSP_CLIENT_PARAM_T* param);

/**
 * @brief stop one rtsp client channel
 * @param[in] handle returned at start
 * @return error code
 */
OPERATE_RET tuya_ipc_rtsp_client_stop(IN RTSP_CLIENT_HANDLE_T handle);

/**
 * @brief get media info of server, should be called after RTSP_CLIENT_STATUS_DESCRIBE_RESP status
 * @param[in] handle returned at start
 * @param[out] pmedia_info media info of server
 * @return error code
 */
OPERATE_RET tuya_ipc_rtsp_client_get_media_info(IN RTSP_CLIENT_HANDLE_T handle, OUT RTSP_MEDIA_INFO_T pmedia_info[RTSP_MEDIA_TYPE_NUM]);

/**
 * @brief notify rtsp client to update pts, should be called after system time changed
 * @return error code
 */
OPERATE_RET tuya_ipc_rtsp_client_update_systime(VOID);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __RTSP_CLIENT_H__
