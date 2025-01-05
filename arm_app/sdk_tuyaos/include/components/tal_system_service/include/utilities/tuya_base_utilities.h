/**
 * @file tuya_base_utilities.h
 * @brief tuya utilities module
 * @version 1.0
 * @date 2019-10-30
 * 
 * @copyright Copyright (c) tuya.inc 2019
 * 
 */
#ifndef _TUYA_BASE_UTILITIES_H
#define _TUYA_BASE_UTILITIES_H

#include "tal_memory.h"
#include "cJSON.h"
#include "ty_cJSON.h"
#include "tuya_list.h"
#include "tuya_smartpointer.h"
#include "uni_queue.h"
#include "tuya_slist.h"
#include "tuya_hlist.h"
#include "tal_time_service.h"
#include "tal_thread.h"
#include "uni_network.h"
#include "mix_method.h"
#include "tuya_hashmap.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief utilites initialization
 * 
 * @return OPRT_OK on success, others on failed, please refer to tuya_error_code.h  
 */
OPERATE_RET tuya_base_utilities_init(VOID);


#ifdef __cplusplus
}
#endif


#endif

