/**
 * @file tuya_ipc_video_proc.h
 * @brief This is tuya ipc video proc file
 * @version 1.0
 * @date 2021-09-10
 *
 * @copyright Copyright 2021-2031 Tuya Inc. All Rights Reserved.
 *
 */

#ifndef __TUYA_IPC_VIDEO_PROC_H__
#define __TUYA_IPC_VIDEO_PROC_H__

#include "tuya_ipc_img_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TUYA_MD_MULTI_ZONE_NUM (4)       ///< 最多支持4个区域配置
#define TUYA_MD_POLYGON_NUM (8)          ///< 最多支持8边形
#define TUYA_MD_MULTI_POLYGON_NUM (4)    ///< 最多支持4个多边形

///< setting region type MD
typedef enum {
    SINGLE = 1,         ///< 单区域
    MULTI,              ///< 多区域，矩阵表格类型，如检测5x5 4x3区域
    MULTI_ZONE,         ///< 多区域，支持多个自定义框配置
    MULTI_POLYGON,      ///< 多区域，多边形
} TUYA_MD_REGION_TYPE_E;

typedef struct {
    char x;      ///< 起点X   【0-100】
    char y;      ///< 起点Y   【0-100】
    char width;  ///< 宽度    【0-100】
    char height; ///< 高度    【0-100】
} TUYA_MD_ZONE_T;

typedef struct {
    int num;                                       ///< 检测区域个数
    TUYA_MD_ZONE_T zone[TUYA_MD_MULTI_ZONE_NUM];   ///<
} TUYA_MULTI_ZONE_INFO_T;

typedef struct {
    char x;      ///< X   【0-100】
    char y;      ///< Y   【0-100】
} TUYA_MD_POINT_T;

typedef struct {
    int num;                                       ///< 顶点个数
    TUYA_MD_POINT_T point[TUYA_MD_POLYGON_NUM];
} TUYA_MD_POLYGON_T;

typedef struct {
    INT_T polygon_num;                                                  ///< 多边形个数，最多支持4个
    TUYA_MD_POLYGON_T polygon_list[TUYA_MD_MULTI_POLYGON_NUM];          ///< 多边形信息
} TUYA_MULTI_POLYGON_INFO_T;

///< multi_regions when setting multi md region, for example 5x5 4x3
typedef struct {
    INT_T rows;
    INT_T cols;
    INT_T region_num;
    TUYA_AI_RECT_T *region_list; ///< rect data
} TUYA_MULTI_MD_REGION_T;

typedef struct {
    INT_T frame_w; ///< Input width
    INT_T frame_h; ///< Input height

    INT_T y_thd;       ///< Thres value of Motion Detect, default 30, set 5 when low-light, recommend 5-30
    INT_T sensitivity; ///< Sensitivity of Motion Detect，range 1-10, more sensitive

    TUYA_MD_REGION_TYPE_E rect_type;      ///< use NONE when set no rect; use TUYA_RPERCENT_RECT_T when set SINGLE; use
                                          ///< TUYA_MULTI_MD_REGION_T when set MULTI
    TUYA_RPERCENT_RECT_T roi;             ///< ROI of Motion Detect
    TUYA_MULTI_MD_REGION_T motion_region; ///< MULTI ROI of Motion Detect; range 1x1 - 5x5
    INT_T tracking_enable;                ///< Switch of Motion Tracking，O close ,1 open
    TUYA_MULTI_ZONE_INFO_T multi_zone;
    TUYA_MULTI_POLYGON_INFO_T multi_polygon;
} TUYA_MOTION_TRACKING_CFG_T;

/**
 * @brief Init input config
 *
 * @param[in] mt_cfg: motion config
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_ipc_motion_init(IN TUYA_MOTION_TRACKING_CFG_T mt_cfg);

/**
 * @brief Set config dynamically
 *
 * @param[in] mt_cfg: input motion config
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_ipc_set_motion(IN TUYA_MOTION_TRACKING_CFG_T mt_cfg);

/**
 * @brief Get config dynamically
 *
 * @param[out] mt_cfg: output motion config
 *
 * @return VOID
 */
VOID tuya_ipc_get_motion(OUT TUYA_MOTION_TRACKING_CFG_T *mt_cfg);

/**
 * @brief release
 *
 * @param VOID
 *
 * @return VOID
 */
VOID tuya_ipc_motion_release(VOID);

/**
 * @brief get a background frame
 *
 * @param VOID
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_ipc_get_background_frame(VOID);

/**
 * @brief do background sub
 *
 * @param[out] background_motion_flag: motion(1) or not(0)
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_ipc_background_sub(OUT INT_T *background_motion_flag);

/**
 * @brief Execute Motion Detect \ Motion Tracking
 *
 * @param[in] in_data: Input YUV
 * @param[out] motion_flag: Return the value of Motion Detect. 0 for no moving; 1 for moving exist
 * @param[out] motion_point: Return the center point coordinate of the largest moving object
 *                           Both values(x, y) are 0 for no moving; otherwise, moving exist
 *                           Return values(0, 0) when tracking_enable==0
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_ipc_motion(IN UCHAR_T *in_data, OUT INT_T *motion_flag, OUT TUYA_POINT_T *motion_point);

#ifdef __cplusplus
}
#endif

#endif
