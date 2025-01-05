/**
 * @file tuya_ipc_img_proc.h
 * @brief This is tuya ipc img proc file
 * @version 1.0
 * @date 2021-09-10
 *
 * @copyright Copyright 2021-2031 Tuya Inc. All Rights Reserved.
 *
 */

#ifndef __TUYA_IPC_IMG_PROC_H__
#define __TUYA_IPC_IMG_PROC_H__

#include "tuya_ipc_img_defs.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
    LINEAR,
    CUBIC,
} IMG_RESIZE_TYPE_E;

typedef struct {
    INT_T src_width;
    INT_T src_height;
    INT_T dst_width;
    INT_T dst_height;
    IMG_TYPE_E img_type;
    IMG_RESIZE_TYPE_E resize_type;
} TUYA_IMG_RESIZE_PARA_T;

typedef struct {
    INT_T num;
    INT_T* pclass;
    TUYA_AI_RECT_T* rect;
} TUYA_AI_SPEC_T;


/**
 * @brief YUV420 image scale interface
 * 
 * @param[in] in_data: input YUV420
 * @param[in] paras: scale struct
 * @param[out] out_data: output YUV420
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_img_resize(IN UCHAR_T *in_data, IN TUYA_IMG_RESIZE_PARA_T paras, OUT UCHAR_T *out_data);

/**
 * @brief yuv420 to b8g8r8
 * 
 * @param[in] y_img: input y image
 * @param[in] u_img: input u image 
 * @param[in] v_img: input v image 
 * @param[in] width: image width
 * @param[in] height: image height
 * @param[out] dst_B: output dest B
 * @param[out] dst_G: output dest G
 * @param[out] dst_R: output dest R
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_img_convert_yuv4202bgr888(IN UCHAR_T *y_img, IN UCHAR_T *u_img, IN UCHAR_T *v_img, IN INT_T width,
                                              IN INT_T height, OUT UCHAR_T *dst_B, OUT UCHAR_T *dst_G, OUT UCHAR_T *dst_R);

/**
 * @brief draw rect to Yuv
 * 
 * @param[in] y_img: input y image
 * @param[in] width: image width
 * @param[in] height: image height
 * @param[in] ai_spec: ai spec
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_img_draw_rect_yuv(IN UCHAR_T *y_img, IN INT_T width, IN INT_T height, IN TUYA_AI_SPEC_T ai_spec);

/**
 * @brief get Yuv of the first rect
 * 
 * @param[in] src_img: input src image
 * @param[in] width: image width
 * @param[in] height: image height
 * @param[in] type: IMG_TYPE_E
 * @param[in] ai_spec: ai spec
 * @param[out] dst_img: out dest image
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_img_get_rect(IN UCHAR_T *src_img, IN INT_T width, IN INT_T height, IN IMG_TYPE_E type, IN TUYA_AI_SPEC_T ai_spec, OUT UCHAR_T *dst_img);

/**
 * @brief ARGB1555 OSD 
 * 
 * @param[in] src_img: input data and output data, RGB24
 * @param[in] width: width of srcImg
 * @param[in] height: height of srcImg
 * @param[out] osd_img: osd argb1555 data
 * @param[out] osd_rect: chan0 0,0,640,64	chan1 0,0,160,16
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET tuya_ipc_img_osd_ARGB1555(IN UCHAR_T *src_img, IN INT_T width, IN INT_T height, OUT UINT16_T *osd_img, OUT TUYA_AI_RECT_T osd_rect);


#ifdef __cplusplus
}
#endif

#endif
