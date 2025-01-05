#ifndef __RTP_PACKAGER_H__
#define __RTP_PACKAGER_H__


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include "rtp.h"

/**
* @struct TUYA_IPC_P2P_VAR_T
*
* @brief p2p param
*/
typedef struct {
    /** package */
    int (*pack)(RTP_PARAM_T *rtp_param, const char* frm_buf, int frm_len,tuya_ipc_rtp_pkt_handler_cb handler, void* arg);
    /** package fragment*/
    int (*pack_fragment)(RTP_PARAM_T *rtp_param, RTP_PACKAGER_FRAGMENT_T* pfrag,tuya_ipc_rtp_pkt_handler_cb handler, void* arg);
    /** ubpackage*/
    int (*unpack)(RTSP_DATA_PACKET_T* pkt, RTP_FRAME_T* pfrm);
    /** rtp package process*/
    int (*assemble_rtp_packet)(RTP_PARAM_T* rtp_param, uint32_t marker,
            const char* pkt_buf, int pkt_len, char* fu_buf, int fu_len,
            tuya_ipc_rtp_pkt_handler_cb handler, void* arg);
} RTP_PACKAGE_T;

/**
* @brief get rtp package fuction instance.
*
* @param[in] rtp code

* @return rtp  package fuction instance.
*/
RTP_PACKAGE_T* tuya_ipc_rtp_packager_get(RTP_CODEC_E codec);

#ifdef __cplusplus
}
#endif

#endif