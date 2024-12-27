/**
* @file tuya_hal_storage.h
* @brief Common process - Initialization
* @version 0.1
* @date 2020-11-09
*
* @copyright Copyright 2020-2021 Tuya Inc. All Rights Reserved.
*
*/
#ifndef __TUYA_HAL_STORAGE_H__
#define __TUYA_HAL_STORAGE_H__

#include "tal_flash.h"

#ifdef __cplusplus
    extern "C" {
#endif

/**
* @brief Read flash
*
* @param[in] addr: flash address
* @param[out] dst: pointer of buffer
* @param[in] size: size of buffer
*
* @note This API is used for reading flash.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
#define tuya_hal_flash_read(addr, dst, size) tal_flash_read(addr, dst, size)

/**
* @brief Write flash
*
* @param[in] addr: flash address
* @param[in] src: pointer of buffer
* @param[in] size: size of buffer
*
* @note This API is used for reading flash.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
#define tuya_hal_flash_write(addr, src, size) tal_flash_write(addr, src, size)

/**
* @brief Erase flash
*
* @param[in] addr: flash address
* @param[in] size: size of flash block
*
* @note This API is used for erasing flash.
*
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
*/
#define tuya_hal_flash_erase(addr, size) tal_flash_erase(addr, size)

#ifdef __cplusplus
}
#endif

#endif // __TUYA_HAL_STORAGE_H__

