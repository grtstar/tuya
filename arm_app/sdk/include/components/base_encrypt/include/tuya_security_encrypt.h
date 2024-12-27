/**
 * @file tuya_security_encrypt.h
 * @brief This is tuya security encrypt file
 * @version 1.0
 * @date 2022-07-20
 *
 * @copyright Copyright 2022-2031 Tuya Inc. All Rights Reserved.
 *
 */

#ifndef __TUYA_SECURITY_ENCRYPT_H__
#define __TUYA_SECURITY_ENCRYPT_H__

#include <string.h>
#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TUYA_IMM_SECURITY_SIGN_LEN  16    ///< sign length for aes128-gcm, sign will be added automatically at the end of pdata_out

typedef enum {
    TUYA_IMM_SECURITY_LEVEL_0,  ///< not supported
    TUYA_IMM_SECURITY_LEVEL_1,  ///< not supported
    TUYA_IMM_SECURITY_LEVEL_2,  ///< chacha20
    TUYA_IMM_SECURITY_LEVEL_3,  ///< aes128-cbc
    TUYA_IMM_SECURITY_LEVEL_4,  ///< aes128-gcm
    TUYA_IMM_SECURITY_LEVEL_5,  ///< not supported
    TUYA_IMM_SECURITY_LEVEL_MAX
}TUYA_IMM_SECURITY_LEVEL_E;

/**
 * @brief tuya security encrypt
 * 
 * @param[in] security_level
 * @param[in] pdata_in: input data to be encrypt
 * @param[in] data_len: intput data length
 * @param[out] pdata_out: output encrypted data
 * @param[out] pdata_out_len: output data length
 * @param[in] pkey: encrypt key
 * @param[in] piv: encrypt iv
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_imm_security_encrypt(IN TUYA_IMM_SECURITY_LEVEL_E security_level, IN CONST BYTE_T *pdata_in, IN CONST UINT_T data_len,
                                            OUT BYTE_T *pdata_out, OUT UINT_T *pdata_out_len,
                                            IN CONST BYTE_T *pkey, IN BYTE_T *piv);

/**
 * @brief tuya security decrypt
 * 
 * @param[in] security_level
 * @param[in] pdata_in: input data to be decrypt
 * @param[in] data_len: input data length
 * @param[out] pdata_out: output decrypted data
 * @param[out] pdata_out_len: ouput data length
 * @param[in] pkey: decrypt key
 * @param[in] piv: decrypt iv
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_imm_security_decrypt(IN TUYA_IMM_SECURITY_LEVEL_E security_level, IN CONST BYTE_T *pdata_in, IN CONST UINT_T data_len,
                                            OUT BYTE_T *pdata_out, OUT UINT_T *pdata_out_len,
                                            IN CONST BYTE_T *pkey, IN BYTE_T *piv);

/**
 * @brief check security level
 *
 * @param[in] security_level
 *
 * @return TRUE if security_level is supported.
 */
BOOL_T tuya_imm_check_security_level(IN TUYA_IMM_SECURITY_LEVEL_E security_level);

/**
 * @brief get security_ability
 *
 * @param[in] VOID
 *
 * @return security_ability
 */
INT_T tuya_imm_get_security_ability(VOID);

#ifdef __cplusplus
}
#endif

#endif
