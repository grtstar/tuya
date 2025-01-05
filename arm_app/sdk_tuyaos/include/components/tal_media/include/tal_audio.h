/**
 * @file tal_audio.h
 * @brief Common process - the audio api
 * @version 0.1
 * @date 2021-11-09
 *
 * @copyright Copyright 2019-2021 Tuya Inc. All Rights Reserved.
 *
 */

#ifndef __TAL_AUDIO_H__
#define __TAL_AUDIO_H__

#include "tkl_audio.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TAL_AUDIO_CONFIG_T           TKL_AUDIO_CONFIG_T
#define TAL_AUDIO_FRAME_INFO_T       TKL_AUDIO_FRAME_INFO_T
#define TAL_AUDIO_HARDWARE_SOURCE    TKL_AUDIO_HARDWARE_SOURCE

typedef enum {
    TAL_AI_CMD_VOL, // AI volume
} TAL_AI_CMD_E;

typedef enum {
    TAL_AO_CMD_VOL, // AO volume, int, val[0, 100]
} TAL_AO_CMD_E;

/**
 * @brief ai init
 *
 * @param[in] pconfig: audio config
 * @param[in] count: count of pconfig
 *
 * @return OPRT_OK on success. Others on error, please refer to tkl_error_code.h
 */
OPERATE_RET tal_ai_init(TAL_AUDIO_CONFIG_T *pconfig, INT32_T count);

/**
 * @brief ai start
 *
 * @param[in] card: card number
 * @param[in] chn: channel number
 *
 * @return OPRT_OK on success. Others on error, please refer to tkl_error_code.h
 */
OPERATE_RET tal_ai_start(INT32_T card, INT32_T chn);

/**
 * @brief ai set
 *
 * @param[in] card: card number
 * @param[in] chn: channel number
 * @param[in] cmd
 * @param[in] parg
 *
 * @return OPRT_OK on success. Others on error, please refer to tkl_error_code.h
 */
OPERATE_RET tal_ai_set(INT32_T card, INT32_T chn, TAL_AI_CMD_E cmd, VOID *parg);

/**
 * @brief ai get frame
 *
 * @param[in] card: card number
 * @param[in] chn: channel number
 * @param[out] pframe: audio frame, pframe->pbuf allocated by upper layer application
 *
 * @return OPRT_OK on success. Others on error, please refer to tkl_error_code.h
 */
OPERATE_RET tal_ai_get_frame(INT32_T card, INT32_T chn, TAL_AUDIO_FRAME_INFO_T *pframe);

/**
 * @brief ai stop
 *
 * @param[in] card: card number
 * @param[in] chn: channel number
 *
 * @return OPRT_OK on success. Others on error, please refer to tkl_error_code.h
 */
OPERATE_RET tal_ai_stop(INT32_T card, INT32_T chn);

/**
 * @brief ai uninit
 *
 * @return OPRT_OK on success. Others on error, please refer to tkl_error_code.h
 */
OPERATE_RET tal_ai_uninit(VOID);

/**
 * @brief ao init
 *
 * @param[in] pconfig: audio config
 * @param[in] count: config count
 *
 * @return OPRT_OK on success. Others on error, please refer to tkl_error_code.h
 */
OPERATE_RET tal_ao_init(TAL_AUDIO_CONFIG_T *pconfig, INT32_T count, VOID **handle);

/**
 * @brief ao start
 *
 * @param[in] card: card number
 * @param[in] chn: channel number
 * @param[out] handle: handle of start
 *
 * @return OPRT_OK on success. Others on error, please refer to tkl_error_code.h
 */
OPERATE_RET tal_ao_start(INT32_T card, INT32_T chn, VOID *handle);

/**
 * @brief ao set
 *
 * @param[in] card: card number
 * @param[in] chn: channel number
 * @param[in] handle: the return of start
 * @param[in] cmd
 * @param[in] parg
 *
 * @return OPRT_OK on success. Others on error, please refer to tkl_error_code.h
 */
OPERATE_RET tal_ao_set(INT32_T card, INT32_T chn, VOID *handle, TAL_AO_CMD_E cmd, VOID *parg);

/**
 * @brief ao get
 *
 * @param[in] card: card number
 * @param[in] chn: channel number
 * @param[in] handle: the return of start
 * @param[in] cmd
 * @param[in] parg
 *
 * @return OPRT_OK on success. Others on error, please refer to tkl_error_code.h
 */
OPERATE_RET tal_ao_get(INT32_T card, INT32_T chn, VOID *handle, TAL_AO_CMD_E cmd, VOID *parg);

/**
 * @brief ao output frame
 *
 * @param[in] card: card number
 * @param[in] chn: channel number
 * @param[in] handle: handle of start
 * @param[in] pframe: output frame
 *
 * @return OPRT_OK on success. Others on error, please refer to tkl_error_code.h
 */
OPERATE_RET tal_ao_put_frame(INT32_T card, INT32_T chn, VOID *handle, TAL_AUDIO_FRAME_INFO_T *pframe);

/**
 * @brief ao stop
 *
 * @param[in] card: card number
 * @param[in] chn: channel number
 * @param[in] handle: handle of start
 *
 * @return OPRT_OK on success. Others on error, please refer to tkl_error_code.h
 */
OPERATE_RET tal_ao_stop(INT32_T card, INT32_T chn, VOID *handle);

/**
 * @brief ao uninit
 *
 * @return OPRT_OK on success. Others on error, please refer to tkl_error_code.h
 */
OPERATE_RET tal_ao_uninit(VOID *handle);

#ifdef __cplusplus
}
#endif

#endif
