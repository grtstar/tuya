/**
 * @file tuya_ipc_img_defs.h
 * @brief This is tuya ipc img defs file
 * @version 1.0
 * @date 2021-09-10
 *
 * @copyright Copyright 2021-2031 Tuya Inc. All Rights Reserved.
 *
 */

#ifndef __TUYA_IPC_IMG_DEFS_H__
#define __TUYA_IPC_IMG_DEFS_H__

#include "tuya_cloud_types.h" 
#include "tuya_error_code.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef enum{
    Y = 1,
    YUV,
    UYUV,
    BGR888,
    RGB888,
    ARBG1555,
} IMG_TYPE_E;
    
typedef enum {
    TUYA_U8 =1,
    TUYA_U16,
    TUYA_S32,
    TUYA_U32,
    TUYA_S64,

    TUYA_3U8,
    TUYA_3U16,
    TUYA_3S32,
    TUYA_3U32,
    TUYA_3S64,
} CHANNEL_TYPE_E;

typedef struct {
    INT_T x;
    INT_T y;
} TUYA_POINT_T;

typedef struct {
    INT_T x_percent;      
    INT_T y_percent;      
    INT_T width_percent;  
    INT_T height_percent; 
} TUYA_RPERCENT_RECT_T;

typedef struct {
    INT_T left;
    INT_T top;
    INT_T right;
    INT_T bottom;
} TUYA_AI_RECT_T;


#ifdef __cplusplus
}
#endif

#endif
