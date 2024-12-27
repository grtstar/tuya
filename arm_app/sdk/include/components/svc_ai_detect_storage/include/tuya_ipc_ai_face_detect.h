/**
 * @file tuya_ipc_ai_face_detect.h
 * @brief This is tuya ipc ai face detect file
 * @version 1.0
 * @date 2021-11-17
 *
 * @copyright Copyright 2021-2031 Tuya Inc. All Rights Reserved.
 *
 */

#ifndef __TUYA_IPC_AI_FACE_DETECT_H__
#define __TUYA_IPC_AI_FACE_DETECT_H__

#include "tuya_ipc_cloud_storage.h"


#ifdef __cplusplus
extern "C" {
#endif


#define FACE_PIC_MAX_SIZE (128*1024)


typedef VOID (*TUYA_IPC_AI_FACE_DETECT_CB)(IN CONST CHAR_T* action, IN CONST CHAR_T* face_id_list);

typedef VOID (*TUYA_IPC_AI_FACE_DOWNLOAD_PIC_CB)(OUT PVOID_T callback_data);


typedef struct
{
    INT_T       is_strang_flag;         ///< flag of human, 0 strange, 1 marked
    INT_T       face_score;             ///< score of face
    INT_T       face_id;                ///< face id, unique, creat by tuya service

    CHAR_T*     face_jpeg_data;         ///< face image
    INT_T       face_jpeg_data_size;    ///< face image size
    
    CHAR_T*     scene_jpeg_data;        ///< scene image
    INT_T       scene_jpeg_data_size;   ///< scene image size
    
}TUYA_AI_FACE_UPLOAD_PARA_T;

typedef struct
{
    INT_T       face_id;    ///< face id, unique, creat by tuya service
    CHAR_T      face_jpeg_data[FACE_PIC_MAX_SIZE];  ///< face image
    UINT_T      face_jpeg_data_size;    ///< face image size
}TUYA_AI_FACE_DATA_T;


/**
 * @brief Init ai face detect
 * 
 * @param[in] cb: callback to set ai_cfg of callback[del add update]
 * @param[in] pic_download_cb: callback to set pic download of callback[download pic circularly]
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_ai_face_detect_storage_init(IN TUYA_IPC_AI_FACE_DETECT_CB cb, IN TUYA_IPC_AI_FACE_DOWNLOAD_PIC_CB pic_download_cb);

/**
 * @brief device upload signed face pic to service
 * 
 * @param[in] ai_face_image 
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_ai_face_signed_report(IN TUYA_AI_FACE_UPLOAD_PARA_T *ai_face_image);

/**
 * @brief device upload new face pic to service, service will send face_id to user
 * 
 * @param[in] ai_face_image 
 * @param[out] face_id 
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_ai_face_new_report(IN TUYA_AI_FACE_UPLOAD_PARA_T *ai_face_image, OUT UINT_T *face_id);

/**
 * @brief  this func used to update all the AI face picture in the cloud.
 *         tuya sdk will callback the picture data in TUYA_IPC_AI_FACE_DOWNLOAD_PIC_CB set in
 *         tuya_ipc_ai_face_detect_storage_init circularly
 *         this func is blocked until all the picture done callback to user
 * 
 * @param VOID
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_ai_face_update_cloud_face(VOID);


#ifdef __cplusplus
}
#endif

#endif

