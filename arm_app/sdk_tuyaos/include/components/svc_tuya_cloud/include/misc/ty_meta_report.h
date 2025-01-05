/**
 * @file ty_meta_report.h
 * @brief meta report
 * @version 0.1
 * @date 2022-09-22
 * 
 * @copyright Copyright (c) 2021-2022 Tuya Inc. All Rights Reserved.
 * 
 * Permission is hereby granted, to any person obtaining a copy of this software and
 * associated documentation files (the "Software"), Under the premise of complying 
 * with the license of the third-party open source software contained in the software,
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software.
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * 
 */

#ifndef __TY_META_REPORT_H__
#define __TY_META_REPORT_H__

#include "tuya_cloud_types.h"
#include "ty_cJSON.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Definition of metas report mode
 */
typedef enum {
    REPORT_MODE_DEFAULT, // if values already reported, then no more report
    REPORT_MODE_FORCE,   // force report even if values alread reported
} META_REPORT_MODE_E;

/**
 * @brief Init meta report
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET ty_meta_init(VOID);

/**
 * @brief Report meta capability to cloud
 *
 * @param[in] metas Meta capability, i.e {k1:v1, k2:v2}
 * @param[in] mode Report mode, @see META_REPORT_MODE_E
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET ty_meta_report(ty_cJSON *meta, META_REPORT_MODE_E mode);

#ifdef __cplusplus
}
#endif

#endif // __TY_META_REPORT_H__