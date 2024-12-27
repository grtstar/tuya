/**
 * @file tuya_cert_manager.h
 * @author fangweng.hu@tuya.com
 * @brief Tuya Cert Manager include
 * @version 0.1
 * @date 2022-04-08
 *
 * @copyright Copyright 2022 Tuya Inc. All Rights Reserved.
 */

#ifndef __TUYA_CERT_MANAGER_H__
#define __TUYA_CERT_MANAGER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "tuya_cloud_types.h"
#include "ty_cJSON.h"

/**
 * Definition of client cert info
 */
typedef struct {
    BYTE_T *cert;
    UINT_T cert_len;
    BYTE_T *private_key;
    UINT_T private_key_len;
} client_cert_info_t;

/**
 * Definition of client psk info
 */
typedef struct {
    CHAR_T *psk_key;
    INT_T psk_key_size;
    CHAR_T *psk_id;
    INT_T psk_id_size;
} client_psk_info_t;

/**
 * @brief according url get third cloud ca
 *
 * @param[in] url Third cloud url
 *
 * @return
 */
VOID tuya_iot_get_third_cloud_ca(CHAR_T *p_url);

/**
 * @brief root ca write
 *
 * @param[in] value CA value
 * @param[in] len CA len
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_client_cert_write(IN CONST BYTE_T *value, IN CONST UINT_T len);

/**
 * @brief client private key write
 *
 * @param[in] value private key value
 * @param[in] len private key len
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_client_private_key_write(IN CONST BYTE_T *value, IN CONST UINT_T len);

/**
 * @brief get client cert
 *
 * @return const client_cert_info_t*
 */
CONST client_cert_info_t* tuya_client_cert_get(void);

/**
 * @brief cert manager init
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_cert_manager_init();

/**
 * @brief cert manager deinit
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
VOID tuya_cert_manager_deinit();

/**
 * @brief cert get accroding one url
 * @param[in] result cert result
 * @param[in] url_msg url
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET httpc_domain_certs_get(OUT ty_cJSON **result, IN CONST CHAR_T *url_msg);

/**
 * @brief cert parse cb
 *
 * @param[in] ctx contex
 * @param[in] cert ca cert
 * @param[in] cert_len ca cert len
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
typedef OPERATE_RET(*CERT_PARSE_CB)(VOID_T *ctx, UCHAR_T *cert, UINT_T cert_len);

/**
 * @brief cert load
 *
 * @param[in] url hostname
 * @param[in] cb cert parse cb
 * @param[in] p_ctx contex
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_cert_manager_load(CHAR_T *url, CERT_PARSE_CB cb, VOID_T *p_ctx);

/**
 * @brief save cert to kv
 *
 * @param[in] result cert info
 *
 */
VOID tuya_cert_save_to_kv(ty_cJSON *result);

/**
 * @brief cert resume from kv to ram
 *
 */
VOID tuya_cert_kv_to_ram(void);

typedef enum {
    TY_TLS_CERT_EXPIRED,
} TY_TLS_EVENT_E;
/**
 * @brief tls event cb
 *
 * @param[in] event event id
 * @param[in] p_args cb args
 *
 */
typedef VOID (*tuya_tls_event_cb)(TY_TLS_EVENT_E event, VOID *p_args);

/**
 * @brief get tls event cb
 *
 * @return event cb
 */
tuya_tls_event_cb tuya_cert_get_tls_event_cb();

/**
 * @brief get client psk
 *
 * @return const client_psk_info_t*
 */
CONST client_psk_info_t* tuya_client_psk_get(VOID);

/**
 * @brief Definition of domain's IP and cert structure
 */
typedef struct {
    /** Host name*/
    CHAR_T *host;
    /** Host port */
    INT_T port;
    /** is certs required */
    BOOL_T need_ca;
    /** is ip required */
    BOOL_T need_ip;
} DNS_QUERY_S;

/**
 * @brief get root ca
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET http_iot_dns_get_root_ca();

#ifdef __cplusplus
}
#endif

#endif //__TUYA_CERT_MANAGER_H__
