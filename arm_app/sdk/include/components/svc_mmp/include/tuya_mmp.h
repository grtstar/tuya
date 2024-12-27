#ifndef __TUYA_MMP_H__
#define __TUYA_MMP_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "tuya_cloud_types.h"
#include "tal_network.h"

typedef void* TUYA_MMP_SESSION_HANDLE_T;
typedef void* TUYA_MMP_STREAM_HANDLE_T;
typedef INT_T TUYA_MMP_ERROR_CODE_E;
#define TUYA_MMP_EC_OK                 0
#define TUYA_MMP_EC_COM_ERROR           -1
#define TUYA_MMP_EC_INVALID_PARM        -2
#define TUYA_MMP_EC_MALLOC_FAILED       -3
#define TUYA_MMP_EC_TOKEN_ERROR        -10
#define TUYA_MMP_EC_DATA_FORMAT_ERROR  -11
#define TUYA_MMP_EC_STREAM_OVER_LIMIT  -12
#define TUYA_MMP_EC_TIMEOUT            -13
#define TUYA_MMP_EC_URI_NOT_FOUND      -14
#define TUYA_MMP_EC_BUFFER_NOT_ENOUGH  -15

typedef OPERATE_RET (*TUYA_MMP_CONNECT_CB)(TUYA_MMP_ERROR_CODE_E result_ret,TUYA_MMP_SESSION_HANDLE_T sess_hdl, void* priv);
typedef OPERATE_RET (*TUYA_MMP_DISCONNECT_CB)(TUYA_MMP_ERROR_CODE_E result_ret, void* priv);
typedef OPERATE_RET (*TUYA_MMP_STREAM_CTREATE_CB)(TUYA_MMP_ERROR_CODE_E result_ret,TUYA_MMP_STREAM_HANDLE_T sess_hdl, void* priv);
typedef OPERATE_RET (*TUYA_MMP_STREAM_DESTROY_CB)(TUYA_MMP_ERROR_CODE_E result_ret, void* priv);
typedef OPERATE_RET (*TUYA_MMP_STREAM_DATA_CB)(TUYA_MMP_ERROR_CODE_E err_code, CHAR_T* data, INT_T len, BOOL_T end, void* priv);
typedef OPERATE_RET (*TUYA_MMP_LIST_URI_CB)(TUYA_MMP_ERROR_CODE_E err_code, CHAR_T* uri[], INT_T uri_count, void* priv);
typedef OPERATE_RET (*TUYA_MMP_GET_URI_CB)(TUYA_MMP_ERROR_CODE_E err_code, UINT_T ssrc, void* priv);
typedef OPERATE_RET (*TUYA_MMP_PUSH_URI_CB)(TUYA_MMP_ERROR_CODE_E err_code, void* priv);
typedef OPERATE_RET (*TUYA_MMP_ON_GET_URI_REQUEST_CB)(TUYA_MMP_ERROR_CODE_E err_code, TUYA_MMP_STREAM_HANDLE_T str_hdl, UINT_T ssrc, CHAR_T* uri, void* priv);
typedef OPERATE_RET (*TUYA_MMP_DELETE_URI_RESULT_CB)(TUYA_MMP_ERROR_CODE_E err_code, void* priv);
typedef TUYA_MMP_LIST_URI_CB TUYA_MMP_REC_PUSH_LIST_CB;

typedef struct {
    UINT_T stack_size;           ///<stack size of mmp thread
    UINT_T stream_count_max;     ///<max stream number limit
    UINT_T port;                 ///<listen port
    CHAR_T devuid[64];          // device Unique Identifier
    TUYA_MMP_REC_PUSH_LIST_CB on_rec_push_list_cb; //receive push list callback;
    VOID * on_rec_push_list_pri; //receive push list callback parameter
} TUYA_MMP_INIT_PARAM_T;

/**
 * @brief do init stuff, create thread, socket and so on.
 * 
 * @param param init parameter
 * @return error code 
 * - OPRT_OK init ok
 * - Others fail
 */
OPERATE_RET tuya_mmp_init(
    TUYA_MMP_INIT_PARAM_T *param
);

/**
 * @brief do uninit stuff
 * 
 * @return error code
 * - OPRT_OK init ok
 * - Others fail
 */
OPERATE_RET tuya_mmp_uninit();

/**
 * @brief connect to peer device, on_result will be called after handshake
 * 
 * @param ip ip of peer device
 * @param port port of peer device
 * @param on_result the callback after handshake with peer device
 * @param priv private data which can be used in callback
 * @return error code
 * - OPRT_OK init ok
 * - Others fail
 */
OPERATE_RET tuya_mmp_connect(
    TUYA_IP_ADDR_T ip,
    UINT_T port,
    TUYA_MMP_CONNECT_CB on_result,
    void* priv
);

/**
 * @brief  disconnect with peer device
 * 
 * @param sess_hdl session handle
 * @param on_result callback after disconnect
 * @param priv callback parameter
 * @return error code
 * - OPRT_OK init ok
 * - Others fail
 */
OPERATE_RET tuya_mmp_disconnect(
    TUYA_MMP_SESSION_HANDLE_T sess_hdl,
    TUYA_MMP_DISCONNECT_CB on_result,
    void* priv
);

