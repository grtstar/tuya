#ifndef __TUYA_IPC_P2P_WRAPPER_H__
#define __TUYA_IPC_P2P_WRAPPER_H__

#include "tuya_cloud_types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef INT_T (*tuya_p2p_rtc_listen_cb_t)(INT_T handle);
// 注册 p2p 监听函数
INT_T tuya_p2p_rtc_register_p2p_listen_cb(tuya_p2p_rtc_listen_cb_t listen_cb);
// 注册 webrtc 监听函数
INT_T tuya_p2p_rtc_register_webrtc_listen_cb(tuya_p2p_rtc_listen_cb_t listen_cb);

OPERATE_RET tuya_p2p_rtc_listen_start();

VOID tuya_p2p_rtc_listen_stop();

#ifdef __cplusplus
}
#endif

#endif
