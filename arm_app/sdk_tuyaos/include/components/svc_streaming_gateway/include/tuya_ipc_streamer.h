/**
* @tuya_ipc_stream.h
* @brief gateway stream used
* @version 0.1
* @date 2021-11-17
*
* @copyright Copyright (c) tuya.inc 2011
* 
*/

#ifndef _TUYA_STREAMER_H_
#define _TUYA_STREAMER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "tuya_ipc_media_adapter.h"
#include "tuya_ipc_media_stream_event.h"


/**
* @enum STREAM_TRANS_MODE_E
*
* @brief define gateway module prefer transfer protocol
*/
typedef enum
{
    E_TRANS_MODE_INVAILD,
    E_TRANS_MODE_KCP,   //kcp transfer protocol
    E_TRANS_MODE_TCP,   //tcp transfer protocol

}STREAM_TRANS_MODE_E;

/**
* @brief gateway stream module init api
*
* @param[in] VOID

* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tuya_ipc_gateway_streamer_init(VOID);

/**
* @brief get gateway stream online cnt
*
* @param[in] VOID

* @return gateway stream online cnt
*/
INT_T       tuya_ipc_gateway_stream_alive_cnt(VOID);

/**
* @brief pause gateway push stream
*
* @param[in] VOID

* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tuya_ipc_gateway_stream_pause(VOID);

/**
* @brief reume gateway push stream
*
* @param[in] VOID

* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tuya_ipc_gateway_stream_resume(VOID);

/**
* @brief stop gateway push stream
*
* @param[in] VOID

* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tuya_ipc_gateway_stream_stop(VOID);

/**
* @brief set gateway stream transfer protocol
*
* @param[in] transMode:transfer protocol

* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tuya_ipc_gateway_stream_set_prefer_mode(STREAM_TRANS_MODE_E transMode);

/**
* @brief set gateway stream fec param
*
* @param[in] fec_switch:fec switch
* @param[in] data_share:fec data  share
* @param[in] parity_share:fec parity share
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
OPERATE_RET tuya_ipc_gateway_set_fec_param (BOOL_T fec_switch,UINT_T data_share,UINT_T parity_share );

#ifdef __cplusplus
}
#endif

#endif

