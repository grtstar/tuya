/**
 * @file tuya_ipc_ai_detect.h
 * @brief This is tuya ipc ai detect file
 * @version 1.0
 * @date 2022-12-09
 *
 * @copyright Copyright 2021-2031 Tuya Inc. All Rights Reserved.
 *
 */

#ifndef __TUYA_IPC_AI_DETECT_H__
#define __TUYA_IPC_AI_DETECT_H__

#include "tuya_cloud_types.h"
#include "cloud_operation.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  AI algorithms result. 
 * 
 */
typedef struct
{
    INT_T alg_id;
    BOOL_T positive;
}ALG_RET_T;

typedef struct
{
    INT_T ret_cnt;
    ALG_RET_T rets[0];
}AI_DETECT_RETS_T;

/**
 * @brief callback to update alg algorithms from cloud service.
 * 
 */
typedef VOID (*AI_ALG_UPDATE_CB)(AI_ALG_INFO_T *ai_alg_infos);

/**
 * @brief free callback when tuya free the pic.
 * 
 */
typedef VOID (*ON_PIC_FREE)(UCHAR_T **pic_buf, AI_DETECT_RETS_T **ai_rets_info);

/**
 * @brief get pic with ai alg results.
 * 
 */
typedef OPERATE_RET (*AI_DETECT_GET_AI_PIC_CB)(UCHAR_T **pic_buf, INT_T *len, AI_DETECT_RETS_T **ai_rets_info, ON_PIC_FREE *on_free);

/**
 * @brief callback to get ai detect results from cloud service.
 * 
 */
typedef VOID (*AI_DETECT_RET_CB)(AI_DETECT_RETS_T *ai_rets_info, CHAR_T* session_id);

/**
 * @brief Init ai detect function
 * 
 * @param VOID
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_ai_detect_init(AI_ALG_UPDATE_CB update_cb, AI_DETECT_GET_AI_PIC_CB get_pic_cb, AI_DETECT_RET_CB ret_cb);

/**
 * @brief Exit ai detect function
 * 
 * @param VOID
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_ai_detect_exit(VOID);

/**
 * @brief When the motion started, use this api to start ai detect, 
 *        to check is there any body or face in the pic. 
 * 
 * @param VOID
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_ai_detect_start(VOID);

/**
 * @brief When the motion stopped, use this api to stop detect
 * 
 * @param VOID
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_ai_detect_stop(VOID);

/**
 * \fn OPERATE_RET tuya_ipc_ai_detect_pause  
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
OPERATE_RET tuya_ipc_ai_detect_pause(VOID);

/**
 * @brief use api to resume ai detect
 * 
 * @param VOID
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_ai_detect_resume(VOID);


#ifdef __cplusplus
}
#endif

#endif