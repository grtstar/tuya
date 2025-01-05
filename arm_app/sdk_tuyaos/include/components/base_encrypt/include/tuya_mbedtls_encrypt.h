/**
 * @file tuya_mbedtls_encrypt.h
 * @brief This is tuya mbedtls encrypt file
 * @version 1.0
 * @date 2022-07-15
 *
 * @copyright Copyright 2022-2031 Tuya Inc. All Rights Reserved.
 *
 */

#ifndef __TUYA_MBEDTLS_ENCRYPT_H__
#define __TUYA_MBEDTLS_ENCRYPT_H__

#include <string.h>
#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief mbedtls chacha20 encrypt
 * 
 * @param[in] pdata_in: input data to be encrypt
 * @param[in] data_len: intput data length
 * @param[out] pdata_out: output encrypted data
 * @param[out] pdata_out_len: output data length
 * @param[in] pkey: encrypt key
 * @param[in] piv: encrypt iv
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
 OPERATE_RET tuya_mbedtls_chacha20_encrypt(IN CONST BYTE_T *pdata_in, IN CONST UINT_T data_len,
                                            OUT BYTE_T *pdata_out, OUT UINT_T *pdata_out_len,
                                            IN CONST BYTE_T *pkey, IN BYTE_T *piv);

/**
 * @brief mbedtls chacha20 decrypt
 * 
 * @param[in] pdata_in: input data to be decrypt
 * @param[in] data_len: input data length
 * @param[out] pdata_out: output decrypted data
 * @param[out] pdata_out_len: ouput data length
 * @param[in] pkey: decrypt key
 * @param[in] piv: decrypt iv
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_mbedtls_chacha20_decrypt(IN CONST BYTE_T *pdata_in, IN CONST UINT_T data_len,
                                           OUT BYTE_T *pdata_out, OUT UINT_T *pdata_out_len,
                                           IN CONST BYTE_T *pkey, IN BYTE_T *piv);

/**
 * @brief mbedtls aes-gcm128 encrypt
 * 
 * @param[in] pdata_in: input data to be encrypt
 * @param[in] data_len: intput data length
 * @param[out] pdata_out: output encrypted data
 * @param[out] pdata_out_len: output data length
 * @param[in] pkey: encrypt key
 * @param[in] piv: encrypt iv
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_mbedtls_aes_gcm128_encrypt(IN CONST BYTE_T *pdata_in, IN CONST UINT_T data_len,
                                            OUT BYTE_T *pdata_out, OUT UINT_T *pdata_out_len,
                                            IN CONST BYTE_T *pkey, IN BYTE_T *piv);

/**
 * @brief mbedtls aes-gcm218 decrypt
 * 
 * @param[in] pdata_in: input data to be decrypt
 * @param[in] data_len: input data length
 * @param[out] pdata_out: output decrypted data
 * @param[out] pdata_out_len: ouput data length
 * @param[in] pkey: decrypt key
 * @param[in] piv: decrypt iv
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_mbedtls_aes_gcm128_decrypt(IN CONST BYTE_T *pdata_in, IN CONST UINT_T data_len,
                                  OUT BYTE_T *pdata_out, OUT UINT_T *pdata_out_len,
                                  IN CONST BYTE_T *pkey, IN BYTE_T *piv);

#ifdef __cplusplus
}
#endif

#endif
