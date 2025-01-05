/** @file tuya_tmm_link.h
 * @brief contains link protocol between devices
 */
#ifndef __TUYA_TMM_LINK_H__
#define __TUYA_TMM_LINK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "tuya_ipc_media.h"
#include "tuya_list.h"
#include "tuya_cloud_com_defs.h"

/** @enum TMM_TRANS_TYPE_E
 * @brief the transfer type of media data
 */
typedef enum
{
    TMM_TRANS_P2P,  ///<transfer over p2p
    TMM_TRANS_CGW   ///<transfer over media gateway
}TMM_TRANS_TYPE_E;

/** @enum TMM_MEDIA_CTL_FLAG_E
 * @brief the flag to control media transfer, which can be composed by bit
 */
typedef enum {
    TMM_SEND_VIDEO = 1,          ///<send video to peer
    TMM_SEND_AUDIO = 1 << 1,     ///<send audio to peer
    TMM_RECV_VIDEO = 1 << 2,     ///<receive video from peer
    TMM_RECV_AUDIO = 1 << 3,     ///<receive audio from peer
} TMM_MEDIA_CTL_FLAG_E;

/** @enum TMM_STREAM_TYPE_E
 * @brief stream type, used to describe the stream ability of a device, can be composed by bit
 */
typedef enum {
    TMM_STREAM_TYPE_VIDEO_1ST = 1 << 1,   ///<the first video stream
    TMM_STREAM_TYPE_VIDEO_2ND = 1 << 2,   ///<the second video stream
    TMM_STREAM_TYPE_VIDEO_3RD = 1 << 3,   ///<the third video stream
    TMM_STREAM_TYPE_VIDEO_4TH = 1 << 4,   ///<the forth video stream
    TMM_STREAM_TYPE_AUDIO_1ST = 1 << 0,   ///<the first audio stream
    TMM_STREAM_TYPE_AUDIO_2ND = 1 << 5,   ///<the second audio stream
    TMM_STREAM_TYPE_AUDIO_3RD = 1 << 6,   ///<the third audio stream
    TMM_STREAM_TYPE_AUDIO_4TH = 1 << 7,   ///<the forth audio stream
} TMM_STREAM_TYPE_E;

/** @name TMM_STREAM_DESC_T
 * @brief the media stream ability
 */
typedef struct
{
    UINT_T streamType;  ///<stream ability
}TMM_STREAM_DESC_T;


/** @name TMM_ASSOC_DEV_T
 * @brief the info of a peer device
 */
typedef struct
{
    CHAR_T dev_id[DEV_ID_LEN + 1];       ///<device id of the peer device
    CHAR_T local_key[LOCAL_KEY_LEN + 1]; ///<local key of the peer device
    VOID* preserve;                      ///<reserved
    
    TMM_STREAM_DESC_T send;              ///<the ability of send stream to peer
    TMM_STREAM_DESC_T recv;              ///<the ability of receive stream from peer

    struct tuya_list_head node;          ///<list entry
}TMM_ASSOC_DEV_T;

/** @brief callback when the paired device list change
 * @param[in] head  the head node of the paired device list
 */
typedef VOID (*TMM_PAIRED_DEV_LIST_CHANGE_CB)(LIST_HEAD* head);

/**
 * @struct TMM_START_PARAM_T
 * @brief start parameters of link module
 */
typedef struct {
    TMM_PAIRED_DEV_LIST_CHANGE_CB list_change_cb; ///< callback when list change
    BOOL_T upload_disable;
} TMM_START_PARAM_T;

/** @brief link module init function
 * @return error code
 * - OPRT_OK success
 * - Others fail
 */
OPERATE_RET tuya_tmm_link_init();

/**
 * @brief start the link module
 * @warning should be called after device online
 * @param[in] pparam init parameters
 * @return error code
 * - OPRT_OK success
 * - Others fail
 */
OPERATE_RET tuya_tmm_link_start(TMM_START_PARAM_T *pparam);




/***********************************能力上报部分*******************************************/
/*
    启动时通过add decoder和add encoder来设置流媒体能力，通过skill上报
    当发生编解码能力变化时，通过update接口来更新，并需要触发skill上报！
*/

/** @brief set decode ability
 * @warning should be called between init and start
 * @param[in] decoder  decoder information
 * @return error code
 * - OPRT_OK  success
 * - Others   fail
 */
OPERATE_RET tuya_tmm_link_add_decoder(TUYA_DECODER_T decoder);

/** @brief set encode ability
 * @warning should be called between init and start
 * @param[in] media_info encoder information
 * @return error code
 * - OPRT_OK  success
 * - Others   fail
 */
OPERATE_RET tuya_tmm_link_add_encoder(IPC_MEDIA_INFO_T media_info);

#if 0
/* 变化时删除解码能力 */
OPERATE_RET tuya_tmm_link_update_decoder(TUYA_DECODER_T decoder);

/* 变化时更新编码能力 */
OPERATE_RET tuya_tmm_link_update_encoder(IPC_MEDIA_INFO_T media_info);
#endif




/***********************************连接建立部分*******************************************/

/** @brief connect to a peer device
 * @param[in]  dev_id  device id of the peer device
 * @return error code
 * - OPRT_OK  success
 * - Others   fail
 */
OPERATE_RET tuya_tmm_link_connect_device(CHAR_T *dev_id);

/** @brief control media transfer to/from peer device
 * @warning should be called after connect
 * @param[in]  dev_id  device id of the peer device
 * @param[in]  flag    media control flag, composed of TMM_MEDIA_CTL_FLAG_E bit
 * @param[in]  timeout_ms  the function will return before this timeout, -1 means no timeout
 * @return error code
 * - OPRT_OK  success
 * - Others   fail
 */
OPERATE_RET tuya_tmm_link_media_ctl(CHAR_T *dev_id, UINT_T flag, INT_T timeout_ms);



/** @brief disconect with the peer device
 * @param[in] dev_id  device id of the peer device
 * @return error code
 * - OPRT_OK success
 * - Others  fail
 */
OPERATE_RET tuya_tmm_link_disconnect(CHAR_T *dev_id);

/** @brief get the paired device list
 * @warning this will get the local list, no http request to cloud
 * @return the list head of paired device list, use tuya_list_for_each to visit, the type is TMM_ASSOC_DEV_T
 */
LIST_HEAD* tuya_tmm_link_get_paird_dev_list();

/** @brief sync paird device list from cloud
 * @return error code
 * - OPRT_OK success
 * - Others  fail
 */
OPERATE_RET tuya_tmm_link_sync_paird_dev_list();


#ifdef __cplusplus
}
#endif

#endif /*_TUYA_TMM_LINK_H_*/


