#ifndef __COMPONENT_SWEEPER_ADAPTER_H__
#define __COMPONENT_SWEEPER_ADAPTER_H__

#include "tuya_cloud_types.h"
#include "tuya_ipc_stream_storage.h"


#ifdef __cplusplus
extern "C" {
#endif

VOID tuya_ipc_sd_get_capacity(UINT_T *p_total, UINT_T *p_used, UINT_T *p_free);

STREAM_STORAGE_WRITE_MODE_E tuya_ipc_sd_get_mode_config(VOID);

E_SD_STATUS tuya_ipc_sd_get_status(VOID);

CHAR_T *tuya_ipc_get_sd_mount_path(VOID);

VOID tuya_ipc_sd_remount(VOID);



#ifdef __cplusplus
} // extern "C"
#endif

#endif // __COMPONENT_SWEEPER_ADAPTER_H__

