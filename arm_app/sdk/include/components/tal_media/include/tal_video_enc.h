/**
 * @file tal_video_enc.h
 * @brief video encode
 * @version 0.1
 * @date 2021-11-09
 *
 * @copyright Copyright 2019-2021 Tuya Inc. All Rights Reserved.
 *
 * Set video parameters and get video bitstream.
 */

#ifndef __TAL_VIDEO_ENC_H__
#define __TAL_VIDEO_ENC_H__

#include "tkl_video_enc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TAL_VENC_CONFIG_T      TKL_VENC_CONFIG_T
#define TAL_VENC_FRAME_T       TKL_VENC_FRAME_T
#define TAL_VENC_OSD_T         TKL_VENC_OSD_T
#define TAL_VENC_STREAM_BUFF_T TKL_VENC_STREAM_BUFF_T
#define TAL_VENC_MASK_T        TKL_VENC_MASK_T

typedef enum {
    TAL_VENC_CMD_OSD,         // osd
    TAL_VENC_CMD_IDR,         // set IDR
    TAL_VENC_CMD_STREAM_BUFF, // Set the circular cache area of the video memory pool. For example, set two 30K memory
                              // pools for storage
    TAL_VENC_CMD_START,       // venc start
    TAL_VENC_CMD_STOP,        // venc stop
    TAL_VENC_CMD_MASK,        // mask
} TAL_VENC_CMD_E;

/**
 * @brief video encode init
 *
 * @param[in] vi_chn: vi channel number
 * @param[in] pconfig: venc config
 * @param[in] count: count of pconfig
 *
 * @return OPRT_OK on success. Others on error, please refer to tkl_error_code.h
 */
OPERATE_RET tal_venc_init(INT32_T vi_chn, TAL_VENC_CONFIG_T *pconfig, INT32_T count);

/**
 * @brief video set
 *
 * @param[in] vi_chn: vi channel number
 * @param[in] venc_chn: venc chn
 * @param[in] cmd: cmd
 * @param[in] parg: parg
 *
 * @return OPRT_OK on success. Others on error, please refer to tkl_error_code.h
 */
OPERATE_RET tal_venc_set(INT32_T vi_chn, INT32_T venc_chn, TAL_VENC_CMD_E cmd, VOID *parg);

/**
 * @brief video get
 *
 * @param[in] vi_chn: vi channel number
 * @param[in] venc_chn: venc chn
 * @param[in] cmd: cmd
 * @param[out] parg: parg
 *
 * @return OPRT_OK on success. Others on error, please refer to tkl_error_code.h
 */
OPERATE_RET tal_venc_get(INT32_T vi_chn, INT32_T venc_chn, TAL_VENC_CMD_E cmd, VOID *parg);

/**
 * @brief video encode get frame
 *
 * @param[in] vi_chn: vi channel number
 * @param[in] venc_chn: venc channel number
 * @param[out] pframe: output frame
 *
 * @return OPRT_OK on success. Others on error, please refer to tkl_error_code.h
 */
OPERATE_RET tal_venc_get_frame(INT32_T vi_chn, INT32_T venc_chn, TAL_VENC_FRAME_T *pframe);

/**
 * @brief video first snap
 *
 * @param[in] vi_chn: vi channel number
 * @param[out] pframe: output frame
 *
 * @return OPRT_OK on success. Others on error, please refer to tkl_error_code.h
 */
OPERATE_RET tal_venc_get_first_snap(INT32_T vi_chn, TAL_VENC_FRAME_T *pframe);

/**
 * @brief video encode uninit
 *
 * @param[in] vi_chn: vi channel number
 *
 * @return OPRT_OK on success. Others on error, please refer to tkl_error_code.h
 */
OPERATE_RET tal_venc_uninit(INT32_T vi_chn);

#ifdef __cplusplus
}
#endif

#endif
