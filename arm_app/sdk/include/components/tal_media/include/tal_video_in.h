/**
 * @file tal_video_in.h
 * @brief video input
 * @version 0.1
 * @date 2021-11-09
 *
 * @copyright Copyright 2019-2021 Tuya Inc. All Rights Reserved.
 *
 * Video input configuration: for sensor, ISP or VI attribute configuration,
 * such as image flip, anti flicker, compensation mode, profile, etc.
 */

#ifndef __TAL_VIDEO_IN_H__
#define __TAL_VIDEO_IN_H__

#include "tkl_media.h"
#include "tkl_video_in.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TAL_VI_CONFIG_T            TKL_VI_CONFIG_T
#define TAL_VI_HARDWARE_SOURCE_T   TKL_VI_HARDWARE_SOURCE_T
#define TAL_VI_SENSOR_REG_CONFIG_T TKL_VI_SENSOR_REG_CONFIG_T

typedef enum {
    TAL_VI_CMD_MIRROR_FLIP,
    TAL_VI_CMD_ANTIFLICKER,
    TAL_VI_CMD_ILLUMIN,
    TAL_VI_CMD_DAYMODE,
    TAL_VI_CMD_SENSOR_REG,
} TAL_VI_CMD_E;

/**
 * @brief vi init
 *
 * @param[in] pconfig: vi config
 * @param[in] count: count of pconfig
 *
 * @return OPRT_OK on success. Others on error, please refer to tkl_error_code.h
 */
OPERATE_RET tal_vi_init(TAL_VI_CONFIG_T *pconfig, INT32_T count);

/**
 * @brief vi set
 *
 * @param[in] chn: vi chn
 * @param[in] cmd: cmd
 * @param[in] parg: parg
 *
 * @return OPRT_OK on success. Others on error, please refer to tkl_error_code.h
 */
OPERATE_RET tal_vi_set(INT32_T chn, TAL_VI_CMD_E cmd, VOID *parg);

/**
 * @brief vi get
 *
 * @param[in] chn: vi chn
 * @param[in] cmd: cmd
 * @param[out] parg: parg
 *
 * @return OPRT_OK on success. Others on error, please refer to tkl_error_code.h
 */
OPERATE_RET tal_vi_get(INT32_T chn, TAL_VI_CMD_E cmd, VOID *parg);

/**
 * @brief vi uninit
 *
 * @return OPRT_OK on success. Others on error, please refer to tkl_error_code.h
 */
OPERATE_RET tal_vi_uninit(VOID);

#ifdef __cplusplus
}
#endif

#endif
