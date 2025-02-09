/*
 * tuya_ipc_lowpower_api.h
 *
 *  Created on: 2020年8月13日
 *      Author: kuiba
 */

#ifndef SVC_LOWPOWER_INCLUDE_TUYA_IPC_LOWPOWER_API_H_
#define SVC_LOWPOWER_INCLUDE_TUYA_IPC_LOWPOWER_API_H_
#include "tuya_ipc_low_power_com_def.h"

/**
 * \fn OPERATE_RET tuya_ipc_lowpower_server_connect
 * \brief connect tuya low power servivce
 * \return OPERATE_RET 0:success.other:failed
 */

OPERATE_RET tuya_ipc_low_power_server_connect(UNW_IP_ADDR_T serverIp,INT_T port,CHAR_T* pdevId, INT_T idLen, CHAR_T* pkey, INT_T keyLen);
/**
 * \fn OPERATE_RET tuya_ipc_low_power_socket_fd_get
 * \brief get tuya low power keep alive tcp handler
 * \return OPERATE_RET 0:success.other:failed
 */
OPERATE_RET tuya_ipc_low_power_socket_fd_get();
/**
 * \fn OPERATE_RET tuya_ipc_lowpower_server_connect
 * \brief get tuya low power wakeup data
 * \return OPERATE_RET 0:success.other:failed
 */
OPERATE_RET tuya_ipc_low_power_wakeup_data_get(OUT CHAR_T* pdata, OUT UINT_T* plen);
/**
 * \fn OPERATE_RET tuya_ipc_lowpower_server_connect
 * \brief get tuya low power heart beat data;
 * \return OPERATE_RET 0:success.other:failed
 */
OPERATE_RET tuya_ipc_low_power_heart_beat_get(OUT CHAR_T * pdata,OUT UINT_T *plen);


#endif /* SVC_LOWPOWER_INCLUDE_TUYA_IPC_LOWPOWER_API_H_ */
