/**
* @ tuya_ipc_p2p.h
* @brief p2p stream
* @version 0.1
* @date 2021-11-17
*
* @copyright Copyright (c) tuya.inc 2011
* 
*/

#ifndef __TUYA_IPC_P2P_H__
#define __TUYA_IPC_P2P_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "tuya_cloud_types.h"
#include "tuya_ipc_media.h"
#include "tuya_ipc_media_stream_event.h"
#include "tuya_ipc_media_adapter.h"


typedef enum
{
	TY_VIDEO_CLARITY_VALUE_PROFLOW = 0x1, /**< 省流量 */
    TY_VIDEO_CLARITY_VALUE_STANDARD = 0x2, /**< 标清 */
    TY_VIDEO_CLARITY_VALUE_HIGH = 0x4,     /**< 高清 */
    TY_VIDEO_CLARITY_VALUE_S_HIGH = 0x8,     /**< 超清 */
    TY_VIDEO_CLARITY_VALUE_SS_HIGH = 0x10,     /**< 超超清 */
}TRANSFER_VIDEO_CLARITY_VALUE_E;

/**
* @enum TRANS_DEFAULT_QUALITY_E
*
* @brief default quality for live P2P transferring
*/
typedef enum
{
    TRANS_DEFAULT_STANDARD = 0,     /**ex. 640*480, 15fps */
    TRANS_DEFAULT_HIGH,        /** ex. 1920*1080, 20fps */
    TRANS_DEFAULT_THIRD,
    TRANS_DEFAULT_FOURTH,
    TRANS_DEFAULT_MAX
}TRANS_DEFAULT_QUALITY_E;

/**
* @struct TUYA_IPC_P2P_VAR_T
*
* @brief p2p param
*/
typedef struct
{
    INT_T max_client_num;  /**max p2p connect num*/
    TRANS_DEFAULT_QUALITY_E def_live_mode;  /** for multi-streaming ipc, the default quality for live preview */  
    BOOL_T low_power;
    UINT_T recv_buffer_size; /*recv app data size. if recv_buffer_size = 0,default = 16*1024*/
}TUYA_IPC_P2P_VAR_T;

/**
* @struct CLIENT_CONNECT_INFO_T
*
* @brief P2P connection information
*/
typedef struct
{
    UCHAR_T p2p_mode; /** 0: P2P mode, 1: Relay mode, 2: LAN mode, 255: Not connected. */
    UCHAR_T local_nat_type; /** The local NAT type, 0: Unknown type, 1: Type 1, 2: Type 2, 3: Type 3, 10: TCP only */
    UCHAR_T remote_nat_type; /** The remote NAT type, 0: Unknown type, 1: Type 1, 2: Type 2, 3: Type 3, 10: TCP only */
    UCHAR_T relay_type; /** 0: Not Relay, 1: UDP Relay, 2: TCP Relay */
    VOID *p_reserved;
}CLIENT_CONNECT_INFO_T;

