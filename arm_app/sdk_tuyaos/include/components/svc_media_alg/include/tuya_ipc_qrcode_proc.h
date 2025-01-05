/**
 * @file tuya_ipc_qrcode_proc.h
 * @brief This is tuya ipc qrcode proc file
 * @version 1.0
 * @date 2021-09-10
 *
 * @copyright Copyright 2021-2031 Tuya Inc. All Rights Reserved.
 *
 */

#ifndef __TUYA_IPC_QRCODE_PROC_H__
#define __TUYA_IPC_QRCODE_PROC_H__

#include "tuya_ipc_img_defs.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Tuya qrcode image enhance
 * 
 * @param[in] in_data: input YUV420 ,from platform decode
 * @param[in] in_w: input width	,from platform decode
 * @param[in] in_h: input height ,from platform decode
 * @param[out] out_data: output YUV420 ,send in zbar
 * @param[out] out_w: output width ,send in zbar
 * @param[out] out_h: output height ,send in zabr
 * @param[in] binary_thres: image binary threshold，depends on different ISP para, binary data range: 100-150, recommend 128.
 * @param[in] scale_flag: image scale, needed when Low-resolution images
 *                        1: scale; 0: no scale, if origin size 16*9, the output will be 32*18 when seting 1
 *                        recommend original size 640*360, slower when scale to 1280*720.
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_qrcode_enhance(IN UCHAR_T *in_data, IN INT_T in_w, IN INT_T in_h, 
                                    OUT UCHAR_T **out_data, OUT INT_T* out_w, OUT INT_T* out_h, 
                                    IN INT_T binary_thres, IN BOOL_T scale_flag);


#ifdef __cplusplus
}
#endif

#endif
