/**
 * \file config.h
 *
 * \brief Configuration options (set of defines)
 *
 *  This set of compile-time options may be used to enable
 *  or disable features selectively, and reduce the global
 *  memory footprint.
 */
/*
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

#ifndef MBEDTLS_CONFIG_H
#define MBEDTLS_CONFIG_H

#include "tuya_iot_config.h"
#include "compat-2.x.h"

#ifndef MAX_SECURITY_LEVEL
#define MAX_SECURITY_LEVEL TUYA_SECURITY_LEVEL
#elif TUYA_SECURITY_LEVEL > MAX_SECURITY_LEVEL
    #error TUYA_SECURITY_LEVEL > MAX_SECURITY_LEVEL
#endif

#if defined(MAX_SECURITY_LEVEL)  && (MAX_SECURITY_LEVEL == 0)
    #include "tuya_tls_security_L0_default_config.h"
#endif
#if defined(MAX_SECURITY_LEVEL)  && (MAX_SECURITY_LEVEL == 1)
    #include "tuya_tls_security_L1_default_config.h"
#endif

#if defined(MAX_SECURITY_LEVEL)  && (MAX_SECURITY_LEVEL == 2)
    #include "tuya_tls_security_L2_default_config.h"
#endif

#if defined(MAX_SECURITY_LEVEL)  && (MAX_SECURITY_LEVEL == 3)
    #include "tuya_tls_security_L3_default_config.h"
#endif

#include "check_config.h"

#endif /* MBEDTLS_CONFIG_H */
