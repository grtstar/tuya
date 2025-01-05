/**
 * @file uni_md5.h
 * @brief
 * @author www.tuya.com
 * @version 1.0.0
 * @date 2021-01-11
 */

#ifndef __UNI_MD5_H
#define __UNI_MD5_H

#include "tal_hash.h"

#define MD5_DECRYPT_LEN   16


#define UNI_MD5_CTX_S  TKL_HASH_HANDLE

/**
 * @brief uni_md5_init
 *
 * @param context
 */
// void uni_md5_init(UNI_MD5_CTX_S *context);
#define uni_md5_init(__ctx)                                     do { tal_md5_create_init(__ctx);  tal_md5_starts_ret(*(__ctx));} while(0)

/**
 * @brief uni_md5_update
 *
 * @param context
 * @param[in] input
 * @param[in] inputlen
 */
// void uni_md5_update(UNI_MD5_CTX_S *context, const unsigned char *input, const unsigned int inputlen);
#define uni_md5_update(__ctx, __input, __inputlen)              tal_md5_update_ret(*(__ctx), __input, __inputlen)

/**
 * @brief uni_md5_final
 *
 * @param context
 * @param digest[16]
 */
// void uni_md5_final(UNI_MD5_CTX_S *context, unsigned char digest[16]);
#define uni_md5_final(__ctx, __digest)                          do {tal_md5_finish_ret(*(__ctx), __digest); tal_md5_free(*(__ctx));} while(0)


// void uni_md5_digest_tolal(const unsigned char *input, const unsigned int ilen, unsigned char output[16]);
#define uni_md5_digest_tolal                                    tal_md5_ret


#endif
