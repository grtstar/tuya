/**
 * @file tuya_ipc_doorbell.h
 * @brief This is tuya ipc doorbell file
 * @version 1.0
 * @date 2021-11-17
 *
 * @copyright Copyright 2021-2031 Tuya Inc. All Rights Reserved.
 *
 */

#ifndef __TUYA_IPC_DOORBELL_H__
#define __TUYA_IPC_DOORBELL_H__

#include "tuya_cloud_types.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * callback to handle event from App, such as answer/hangup eg.
 * paramter 'action' is one of these: ("accept"/"stop"/"hearbeat")
 */
typedef VOID (*TUYA_IPC_DOORBELL_EVENT_CB)(CHAR_T* action);


typedef struct {
    TUYA_IPC_DOORBELL_EVENT_CB doorbell_event_cb; ///< event callback
} TUYA_IPC_DOORBELL_ENV_T;


OPERATE_RET mqc_door_bell_press(IN TIME_T current_time, IN CHAR_T* suffix);

OPERATE_RET mqc_door_bell_press_generic(IN CHAR_T* etype, IN TIME_T current_time, IN CHAR_T* suffix);

OPERATE_RET mqc_door_bell_answer(CHAR_T *extra_data);

OPERATE_RET mqc_dev_door_bell_press_generic(IN CHAR_T* node_id, IN CHAR_T* etype, IN TIME_T current_time, IN CHAR_T* suffix);

/**
 * @brief init ac doorbell
 * 
 * @param[in] p_var: if you want to handle App's event such as answer/hangup eg. you need set 'doorbell_event_cb'
 * 
 * @return OPERATE_RET 
 */
OPERATE_RET tuya_ipc_doorbell_init(IN TUYA_IPC_DOORBELL_ENV_T* p_var);

#ifdef __cplusplus
}
#endif

#endif

