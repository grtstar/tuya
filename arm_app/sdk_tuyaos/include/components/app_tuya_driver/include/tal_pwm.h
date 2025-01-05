/**
 * @file tal_pwm.h
 * @brief Common process - adapter the pwm api
 * @version 1.0
 * @date 2021-09-10
 *
 * @copyright Copyright 2021-2031 Tuya Inc. All Rights Reserved.
 *
 */

#ifndef __TAL_PWM_H__
#define __TAL_PWM_H__

#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief pwm init
 * 
 * @param[in] ch_id: pwm channal id,id index starts from 0
 * @param[in] cfg: pwm config
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tal_pwm_init(UINT32_T ch_id, TUYA_PWM_BASE_CFG_T *cfg);

/**
 * @brief pwm deinit
 * 
 * @param[in] ch_id: pwm channal id,id index starts from 0
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tal_pwm_deinit(UINT32_T ch_id);

/**
 * @brief pwm start
 * 
 * @param[in] ch_id: pwm channal id,id index starts at 0
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tal_pwm_start(UINT32_T ch_id);

/**
 * @brief pwm stop
 * 
 * @param[in] ch_id: pwm channal id,id index starts from 0
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tal_pwm_stop(UINT32_T ch_id);

/**
 * @brief set pwm info
 * 
 * @param[in] ch_id: pwm channal id,id index starts from 0
 * @param[in] info: pwm info
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tal_pwm_info_set(UINT32_T ch_id, TUYA_PWM_BASE_CFG_T *info);

/**
 * @brief get pwm info
 * 
 * @param[in] ch_id: pwm channal id,id index starts from 0
 * @param[out] info: pwm info
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tal_pwm_info_get(UINT32_T ch_id, TUYA_PWM_BASE_CFG_T *info);


#ifdef __cplusplus
} // extern "C"
#endif

#endif // __TAL_PWM_H__

