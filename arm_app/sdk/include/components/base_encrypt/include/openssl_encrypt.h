/**
 * @file openssl_encrypt.h
 * @brief This is tuya openssl encrypt file
 * @version 1.0
 * @date 2021-11-17
 *
 * @copyright Copyright 2021-2031 Tuya Inc. All Rights Reserved.
 *
 */

#ifndef __TUYA_OPENSSL_ENCRYPT_H__
#define __TUYA_OPENSSL_ENCRYPT_H__

#include <string.h>
#include "tuya_error_code.h"
#include "tuya_cloud_types.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief openssl AES CBC 128bit encrypt
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
OPERATE_RET openssl_aes_cbc128_encrypt(IN CONST BYTE_T *pdata_in, IN CONST UINT_T data_len,
                                      OUT BYTE_T *pdata_out, OUT UINT_T *pdata_out_len,
                                      IN CONST BYTE_T *pkey, IN BYTE_T *piv);

/**
 * @brief openssl AES CBC 128bit decrypt
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
OPERATE_RET openssl_aes_cbc128_decrypt(IN CONST BYTE_T *pdata_in, IN CONST UINT_T data_len,
                                      OUT BYTE_T *pdata_out, OUT UINT_T *pdata_out_len,
                                      IN CONST BYTE_T *pkey, IN BYTE_T *piv);


#ifdef __cplusplus
}
#endif

#endif
