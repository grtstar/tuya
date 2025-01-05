/**
 * @file tuya_ipc_log.h
 * @brief This is tuya ipc log file
 * @version 1.0
 * @date 2021-11-17
 *
 * @copyright Copyright 2021-2031 Tuya Inc. All Rights Reserved.
 *
 */

#ifndef __TUYA_IPC_LOG_H__
#define __TUYA_IPC_LOG_H__
#include <string.h>
#include <stdio.h>
#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TY_IPC_LOG_LEVEL_ERR                     0
#define TY_IPC_LOG_LEVEL_WARN                    1
#define TY_IPC_LOG_LEVEL_NOTICE                  2
#define TY_IPC_LOG_LEVEL_INFO                    3
#define TY_IPC_LOG_LEVEL_DEBUG                   4
#define TY_IPC_LOG_LEVEL_TRACE                   5

/**
 * @brief set tuya SDK log level and destination
 * 
 * @param[in] log_level: TY_IPC_LOG_LEVEL_ERR ... (_WARN,_NOTICE,_INFO,_DEBUG,_TRACE)
 * @param[in] filename: if need to print to filesystem
 * 
 * @return OPERATE_RET 
 */
OPERATE_RET tuya_ipc_set_log_attr(IN CONST INT_T log_level, IN CHAR_T *filename);

/**
 * @brief set tuya SDK log level,destination and if_ms_level
 * 
 * @param log_level: TY_IPC_LOG_LEVEL_ERR ... (_WARN,_NOTICE,_INFO,_DEBUG,_TRACE) 
 * @param filename: FILE*, User fopen file first, and give fp to TUYA SDK 
 * @param if_ms_level: the log time with ms
 * 
 * @return OPERATE_RET 
 */
OPERATE_RET tuya_ipc_log_setting(IN CONST INT_T log_level, FILE *filename, BOOL_T if_ms_level);


#ifdef __cplusplus
}
#endif

#endif