/**
 * @brief init tuya P2P, need init ipc sdk.
 * @param[in] TUYA_IPC_P2P_VAR_T
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tuya_ipc_tranfser_init(IN CONST TUYA_IPC_P2P_VAR_T *p_var);

/**
* @brief quit tuya P2P, free resources, memory e.g.
*
* @param[in]VOID

* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tuya_ipc_tranfser_quit(VOID);


/**
* @brief get P2P connection information
*
* @param[out] p_client_num: current connected client number
* @param[out] p_p_conn_info: p2p connection info
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tuya_ipc_get_client_conn_info(OUT UINT_T *p_client_num, OUT CLIENT_CONNECT_INFO_T **p_p_conn_info);


/**
* @brief free resource required by tuya_ipc_get_client_conn_info in param p_p_conn_info
*
* @param[in] p_conn_info: p2p connection info
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/

OPERATE_RET tuya_ipc_free_client_conn_info(IN CLIENT_CONNECT_INFO_T *p_conn_info);

/**
* @brief user set p2p to secret mode
*
* @param[in] mode：TRUE/FALSE secret/no secret
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/

OPERATE_RET tuya_ipc_tranfser_secret_mode(BOOL_T mode);

/**
* @brief delete video finish
*
* @param[in] client:current connected client number
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tuya_ipc_delete_video_finish(IN CONST UINT_T client);

/**
* @brief delete video finish v2
*
* @param[in] client:current connected client number
* @param[in] type:download type
* @param[in] success:0 fail 1 success
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tuya_ipc_delete_video_finish_v2(IN CONST UINT_T client, TUYA_DOWNLOAD_DATA_TYPE type, int success);

/**
* @brief p2p debug info
*
* @param[in] VOID
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tuya_ipc_p2p_debug(VOID);

/**
* @brief p2p client connect
*
* @param[out]handle:session idx
* @param[in]remote_id:remote device id
* @param[in]local_key:local_key
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tuya_ipc_p2p_client_connect(OUT           INT_T *handle, IN char *remote_id, IN char *local_key);

/**
* @brief p2p client disconnect
*
* @param[in] handle:session idx
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tuya_ipc_p2p_client_disconnect(int handle);

/**
* @brief start pull video frame
*
* @param[in] handle:session idx
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tuya_ipc_p2p_client_start_prev(INT_T handle);

/**
* @brief stop pull video frame
*
* @param[in] handle:session idx
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tuya_ipc_p2p_client_stop_prev(INT_T handle);

/**
* @brief start pull audio frame
*
* @param[in] handle:session idx
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tuya_ipc_p2p_client_start_audio(INT_T handle);

/**
* @brief stop pull audio frame
*
* @param[in] handle:session idx
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tuya_ipc_p2p_client_stop_audio(INT_T handle);

/**
* @brief set video standard clarity 
*
* @param[in] handle:session idx
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tuya_ipc_p2p_client_set_video_clarity_standard(INT_T handle);

/**
* @brief set video high clarity
*
* @param[in] handle:session idx
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tuya_ipc_p2p_client_set_video_clarity_high(INT_T handle);

/**
* @brief start push video frame
*
* @param[in] handle:session idx
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tuya_ipc_p2p_client_video_send_start(INT_T handle);

/**
* @brief start push video frame
*
* @param[in] handle:session idx
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tuya_ipc_p2p_client_video_send_stop(INT_T handle);

/**
* @brief start push audio frame
*
* @param[in] handle:session idx
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tuya_ipc_p2p_client_audio_send_start(INT_T handle);

/**
* @brief stop push audio frame
*
* @param[in] handle:session idx
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tuya_ipc_p2p_client_audio_send_stop(INT_T handle);

/**
* @brief BIND custom clarity to chn. TY_VIDEO_CLARITY_THIRD first, then TY_VIDEO_CLARITY_FOURTH
*
* @param[in] type:session idx
* @param[in] value:session idx
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tuya_ipc_bind_clarity_with_chn(TRANSFER_VIDEO_CLARITY_TYPE_E type, TRANSFER_VIDEO_CLARITY_VALUE_E value);

/***********************************album protocol ****************************************/
/**
 * \fn tuya_ipc_stop_send_data_to_app
 * \brief send file to app by p2p
 * \param[in] client: client handle
 * \return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_ipc_stop_send_data_to_app(IN INT_T client);

/**
 * \fn tuya_sweeper_send_data_with_buff
 * \brief send file to app by p2p
 * \param[in] client: client handle
 * \param[in] name: file name
 * \param[in] fileLen: file len
 * \param[in] fileBuff: file buff
 * \param[in] timeout_ms: timeout_ms 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_sweeper_send_data_with_buff(IN INT_T client, IN CHAR_T* name , IN INT_T fileLen, IN CHAR_T* fileBuff,IN INT_T timeout_ms);


/***********************************album protocol end ****************************************/

/**
 * \fn tuya_p2p_keep_alive
 * \brief keep p2p connection alive
 * \param[in] client: handle
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_p2p_keep_alive(IN INT_T client);

/**
*  Function: tuya_ipc_p2p_alone_stream_close
*  Note: One link is closed on the device
*  Input: client: handle
*  Output: none
*  return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tuya_ipc_p2p_alone_stream_close(IN INT_T client);

#ifdef __cplusplus
}
#endif

#endif
