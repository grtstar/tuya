 /**
 * @file tkl_video_enc.h
 * @brief video encode
 * @version 0.1
 * @date 2021-11-04
 *
 * @copyright Copyright 2019-2021 Tuya Inc. All Rights Reserved.
 *
 * Set video parameters and get video bitstream.
 */

#ifndef __TKL_VIDEO_ENC_H__
#define __TKL_VIDEO_ENC_H__

#include "tuya_cloud_types.h"
#include "tkl_video_in.h"

#ifdef __cplusplus
    extern "C" {
#endif

typedef enum
{
    TKL_VENC_0 = 0,
    TKL_VENC_1,
    TKL_VENC_2,
    TKL_VENC_3,
    TKL_VENC_4,
    TKL_VENC_5,
    TKL_VENC_6,
    TKL_VENC_7,
    TKL_VENC_MAX,
}TKL_VENC_CHN_E;

typedef enum
{
    TKL_VENC_MAIN = 0,                                   // main stream
    TKL_VENC_SUB,                                        // sub stream
    TKL_VENC_3RD,                                        // 3rd stream
    TKL_VENC_4TH,                                        // 4th stream
    TKL_VENC_SNAP,                                       // snap
    TKL_VENC_MD,                                         // move detect
    TKL_VENC_HD,                                         // human detect
    TKL_VENC_QR,                                         // qrcode
    TKL_VENC_TYPE_MAX,
}TKL_VENC_TYPE_E;                                        // stream work type

typedef struct
{
    TKL_MEDIA_FRAME_TYPE_E frametype;                    // bitrate,kbps
    TKL_MEDIA_CODEC_TYPE_E codectype;                    // codec type
    CHAR_T    *pbuf;                                     // frame buffer
    UINT_T     buf_size;                                 // buffer size
    UINT_T     used_size;                                // used buffer size
    UINT_T     width;                                    // frame width
    UINT_T     height;                                   // frame height
    UINT64_T   pts;                                      // sdk pts
    UINT64_T   timestamp;                                // system utc time，unit: ms
    UINT_T seq;                                          // frame sequence number
    UINT_T fragment;                                     // frame sequence's fragment  第几个分包
    BYTE_T seq_error;                                    // frame sequence is error  这张图片是否错误
    BYTE_T fragment_is_last;                             // frame sequence's fragment is last fragment
}TKL_VENC_FRAME_T;                                       // video frame

typedef INT_T (*TKL_VENC_PUT_CB)(TKL_VENC_FRAME_T *pframe);

typedef enum
{
    TKL_VENC_UPPER_REQUEST_FRAME = 0,                    // app get frame by using get frame function
    TKL_VENC_PUSH_FRAME_BY_CALLBACK,                     // tkl push frame to app by callback when frame arrive
}TKL_VENC_DATA_TRANS_MODE_E;

typedef struct
{
    UINT_T left;                                         // osd 左上角的位置x坐标
    UINT_T top;                                          // osd 左上角的位置y坐标
    UINT_T font_w;                                       // osd 字体大小,字体的宽
    UINT_T font_h;                                       // osd 字体大小,字体的高
}TKL_VENC_OSD_CONFIG_T;                                  // osd 属性配置

typedef struct
{
    UINT_T enable;                                       // 1,enable,0,disable
    TKL_VENC_CHN_E chn;                                  // video encode channel
    TKL_VENC_TYPE_E type;                                // stream work type
    TKL_MEDIA_CODEC_TYPE_E codectype;                    // codec type
    UINT_T fps;                                          // fps
    UINT_T gop;                                          // I Frame interval
    UINT_T bitrate;                                      // bitrate,kbps
    UINT_T width;
    UINT_T height;
    UINT_T min_qp;
    UINT_T max_qp;
    TKL_VENC_DATA_TRANS_MODE_E trans_mode;
    TKL_VENC_PUT_CB put_cb;
    TKL_VENC_OSD_CONFIG_T osd;
}TKL_VENC_CONFIG_T;

typedef struct
{
    INT32_T enable;
    INT32_T is_dls;
}TKL_VENC_OSD_T;

typedef enum {
    TKL_VENC_IDR_REQUEST = 0,                              // 在最近的编码，请求IDR帧编码
    TKL_VENC_IDR_FLUSH_STREAM,                             // 刷掉编码器里残留的旧码流，并以IDR帧开始编码
    TKL_VENC_IDR_TYPE_MAX,
} TKL_VENC_IDR_E;

typedef struct
{
    UINT_T enable;                                       // 0, 不显示遮挡; 1, 区域遮挡
    UINT_T id;                                           // 遮挡编号 
    UINT_T color;                                        // 遮挡的颜色，ARGB8888类型
    FLOAT_T x;                                           // 矩形区域左上点水平方向上的坐标, [0~100]
    FLOAT_T y;                                           // 矩形区域左上点垂直方向上的坐标, [0~100]
    FLOAT_T width;                                       // 矩形区域的宽度, [0~100]
    FLOAT_T height;                                      // 矩形区域的高度, [0~100]
}TKL_VENC_MASK_T;

typedef struct
{
    UINT32_T pool_buff_num;
    UINT32_T pool_buff_size;
}TKL_VENC_STREAM_BUFF_T;

/**
* @brief video encode init
* 
* @param[in] vi_chn: vi channel number
* @param[in] pconfig: venc config
* @param[in] count: count of pconfig
*
* @return OPRT_OK on success. Others on error, please refer to tkl_error_code.h
*/
OPERATE_RET tkl_venc_init(INT32_T vi_chn, TKL_VENC_CONFIG_T *pconfig, INT32_T count);

/**
* @brief video encode get frame
* 
* @param[in] vi_chn: vi channel number
* @param[in] venc_chn: venc channel number
* @param[out] pframe:  output frame
*
* @return OPRT_OK on success. Others on error, please refer to tkl_error_code.h
*/
OPERATE_RET tkl_venc_get_frame(TKL_VI_CHN_E vi_chn, TKL_VENC_CHN_E venc_chn, TKL_VENC_FRAME_T *pframe);

/**
* @brief video first snap
* 
* @param[in] vi_chn: vi channel number
* @param[out] pframe: output frame
*
* @return OPRT_OK on success. Others on error, please refer to tkl_error_code.h
*/
OPERATE_RET tkl_venc_get_first_snap(TKL_VI_CHN_E vi_chn, TKL_VENC_FRAME_T *pframe);

/**
* @brief video encode set osd
* 
* @param[in] vi_chn: vi channel number
* @param[in] venc_chn: venc channel number
* @param[out] posd:  osd config
*
* @return OPRT_OK on success. Others on error, please refer to tkl_error_code.h
*/
OPERATE_RET tkl_venc_set_osd(TKL_VI_CHN_E vi_chn, TKL_VENC_CHN_E venc_chn, TKL_VENC_OSD_T *posd);

/**
* @brief video encode set osd
* 
* @param[in] vi_chn: vi channel number
* @param[in] venc_chn: venc channel number
* @param[in] idr_type: request idr type
*
* @return OPRT_OK on success. Others on error, please refer to tkl_error_code.h
*/
OPERATE_RET tkl_venc_set_IDR(TKL_VI_CHN_E vi_chn, TKL_VENC_CHN_E venc_chn,  TKL_VENC_IDR_E idr_type);

/**
* @brief video encode set mask
* 
* @param[in] vi_chn: vi channel number
* @param[in] venc_chn: venc channel number
* @param[out] pmask: mask config
*
* @return OPRT_OK on success. Others on error, please refer to tkl_error_code.h
*/
OPERATE_RET tkl_venc_set_mask(TKL_VI_CHN_E vi_chn, TKL_VENC_CHN_E venc_chn, TKL_VENC_MASK_T *pmask);

/**
* @brief video encode stream buff pool set
* 
* @param[in] vi_chn: vi channel number
* @param[in] venc_chn: venc channel number
* @param[in] parg:  buff pool config
*
* @return OPRT_OK on success. Others on error, please refer to tkl_error_code.h
*/
OPERATE_RET tkl_venc_set_video_stream_buffer(TKL_VI_CHN_E vi_chn, TKL_VENC_CHN_E venc_chn, TKL_VENC_STREAM_BUFF_T *parg);

/**
* @brief video encode  start
* 
* @param[in] vi_chn: vi channel number
* @param[in] venc_chn: venc channel number
*
* @return OPRT_OK on success. Others on error, please refer to tkl_error_code.h
*/
OPERATE_RET tkl_venc_start(TKL_VI_CHN_E vi_chn, TKL_VENC_CHN_E venc_chn);

/**
* @brief video encode  stop
* 
* @param[in] vi_chn: vi channel number
* @param[in] venc_chn: venc channel number
*
* @return OPRT_OK on success. Others on error, please refer to tkl_error_code.h
*/
OPERATE_RET tkl_venc_stop( TKL_VI_CHN_E vi_chn, TKL_VENC_CHN_E venc_chn);

/**
* @brief video encode uninit
* 
* @param[in] vi_chn: vi channel number
*
* @return OPRT_OK on success. Others on error, please refer to tkl_error_code.h
*/
OPERATE_RET tkl_venc_uninit(TKL_VI_CHN_E vi_chn);

#ifdef __cplusplus
}
#endif

#endif
