/* Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/* NIST Secure Hash Algorithm
 * 	heavily modified by Uwe Hollerbach uh@alumni.caltech edu
 * 	from Peter C. Gutmann's implementation as found in
 * 	Applied Cryptography by Bruce Schneier
 * 	This code is hereby placed in the public domain
 */

#ifndef APR_SHA1_H
#define APR_SHA1_H


#ifdef __cplusplus
extern "C" {
#endif

#include "tal_hash.h"

/**
 * @file apr_sha1.h
 * @brief APR-UTIL SHA1 library
 */

/** size of the SHA1 DIGEST */
#define APR_SHA1_DIGESTSIZE 20

/**
 * Define the Magic String prefix that identifies a password as being
 * hashed using our algorithm.
 */
#define APR_SHA1PW_ID "{SHA}"

/** length of the SHA Password */
#define APR_SHA1PW_IDLEN 5

/** @see apr_sha1_ctx_t */
// /**
//  * SHA1 context structure
//  */
#define apr_sha1_ctx_t      TKL_HASH_HANDLE

/**
 * Initialize the SHA digest
 * @param context The SHA context to initialize
 */
// void apr_sha1_init(apr_sha1_ctx_t *context);
#define apr_sha1_init(__ctx)                                    do { tal_sha1_create_init(__ctx);  tal_sha1_starts_ret(*(__ctx));} while(0)

/**
 * Update the SHA digest
 * @param context The SHA1 context to update
 * @param input The buffer to add to the SHA digest
 * @param inputLen The length of the input buffer
 */
// void apr_sha1_update(apr_sha1_ctx_t *context, const char *input,
//                      unsigned int inputLen);
#define apr_sha1_update(__ctx, __input, __inputlen)             tal_sha1_update_ret(*(__ctx), __input, __inputlen)

/**
 * Update the SHA digest with binary data
 * @param context The SHA1 context to update
 * @param input The buffer to add to the SHA digest
 * @param inputLen The length of the input buffer
 */
// void apr_sha1_update_binary(apr_sha1_ctx_t *context,
//                             const unsigned char *input,
//                             unsigned int inputLen);
#define apr_sha1_update_binary(__ctx, __input, __inputlen)      tal_sha1_update_ret(*(__ctx), __input, __inputlen)
/**
 * Finish computing the SHA digest
 * @param digest the output buffer in which to store the digest
 * @param context The context to finalize
 */
// void apr_sha1_final(unsigned char digest[APR_SHA1_DIGESTSIZE],
//                     apr_sha1_ctx_t *context);
#define apr_sha1_final(__digest, __ctx)                          do {tal_sha1_finish_ret(*(__ctx), __digest); tal_sha1_free(*(__ctx));} while(0)



//calculate HMAC_SHA1
// void HMAC_SHA1(unsigned char hmac[20], const unsigned char *key, int key_len,
//                const unsigned char *message, int message_len);


#ifdef __cplusplus
}
#endif

#endif	/* APR_SHA1_H */
