#ifndef __TUYA_IPC_UPNP_H__
#define __TUYA_IPC_UPNP_H__

#ifdef __cplusplus
extern "C"{
#endif
#include "tuya_error_code.h"


/**
 * \fn OPERATE_RET tuya_ipc_upnp_init 
 * \brief 
 * \     Init unpn function. 
 * \return OPERATE_RET
 */
OPERATE_RET tuya_ipc_upnp_init(VOID);

OPERATE_RET tuya_ipc_upnp_request_port(int protocol, int *local_port, char *ext_address, int *ext_port);

OPERATE_RET tuya_ipc_upnp_release_port(int protocol, int local_port);

OPERATE_RET tuya_ipc_upnp_bind_result(int protocol, int local_port, int error_code);

VOID* tuya_ipc_upnp_request_port_list(int protocol, UINT_T port);

VOID tuya_ipc_upnp_deinit(VOID);


#ifdef __cplusplus
}
#endif

#endif