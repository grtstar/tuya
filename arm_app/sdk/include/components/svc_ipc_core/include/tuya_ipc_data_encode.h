/*
 * tuya_ipc_data_encode.h
 *Copyright(C),2017-2022, TUYA company www.tuya.com
 *
 *FILE description:
  *
 *  Created on: 2021年8月30日
 *      Author: kuiba
 */

#ifndef TY_IPC_WR_WL_LINUX_SDK_SDK_SVC_IPC_CORE_INCLUDE_TUYA_IPC_DATA_ENCODE_H_
#define TY_IPC_WR_WL_LINUX_SDK_SDK_SVC_IPC_CORE_INCLUDE_TUYA_IPC_DATA_ENCODE_H_

#include"tuya_iot_config.h"
#include "tuya_cloud_types.h"
#ifdef __cplusplus
extern "C"{
#endif
//本地存储和发送给APP数据统一按照小端存取。使用时，根据大小端具体机型进行转换
//当前涉及组件有 svc_local_storage svc_ai_detect_storage svc_enc_pic
#if defined(LITTLE_END) && (LITTLE_END == 1)
#define UNI_HTOLES(X) X
#define UNI_HTOLEL(X) X
#define UNI_HTOLELL(X) X
#define UNI_LETOHS(X) X
#define UNI_LETOHL(X) X
#define UNI_LETOHLL(X) X
#else
#define UNI_HTOLES(X) WORD_SWAP(X)
#define UNI_HTOLEL(X) DWORD_SWAP(X)
#define UNI_HTOLELL(X) DDWORD_SWAP(X)
#define UNI_LETOHS(X) WORD_SWAP(X)
#define UNI_LETOHL(X) DWORD_SWAP(X)
#define UNI_LETOHLL(X) DDWORD_SWAP(X)
#endif


#ifdef __cplusplus
}
#endif




#endif /* TY_IPC_WR_WL_LINUX_SDK_SDK_SVC_IPC_CORE_INCLUDE_TUYA_IPC_DATA_ENCODE_H_ */
