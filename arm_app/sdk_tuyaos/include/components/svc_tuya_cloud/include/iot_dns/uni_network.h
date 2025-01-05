/**
 * @file uni_network.h
 *
 * @brief Tuya iotdns module is the DNS service module provided by Tuya, which ensures that
 * developers can provide reliable, safe and stable DNS services when accessing the network.
 * Tuya iotdns provides a DNS mechanism of mutual protection for static zone DNS resolution,
 * dynamic random zone DNS resolution and local system DNS resolution. Among them, the DNS
 * resolution mechanism relies on Tuya The trusted DNS service provided in the cloud can obtain
 * the optimal DNS policy according to the zone and provide a load balancing mechanism; the dynamic
 * random zone DNS resolution is based on the zone DNS resolution mechanism. In order to avoid a
 * protection mechanism provided by DNS failure 2, a random zone is selected in all non current
 * zones for DNS request; the local system DNS is provided by the local operator As a backing mechanism,
 * the basic DNS service of. Tuya iotdns can switch the resolution mechanism in case of failure
 * to ensure the stability and security of the whole mechanism.
 *
 * @version 1.0
 *
 * @date 2019-09-09
 *
 * @copyright Copyright (c) tuya.inc 2019
 *
 */


#ifndef __TUYA_BASE_UNI_DNS_CACHE_H__
#define __TUYA_BASE_UNI_DNS_CACHE_H__

#include "tuya_iot_config.h"
#include "tuya_cloud_types.h"
#include "tal_network.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief max length of domain name
 *
 */
#define MAX_DOMAIN_NAME_LEN     128

/**
 * @brief DNS priority
 *
 */
typedef UINT_T TY_DNS_PRIO_T;
#define DNS_PRIO_HTTP    0   // use http dns services
#define DNS_PRIO_SYSTEM  1   // use the system dns service of the router

/**
 * @brief DNS entry info
 *
 */
typedef struct {
    CHAR_T          *domain;// the domain of the DNS entry
    TUYA_IP_ADDR_T  ip;     // the ip address of the DNS entry
} TY_DNS_INFO_S;

/**
 * @brief get the domain ip address info by the domain name
 *
 * @param[in] domain the domain name
 * @param[out] addr the ip address of the domain
 * @return OPRT_OK: success, others: fail, please refer to tuya_error_code.h
 */
OPERATE_RET unw_gethostbyname(CONST CHAR_T *domain, TUYA_IP_ADDR_T *addr);

/**
 * @brief connect to a domain with specific tcp port
 *
 * @param[out] sockfd the socket fd
 * @param[in] hostname the domian name
 * @param[in] port the tcp port
 * @param[in] retry_cnt retry count if tcp connect failed
 * @param[in] timeout timeout in ms
 * @return OPRT_OK: success, others: fail, please refer to tuya_error_code.h
 */
OPERATE_RET unw_connect_to_domain(INT_T *sockfd, CHAR_T *hostname, UINT16_T port, UINT_T retry_cnt, UINT_T timeout);

/**
 * @brief init the dns module
 *
 * @return OPRT_OK: success, others: fail, please refer to tuya_error_code.h
 */
OPERATE_RET unw_init_dns_cache(VOID);

/**
 * @brief clean all dynamic dns cache
 *
 * @return OPRT_OK: success, others: fail, please refer to tuya_error_code.h
 */
OPERATE_RET unw_clear_all_dns_cache(VOID);

/**
 * @brief clear the specific domain dns cache
 *
 * @param[in] domain the domian which need to clear
 * @return OPRT_OK: success, others: fail, please refer to tuya_error_code.h
 */
OPERATE_RET  unw_clear_dns_cache(CONST CHAR_T *domain);

/**
 * @brief add dns information to dynamic dns cache
 *
 * @param[in] domain the domian name
 * @param[in] ip the ip address of the domain name
 * @return OPRT_OK: success, others: fail, please refer to tuya_error_code.h
 */
OPERATE_RET unw_add_dns_cache(CONST CHAR_T *domain, CONST TUYA_IP_ADDR_T ip);

/**
 * @brief set the dns priority
 *
 * @param[in] dns_prio the new dns priority
 * @return OPRT_OK: success, others: fail, please refer to tuya_error_code.h
 */
OPERATE_RET unm_set_dns_cache_priority(TY_DNS_PRIO_T dns_prio);

/**
 * @brief get the dns current priority
 *
 * @return REGION_E the current priority
 */
TY_DNS_PRIO_T unm_get_dns_cache_priority(VOID);

/**
 * @brief decrease the dns current priority
 *
 * @note if the priority is DNS_PRIO_SYSTEM, lower dns priority will return to DNS_PRIO_REGION
 */
VOID unm_lower_dns_cache_priority(VOID);

/**
 * @brief freeze the dns priority, no respond to priority change
 *
 * @param[in] dns_prio the new dns priority
 * @return OPRT_OK: success, others: fail, please refer to tuya_error_code.h
 */
OPERATE_RET unm_freeze_dns_cache_priority(TY_DNS_PRIO_T dns_prio);

#ifdef __cplusplus
}
#endif

#endif // __TUYA_BASE_UNI_DNS_CACHE_H__