/**
 * @brief create a stream channel, which can be used to transfer audio,video or file
 * 
 * @param sess_hdl session handle
 * @param priority priority of the channel, between 1~6
 * @param buffer_size the total recv and send buffer size of the channel
 * @param on_result the callback after channel create success or fail
 * @param on_data the callback after receive data from the channel
 * @param priv private data which can be used in the callback
 * @return OPERATE_RET 
 */
OPERATE_RET tuya_mmp_stream_create(
    TUYA_MMP_SESSION_HANDLE_T sess_hdl,
    UINT_T priority,
    UINT_T buffer_size,
    TUYA_MMP_STREAM_CTREATE_CB on_result,
    TUYA_MMP_STREAM_DATA_CB on_data,
    void* priv
);

/**
 * @brief destroy a stream channel
 * 
 * @param str_hdl stream handle
 * @param on_result callback after destroy
 * @param priv callback parameter
 * @return OPERATE_RET 
 */
OPERATE_RET tuya_mmp_stream_destroy(
    TUYA_MMP_STREAM_HANDLE_T str_hdl,
    TUYA_MMP_STREAM_DESTROY_CB on_result,
    void* priv
);

/**
 * @brief pause a stream channel
 * 
 * @param str_hdl stream handle
 * @return OPERATE_RET 
 */
OPERATE_RET tuya_mmp_stream_pause(
    TUYA_MMP_STREAM_HANDLE_T str_hdl
);

/**
 * @brief resume a stream channel
 * 
 * @param str_hdl stream handle
 * @return OPERATE_RET 
 */
OPERATE_RET tuya_mmp_stream_resume(
    TUYA_MMP_STREAM_HANDLE_T str_hdl
);

/**
 * @brief list all uris of peer device
 * 
 * @param sess_hdl session handle
 * @param on_result the callback after receive the uri
 * @return OPERATE_RET 
 */
OPERATE_RET tuya_mmp_list_uri(
    TUYA_MMP_SESSION_HANDLE_T sess_hdl,
    TUYA_MMP_LIST_URI_CB on_result,
    void* priv
);

/**
 * @brief get content of the specified uri
 * 
 * @param str_hdl stream handle
 * @param uri which uri you want get
 * @param on_result the callback after response of the peer device
 * @param on_data the callback when receive content data of the uri from peer device
 * @return OPERATE_RET 
 */
OPERATE_RET tuya_mmp_get_uri(
    TUYA_MMP_STREAM_HANDLE_T str_hdl,
    CHAR_T* uri,
    TUYA_MMP_GET_URI_CB on_reuslt,
    void* priv
);

/**
 * @brief tell peer device of my uris
 * 
 * @param str_hdl  stream handle
 * @param uri my uris
 * @param uri_count count of uris
 * @param on_result the callback after response
 * @return OPERATE_RET 
 */
OPERATE_RET tuya_mmp_push_uri(
    TUYA_MMP_SESSION_HANDLE_T str_hdl,
    CHAR_T* uri[],
    INT_T uri_count,
    TUYA_MMP_PUSH_URI_CB on_result,
    void* priv
);

/**
 * @brief register uri with a timeout, then peer device can use GET_URI to visit the content
 * 
 * @param uri the uri to register
 * @param expire_time_ms the uri will be removed after the timeout
 * @param on_get_uri the callback after peer device GET_URI
 * @return OPERATE_RET 
 */
OPERATE_RET tuya_mmp_register_uri(
	CHAR_T* uri,
    INT_T expire_time_ms,
    TUYA_MMP_ON_GET_URI_REQUEST_CB on_get_uri,
    void* priv
);

/**
 * @brief delete a uri
 * 
 * @param uri the uri to delete
 * @param on_result delete result callback
 * @param priv  callback param
 * @return OPERATE_RET 
 */
OPERATE_RET tuya_mmp_delete_uri(
    CHAR_T* uri,
    TUYA_MMP_DELETE_URI_RESULT_CB on_result,
    void* priv
);

/**
 * @brief send data to peer device
 * 
 * @param str_hdl stream handle
 * @param data data buffer
 * @param len data length
 * @param end whether to set the END flag
 * @return OPERATE_RET 
 */
OPERATE_RET tuya_mmp_send_data_sync(
	TUYA_MMP_STREAM_HANDLE_T str_hdl,
    CHAR_T* data,
    INT_T len,
    BOOL_T end
);

/**
 * @brief get transport buffer status
 * 
 * @param handle  stream handle
 * @param wait_snd_bytes bytes of data in send buffer
 * @param wait_rcv_bytes bytes of data in recv buffer
 * @param free_bytes free bytes in send buffer
 * @return OPERATE_RET 
 */
OPERATE_RET tuya_mmp_get_buffer_stat_sync(
    TUYA_MMP_STREAM_HANDLE_T handle,
    UINT_T* wait_snd_bytes,
    UINT_T* wait_rcv_bytes,
    UINT_T* free_bytes
);

OPERATE_RET tuya_mmp_dump_session_info();

#ifdef __cplusplus
} // extern "C"
#endif

#endif 

