/**
 * @file tuya_ipc_mp4_convertor.h
 * @brief This is tuya ipc mp4 convertor file
 * @version 1.0
 * @date 2021-11-17
 *
 * @copyright Copyright 2021-2031 Tuya Inc. All Rights Reserved.
 *
 */

#ifndef __TUYA_IPC_MP4_CONVERTOR_H__
#define __TUYA_IPC_MP4_CONVERTOR_H__


#ifdef __cplusplus
extern "C" {
#endif


#ifdef  SS_SUPPORT_MP4

typedef struct __mp4_convertor SS_MP4_CONVETOR;

INT_T tuya_ipc_mp4_create(IN SS_MP4_CONVETOR** p_mp4_convertor, IPC_MEDIA_INFO_T* p_media_info);

INT_T tuya_ipc_mp4_start(IN SS_MP4_CONVETOR* p_mp4_convertor, CHAR_T * file_path);

INT_T tuya_ipc_mp4_stop(IN SS_MP4_CONVETOR* p_mp4_convertor);

INT_T tuya_ipc_mp4_destroy(IN SS_MP4_CONVETOR* p_mp4_convertor);

INT_T tuya_ipc_mp4_append_stream(IN SS_MP4_CONVETOR* p_mp4_convertor, MEDIA_FRAME_TYPE_E type, BYTE_T *p_buf, UINT_T size);

#endif


#ifdef __cplusplus
}
#endif

#endif
