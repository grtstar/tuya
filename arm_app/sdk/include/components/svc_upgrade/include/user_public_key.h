/**
* @file tuya_svc_upgrade.h
* @brief TUYA OTA service
* @version 0.1
* @date 2019-08-28
*
* @copyright Copyright 2019-2021 Tuya Inc. All Rights Reserved.
*/

#ifndef __TUYA_USER_PUBLIC_KEY_H__
#define __TUYA_USER_PUBLIC_KEY_H__

#include"stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(ENABLE_FIRMWARE_SIGN_USER) && (ENABLE_FIRMWARE_SIGN_USER == 1)
extern  int32_t load_sign_user_pub_key(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
