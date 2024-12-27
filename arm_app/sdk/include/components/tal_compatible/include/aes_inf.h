/*
aes_inf.h
Copyright(C),2018-2020, 涂鸦科技 www.tuya.comm
*/
#ifndef _AES_INF_H
#define _AES_INF_H

#include "tuya_cloud_types.h"
#include "tal_symmetry.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AES128_ENCRYPT_KEY_LEN 16

/**
 * @brief aes_pkcs7padding_buffer
 *
 * @param[in] p_buffer
 * @param[in] length
 *
 * @return
 */
#define aes_pkcs7padding_buffer             tal_pkcs7padding_buffer

/**
 * @brief aes128_ecb_encode
 *
 * @param[in] data
 * @param[in] len
 * @param[out] ec_data
 * @param[out] ec_len
 * @param[in] key
 *
 * @return OPERATE_RET
 */
#define aes128_ecb_encode                   tal_aes128_ecb_encode

/**
 * @brief aes128_ecb_decode
 *
 * @param[in] data
 * @param[in] len
 * @param[out] dec_data
 * @param[out] dec_len
 * @param[in] key
 *
 * @return OPERATE_RET
 */
#define aes128_ecb_decode                   tal_aes128_ecb_decode


/**
 * @brief aes128_cbc_encode
 *
 * @param[in] data
 * @param[in] len
 * @param[in] key
 * @param[in] iv
 * @param[out] ec_data
 * @param[out] ec_len
 *
 * @return OPERATE_RET
 */
#define aes128_cbc_encode                   tal_aes128_cbc_encode

/**
 * @brief aes128_cbc_decode
 *
 * @param[in] data
 * @param[in] len
 * @param[in] key
 * @param[in] iv
 * @param[out] dec_data
 * @param[out] dec_len
 *
 * @return OPERATE_RET
 */
#define aes128_cbc_decode                   tal_aes128_cbc_decode

/**
 * @brief aes_free_data
 *
 * @param[in] data
 *
 * @return OPERATE_RET
 */
#define aes_free_data                       tal_aes_free_data

/**
 * @brief aes_get_actual_length
 *
 * @param[in] dec_data
 * @param[in] dec_data_len
 *
 * @return actural length of dec_data, negative on error
 */
#define aes_get_actual_length               tal_get_actual_length

/**
 * @brief aes256_cbc_encode_raw
 *
 * @param[in] data
 * @param[in] len
 * @param[in] key
 * @param[in] iv
 * @param[out] ec_data
 *
 * @return OPERATE_RET
 */
#define aes256_cbc_encode_raw               tal_aes256_cbc_encode_raw

/**
 * @brief aes128_ecb_encode_raw
 *
 * @param[in] data
 * @param[in] len
 * @param[out] ec_data
 * @param[in] key
 *
 * @return OPERATE_RET
 */
#define aes128_ecb_encode_raw               tal_aes128_ecb_encode_raw

/**
 * @brief aes128_ecb_decode_raw
 *
 * @param[in] data
 * @param[in] len
 * @param[out] dec_data
 * @param[in] key
 *
 * @return OPERATE_RET
 */
#define aes128_ecb_decode_raw               tal_aes128_ecb_decode_raw

/**
 * @brief aes128_cbc_encode_raw
 *
 * @param[in] data
 * @param[in] len
 * @param[in] key
 * @param[in] iv
 * @param[out] ec_data
 *
 * @return OPERATE_RET
 */
#define aes128_cbc_encode_raw               tal_aes128_cbc_encode_raw

/**
 * @brief aes128_cbc_decode_raw
 *
 * @param[in] data
 * @param[in] len
 * @param[in] key
 * @param[in] iv
 * @param[out] dec_data
 *
 * @return OPERATE_RET
 */
#define aes128_cbc_decode_raw               tal_aes128_cbc_decode_raw


#define aes128_free_data                    tal_aes_free_data

#define aes128_get_data_actual_length       tal_get_actual_length


#ifdef __cplusplus
}
#endif
#endif

