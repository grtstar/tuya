/**
* @file tuya_cnt_rst.h
* @brief Device reset count utilities
* @version 0.1
* @date 2019-04-25
*
* @copyright Copyright 2019-2021 Tuya Inc. All Rights Reserved.
*/

#ifndef _TUYA_CNT_RST_H_
#define _TUYA_CNT_RST_H_

#include "tuya_cloud_types.h"

/**
 * @brief Handler when reset count matches rst_num
 */
typedef VOID(* TUYA_CNT_RST_INFORM_CB)(VOID);

/**
 * @brief Set count reset mode, need be involked within pre_device_init
 * 
 * @param rst_num Reset count
 * @param cb Callback when reset
 * 
 * @note cb will be involked between pre_app_init and app_init, if reset count matches rst_num
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_set_cnt_rst_mode(IN CONST UINT8_T rst_num, IN CONST TUYA_CNT_RST_INFORM_CB cb);

#endif