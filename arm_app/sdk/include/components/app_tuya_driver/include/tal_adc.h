/**
 * @file tal_adc.h
 * @brief Common process - adapter the adc api
 * @version 0.1
 * @date 2021-08-24
 *
 * @copyright Copyright 2021-2025 Tuya Inc. All Rights Reserved.
 *
 */

#ifndef __TAL_ADC_H__
#define __TAL_ADC_H__

#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief tuya hal adc init
 * 
 * @param[in] unit_num: adc unit number,ref to TUYA_ADC_NUM_E
 * @param[in] cfg: adc config
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tal_adc_init(TUYA_ADC_NUM_E unit_num, TUYA_ADC_BASE_CFG_T *cfg);

/**
 * @brief adc deinit
 * 
 * @param[in] unit_num: adc unit number,ref to TUYA_ADC_NUM_E
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tal_adc_deinit(TUYA_ADC_NUM_E unit_num);

/**
 * @brief adc read
 * 
 * @param[in] unit_num: adc unit number,ref to TUYA_ADC_NUM_E
 * @param[out] buff: points to the list of data read from the ADC register
 * @param[in] len:  buff len
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tal_adc_read_data(TUYA_ADC_NUM_E unit_num, INT32_T *buff, UINT16_T len);

/**
 * @brief read single channel
 *
 * @param[in] unit_num: adc unit number,ref to TUYA_ADC_NUM_E
 * @param[in] ch_num: channel number in one unit
 * @param[out] buf: convert result buffer
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 *
 */
OPERATE_RET tal_adc_read_single_channel(TUYA_ADC_NUM_E unit_num, UINT8_T ch_num, INT32_T *buf);


/**
 * @brief get adc width
 * 
 * @param[in] unit_num: adc unit number,ref to TUYA_ADC_NUM_E
 *
 * @return adc width
 */
UINT8_T tal_adc_width_get(TUYA_ADC_NUM_E unit_num);

/**
 * @brief adc get temperature
 *
 * @return temperature(bat: 'C)
 */
INT32_T tal_adc_temperature_get(VOID_T);

/**
 * @brief get adc reference voltage
 * 
 * @param[in] unit_num: adc unit number,ref to TUYA_ADC_NUM_E
 *
 * @return adc reference voltage(bat: mv)
 */
UINT32_T tal_adc_ref_voltage_get(TUYA_ADC_NUM_E port_num);

/**
 * @brief get adc voltage
 * @param[in] unit_num: adc unit number,ref to TUYA_ADC_NUM_E
 * @param[out] buff: list of voltage , -vref - +vref ,bat : mv
 * @param[in] len: buff len
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tal_adc_read_voltage(TUYA_ADC_NUM_E port_num, INT32_T *buff, UINT16_T len);


#ifdef __cplusplus
} 
#endif

#endif

