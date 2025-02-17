#pragma once

#include <stdio.h>
#include "tuya_cloud_types.h"
#include "tuya_ipc_api.h"
#include "uni_log.h"
#include "tuya_ipc_media.h"
#include "tuya_ipc_p2p.h"
#include "tuya_ipc_video_msg.h"
//#include "tuya_ipc_chromecast.h"
//#include "tuya_ipc_echo_show.h"
#include "tuya_cloud_com_defs.h"
#include "aes_inf.h"


#if defined(WIFI_GW) && (WIFI_GW==1)
#include "tuya_cloud_wifi_defs.h"
#endif

#include "tuya_cloud_types.h"
#include "tuya_cloud_error_code.h"
#include "tuya_cloud_com_defs.h"

#include "tuya_os_adapter.h"

#ifdef __cplusplus
extern "C" {
#endif
void PlayVoice(int v, int mode);
void DevicePairingSuccess(void);
#ifdef __cplusplus
}
#endif