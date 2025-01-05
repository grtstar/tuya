/**
 * @file tuya_ipc_ai_detect_storage.h
 * @brief This is tuya ipc ai detect storage file
 * @version 1.0
 * @date 2021-11-17
 *
 * @copyright Copyright 2021-2031 Tuya Inc. All Rights Reserved.
 *
 */

#ifndef __TUYA_IPC_AI_DETECT_STORAGE_H__
#define __TUYA_IPC_AI_DETECT_STORAGE_H__

#include "tuya_ipc_ai_detect.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Init ai detect function
 * 
 * @param VOID
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
#define tuya_ipc_ai_detect_storage_init(VOID)  tuya_ipc_ai_detect_init(NULL, NULL, NULL)

/**
 * @brief Exit ai detect function
 * 
 * @param VOID
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
#define tuya_ipc_ai_detect_storage_exit(VOID) tuya_ipc_ai_detect_exit(VOID)

/**
 * @brief When the motion started, use this api to start ai detect, 
 *        to check is there any body or face in the pic. 
 * 
 * @param VOID
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
#define tuya_ipc_ai_detect_storage_start(VOID) tuya_ipc_ai_detect_start(VOID)

/**
 * @brief When the motion stopped, use this api to stop detect
 * 
 * @param VOID
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
#define tuya_ipc_ai_detect_storage_stop(VOID) tuya_ipc_ai_detect_stop(VOID)

/**
 * \fn OPERATE_RET tuya_ipc_ai_detect_storage_pause  
 * \brief  
 * \return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */

/**
 * @brief IN sleep mode or don't want to detect the pic, use this api to pause
 * 
 * @param VOID
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
#define tuya_ipc_ai_detect_storage_pause(VOID) tuya_ipc_ai_detect_pause(VOID)

/**
 * @brief use api to resume ai detect
 * 
 * @param VOID
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
#define tuya_ipc_ai_detect_storage_resume(VOID) tuya_ipc_ai_detect_resume(VOID)


#ifdef __cplusplus
}
#endif

#endif