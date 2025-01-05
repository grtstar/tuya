/**
 * @file tuya_ipc_ai_face_db.h
 * @brief This is tuya ipc ai face db file
 * @version 1.0
 * @date 2021-11-17
 *
 * @copyright Copyright 2021-2031 Tuya Inc. All Rights Reserved.
 *
 */

#ifndef __TUYA_IPC_AI_FACE_DB_H__
#define __TUYA_IPC_AI_FACE_DB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "tuya_ipc_cloud_storage.h"

#define DB_FILE_PATH 128
#define DB_FILE_NAME 6

typedef struct {
    CHAR_T* face_image_add;
    INT_T face_id;
} AI_FACE_DB_DATA_T;

/**
 * @brief user should set the path of face feature db saved locally, also set the lenth of face feature db
 * 
 * @param[in] p_path: local path to save
 * @param[in] feature_len: length of face feature db
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_ai_face_db_init(IN CHAR_T* p_path, IN INT_T feature_len);

/**
 * @brief add face feature db ,send in face_id and data
 * 
 * @param[in] p_face_id: face id
 * @param[in] data: data
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_ai_face_db_add(IN CHAR_T* p_face_id ,IN PVOID_T data);

/**
 * @brief delete face feature db, send int face_id
 * 
 * @param[in] face_id 
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_ai_face_db_delete(IN CHAR_T* face_id);

/**
 * @brief get face feature db, send in face_id
 * 
 * @param[in] p_face_id 
 * @param[out] p_data 
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_ai_face_db_read(IN CHAR_T* p_face_id, OUT PVOID_T p_data);

/**
 * @brief get the path of face feature db set in tuya_ipc_ai_face_db_init
 * 
 * @param[in] p_path 
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_ai_face_get_db_path(IN CHAR_T* p_path);


#ifdef __cplusplus
}
#endif

#endif

