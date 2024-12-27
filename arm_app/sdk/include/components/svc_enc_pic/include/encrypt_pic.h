/**
 * @file encrypt_pic.h
 * @brief This is tuya encrypt pic file
 * @version 1.0
 * @date 2021-09-10
 *
 * @copyright Copyright 2021-2031 Tuya Inc. All Rights Reserved.
 *
 */

#ifndef __ENC_PIC_H__
#define __ENC_PIC_H__

#include "tuya_security_encrypt.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief generate aes iv
 * 
 * @param iv 
 * @param iv_size 
 * @return OPERATE_RET 
 */
OPERATE_RET cloud_generate_random_iv(UCHAR_T *iv, UINT_T iv_size);

/**
 * @brief generate aes key
 * 
 * @param key 
 * @param key_size 
 * @return OPERATE_RET 
 */
OPERATE_RET cloud_generate_random_key(UCHAR_T *key, UINT_T key_size);

/**
 * @brief encrypt pic
 * 
 * @param security_level
 * @param buffer 
 * @param size 
 * @param en_buffer 
 * @param en_size 
 * @param local_aes_key 
 * @return OPERATE_RET 
 */
OPERATE_RET cloud_encrypt_pic_with_random_iv(IN TUYA_IMM_SECURITY_LEVEL_E security_level, IN BYTE_T *buffer, IN UINT_T size, OUT BYTE_T **en_buffer, OUT UINT_T *en_size, IN BYTE_T *local_aes_key);

/**
 * @brief decrypt pic
 * 
 * @param buffer 
 * @param size 
 * @param de_buffer 
 * @param de_size 
 * @param local_aes_key 
 * @return OPERATE_RET 
 */
OPERATE_RET cloud_decrypt_pic_with_random_iv(IN BYTE_T *buffer, IN UINT_T size, OUT BYTE_T *de_buffer, OUT UINT_T *de_size, IN BYTE_T *local_aes_key);

/**
 * @brief get aes key of encrypt-pic
 * 
 * @param key 
 * @param len 
 * @return OPERATE_RET 
 */
OPERATE_RET cloud_encrypt_pic_get_aes_key(BYTE_T *key, INT_T len);

/**
 * @brief init encrypt pic module
 * 
 * @return OPERATE_RET 
 */
OPERATE_RET cloud_encrypt_pic_init(VOID);

/**
 * @brief for xvr, encrypt pic
 * 
 * @param nodeId 
 * @param chan 
 * @param buffer 
 * @param size 
 * @param en_buffer 
 * @param en_size 
 * @param local_aes_key 
 * @return OPERATE_RET 
 */
OPERATE_RET xvr_cloud_encrypt_pic_with_random_iv(IN CHAR_T * nodeId,IN INT_T chan,IN BYTE_T *buffer, IN UINT_T size, OUT BYTE_T **en_buffer, OUT UINT_T *en_size, IN BYTE_T *local_aes_key);

/**
 * @brief for xvr, get aes key of encrypt-pic
 * 
 * @param index 
 * @param key 
 * @param len 
 * @return OPERATE_RET 
 */
OPERATE_RET xvr_cloud_encrypt_pic_aes_key_get(INT_T index,BYTE_T *key, INT_T len);
#ifdef __cplusplus
}
#endif

#endif

