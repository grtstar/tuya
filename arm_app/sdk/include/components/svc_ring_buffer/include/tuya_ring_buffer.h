
#ifndef _TUYA_RING_BUFFER_
#define _TUYA_RING_BUFFER_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file tuya_ring_buffer.h
 * @brief ring buffer module is used to buffer audio/video data,
 */

#include "tuya_cloud_types.h"
#include "tuya_ipc_media.h"

#if ENABLE_MONITOR_WORK
#include "ty_monitor.h"
#define RING_BUF_CHECK_PERIOD 2
#define RING_BUF_CHECK_ERR_LIMIT 20
#endif

/**
 * @enum RBUF_OPEN_TYPE_E
 * @brief the ring buffer open mode
 */
typedef enum {
    E_RBUF_READ,      ///< open in read mode
    E_RBUF_WRITE,     ///< open in write mode
} RBUF_OPEN_TYPE_E;

/** @brief request key frame callback
 * @param[in] device        device number
 * @param[in] channel       channel number
 * @param[in] stream        stream number
 */
typedef VOID (*FUNC_REQUEST_KEY_FRAME_CB)(INT_T device, INT_T channel, IPC_STREAM_E stream);

/** @brief malloc hook used in ring buffer
 * @param[in] size          size to malloc
 * @return  the malloced buffer address or NULL
 */
typedef VOID* (*RING_BUFFER_MALLOC_FUNC_CB)(SIZE_T size);

/** @brief free hook used in ring buffer
 * @param[in] ptr        the address to free
 */
typedef VOID (*RING_BUFFER_FREE_FUNC_CB)(VOID* ptr);

/** @struct RING_BUFFER_NODE_T
 * @brief ring buffer node info
 */
typedef struct 
{
    UINT_T index;                   ///<index of the node in array
    MEDIA_FRAME_TYPE_E type;        ///<media frame type
    UCHAR_T *raw_data;              ///<address of the media data
    UINT_T size;                    ///<data size
    UINT64_T pts;                   ///<time stamp in us
    UINT64_T timestamp;             ///<time stamp is ms
    UINT_T seq_no;                  ///<sequence number
    UCHAR_T *extra_data;            ///<address of the extra data
    UINT_T extra_size;              ///<size of the extra data
    UINT_T seq_sync;                ///<global sequence number used to sync audio/video
}RING_BUFFER_NODE_T;


/** @struct RING_BUFFER_INIT_PARAM_T
 * @brief ring buffer init parameters, should be set as actual
 */
typedef struct
{
    UINT_T bitrate;                                  ///<bitrate of the media in kb
    UINT_T fps;                                      ///<frame number per second
    UINT_T max_buffer_seconds;                       ///<seconds to reserve data, should be more than 1 GOP and less then 10 second. set to 0 as default(10s)
    FUNC_REQUEST_KEY_FRAME_CB request_key_frame_cb;  ///<callback to request a key frame from video encoder. set to NULL if not needed
} RING_BUFFER_INIT_PARAM_T;

/** @brief user handle which returned by open
 */
typedef void* RING_BUFFER_USER_HANDLE_T;

/** @brief initialize one ring buffer for one channel(one device)
 * @param[in]      device           device number
 * @param[in]      channel          channel number in device
 * @param[in]      stream           stream number in channel
 * @param[in]      pparam           initialize parameter
 * @return error code
 * - OPRT_OK        init success
 * - Others         init failed
 */
OPERATE_RET tuya_ipc_ring_buffer_init(INT_T device, INT_T channel, IPC_STREAM_E stream, RING_BUFFER_INIT_PARAM_T* pparam);

/** @brief uninitialize one ring buffer for one channel(one device)
 * @param[in]     device   device number
 * @param[in]     channel  channel number in device
 * @param[in]     stream   stream number in channel
 * @return error code
 * - OPRT_OK      uninit success
 * - Others       uninit failed
*/
OPERATE_RET tuya_ipc_ring_buffer_uninit(INT_T device, INT_T channel, IPC_STREAM_E stream);

/** @brief set the max vidoe afterwards of the first frame got by tuya_ipc_ring_buffer_get_frame, default 3s 
 * @param[in]  afterwards_sec       seconds to backward
 * @return error code
 * - OPRT_OK      set success
 * - Others       set failed
*/
OPERATE_RET tuya_ipc_ring_buffer_set_max_video_afterwards(UINT_T afterwards_sec);


/** @brief get number of ring buffer inited in the specified device, channel
 * @param[in]  device      device number
 * @param[in]  channel     channel number
 * @return number of ring buffer inited
 */
int tuya_ipc_ring_buffer_get_video_num_skill(INT_T device, INT_T channel);

/** @brief open a new session for read/write
 * @param[in]     device   device number
 * @param[in]     channel  channel number in device
 * @param[in]     stream   stream number in channel
 * @param[in]     open_type  open type
 * @return user handle
*/
RING_BUFFER_USER_HANDLE_T tuya_ipc_ring_buffer_open(INT_T device, INT_T channel, IPC_STREAM_E stream, RBUF_OPEN_TYPE_E open_type);

/** @brief close session
 * @warning open/close should be called in pair like file
 * @param[in]   handle      user handle return by open
 * @return error code
 * - OPRT_OK      success
 * - Others       failed
*/
OPERATE_RET tuya_ipc_ring_buffer_close(RING_BUFFER_USER_HANDLE_T handle);
/** @brief append new frame into a ring buffer
 * @param[in]   handle       user handle return by open
 * @param[in]   addr         the address of the data to append
 * @param[in]   size         size of the data
 * @param[in]   type         media type
 * @param[in]   pts          time stamp in us
 * @return error code
 * - OPRT_OK      success
 * - Others       failed
*/
OPERATE_RET tuya_ipc_ring_buffer_append_data(RING_BUFFER_USER_HANDLE_T handle, UCHAR_T *addr, UINT_T size, MEDIA_FRAME_TYPE_E type, UINT64_T pts);

/** @brief append new frame into ring buffer with custom timestamp
 * @param[in]   handle       user handle return by open
 * @param[in]   addr         the address of the data to append
 * @param[in]   size         size of the data
 * @param[in]   type         media type
 * @param[in]   pts          time stamp in us
 * @param[in]   timestamp    time stamp in ms
 * @return error code
 * - OPRT_OK      success
 * - Others       failed
 */ 
OPERATE_RET tuya_ipc_ring_buffer_append_data_with_timestamp(RING_BUFFER_USER_HANDLE_T handle, UCHAR_T *addr, UINT_T size, MEDIA_FRAME_TYPE_E type, UINT64_T pts, UINT64_T timestamp);

/** @brief append extra data to current head node of ring buffer
 * this API is for the situation:
 * different format of the same channel frame needed to be buffered, both PCM&AAC for the same audio frame e.g.
 * data will be stored in extra_data/extra_size field.
 * @warning MUST be called after tuya_ipc_ring_buffer_append_data
 * @param[in]     handle  user handle return by open
 * @param[in]     data_addr   address of the extra data
 * @param[in]     data_size   size of the extra data
 * @return error code
 * - OPRT_OK      success
 * - Others       failed
*/
OPERATE_RET tuya_ipc_ring_buffer_append_extra_data(RING_BUFFER_USER_HANDLE_T handle, UCHAR_T *data_addr, UINT_T data_size);

/** @brief get next frame from ring buffer, will jump to the latest frame if delayed too much time
 * @param[in]     handle  user handle return by open
 * @param[in]     is_retry whether retry to get the last frame
 * @return the ring buffer node or NULL if there is no newer frame in ring buffer
 */
RING_BUFFER_NODE_T *tuya_ipc_ring_buffer_get_frame(RING_BUFFER_USER_HANDLE_T handle, BOOL_T is_retry);

/** @brief get the next video/audio frame in ring buffer(sync by timestamp) will jump to the latest frame if delayed too much time
 * @param[in] v_handle      video user handle
 * @param[in] a_handle      audio user handle
 * @param[in]     is_retry whether retry to get the last frame
 * @return the ring buffer node or NULL if there is no newer frame in ring buffer, may return video or audio frame according to their timestamp
 */
RING_BUFFER_NODE_T* tuya_ipc_ring_buffer_get_av_frame(RING_BUFFER_USER_HANDLE_T v_handle, RING_BUFFER_USER_HANDLE_T a_handle, BOOL_T is_retry);

/** @brief start with the newest frame, find previous frame by count frame_num.
for video channel(device), it will keep find previous frame until it's I frame.
 * @param[in]    handle user handle returned by open
 * @param[in]    frame_num    count frame num backwards
 * @return the ring buffer node or NULL if there is no newer frame in ring buffer
*/
RING_BUFFER_NODE_T *tuya_ipc_ring_buffer_find_pre_by_frame(RING_BUFFER_USER_HANDLE_T handle, UINT_T frame_num);

/** @brief start with the newest frame, find previous frame by count frame_num, and update&anchor user(of userIndex) to this frame node.
for video channel(device), it will keep find previous frame until it's I frame.
 * @param[in]    handle user handle returned by open
 * @param[in]    frame_num    count frame num backwards
 * @return the ring buffer node or NULL if there is no newer frame in ring buffer
*/
RING_BUFFER_NODE_T *tuya_ipc_ring_buffer_get_pre_frame(RING_BUFFER_USER_HANDLE_T handle, UINT_T frame_num);

/** @brief anchor user of handle to specified node.
normally this API is called after tuya_ipc_ring_buffer_find_pre_by_frame
 * @param[in]  handle  user handle returned by open
 * @param[in]  node    ring buffer node
*/
VOID tuya_ipc_ring_buffer_anchor_user_to_node(RING_BUFFER_USER_HANDLE_T handle, RING_BUFFER_NODE_T *node);

/** @brief start from current frame node of user, get the next target frame with right type, and it's sequence number is greater than or equal to seq_no
 * @param[in] handle    user handle returned by open
 * @param[in] seq_no    sequence number
 * @param[in] type      media frame type
 * @return the ring buffer node or NULL if there is no newer frame in ring buffer
*/
RING_BUFFER_NODE_T *tuya_ipc_ring_buffer_get_next_target_frame(RING_BUFFER_USER_HANDLE_T handle, UINT_T seq_no, MEDIA_FRAME_TYPE_E type);

/** @brief clean the state of user, which means for next time tuya_ipc_ring_buffer_get_frame will start from the newest frame node
 * @param[in]  handle   user handle return by open
*/
VOID tuya_ipc_ring_buffer_clean_user_state(RING_BUFFER_USER_HANDLE_T handle);

/** @brief clean the state of user and data buffer, which means tuya_ipc_ring_buffer_get_pre_frame cann't get data earlier than the time this func called,
   for next time tuya_ipc_ring_buffer_get_frame will start from the newest frame node
 * @param[in] handle user handle return by open
*/
VOID tuya_ipc_ring_buffer_clean_user_state_and_buffer(RING_BUFFER_USER_HANDLE_T handle);

/** @brief check if ring buffer user is delayed too much
 * @param[in]  handle  user handle returned by open
 * @return delay or not
*/
BOOL_T tuya_ipc_ring_buffer_if_delay(RING_BUFFER_USER_HANDLE_T handle);

/** @brief get ring buffer max buf seconds
 * @param[in]  device   device number
 * @param[in]  channel  channel number of device
 * @param[in]  stream   stream number of channel
 * @return max buf seconds
*/
INT_T tuya_ipc_ring_buffer_get_max_buf_secs(int device, int channel, IPC_STREAM_E stream);

/** @brief set max frame size of device
 * @param[in]  device   device number
 * @param[in]  channel  channel number of device
 * @param[in]  stream   stream number of channel
 * @param[in]  max_frame_size the max frame size that can be put into ring buffer
 * @return error code
 * - OPRT_OK  success
 * - Others   fail
*/
OPERATE_RET tuya_ipc_ring_buffer_set_max_frame_size(INT_T device, INT_T channel, IPC_STREAM_E stream, UINT_T max_frame_size);

/** @brief get max frame size of the specified stream
 * @param[in]  device   device number
 * @param[in]  channel  channel number of device
 * @param[in]  stream   stream number of channel
 * @return max frame size, the default value is 1.5 * bitrate(kb) * 1024 / 8
*/
INT_T tuya_ipc_ring_buffer_get_max_frame_size(INT_T device, INT_T channel, IPC_STREAM_E stream);

/** @brief synchronize specified video/audio streams according to their timestamp
 * @param[in] handle_video  video user handle
 * @param[in] handle_audio  audio user handle
 * @return error code
 * - OPRT_OK  success
 * - Others   fail
*/
OPERATE_RET tuya_ipc_ring_buffer_sync_stream(RING_BUFFER_USER_HANDLE_T handle_video, RING_BUFFER_USER_HANDLE_T handle_audio);

/** @brief set user index to frame_num backwards, if it's video stream, it will be set to I frame
 * @param[in] handle  user handle return by open
 * @param[in] frame_num count frame_num frames backwards
 * @param[in] check_overlap if it's true, then it will stop at the index which the user has get
 * @return error code
 * - OPRT_OK  success
 * - Others   fail
*/
OPERATE_RET tuya_ipc_ring_buffer_anchor_user(RING_BUFFER_USER_HANDLE_T handle, UINT_T frame_num, BOOL_T check_overlap);

/** @brief set the custom malloc/free hook func 
 * @param[in] malloc_func  malloc func
 * @param[in] free_func free func
 * @return error code
 * - OPRT_OK  success
 * - Others   fail
 */
OPERATE_RET tuya_ipc_ring_buffer_hook_mem(RING_BUFFER_MALLOC_FUNC_CB malloc_func, RING_BUFFER_FREE_FUNC_CB free_func);

/** @brief get the next gop size after encrypt, include video and audio frames
 * @param[in] v_handle  video handle returned by open
 * @param[in] a_handle  audio handle returned by open
 * @param[out] p_frame_num frame number in the next gop
 * @return the next gop size after encrypt
 */
UINT_T tuya_ipc_ring_buffer_get_next_gop_encrypted_size(RING_BUFFER_USER_HANDLE_T v_handle, RING_BUFFER_USER_HANDLE_T a_handle, UINT_T *p_frame_num);

/** @brief register ring buffer to media adapter
 * @return error code
 * - OPRT_OK  success
 * - Others   fail
 */
OPERATE_RET tuya_ipc_ring_buffer_adapter_register_media_source();

/**
 * @brief correct timestamp of the previous frames according to the latest frame
 * 
 * @param device device number, 0 for ipc
 * @param channel  channel number, 0 for ipc
 * @param stream stream type
 * @param frame_interval_ms frame interval in ms
 * @param correct_all whether correct all the timestamp, if set to false, only frames of wrong timestamp will be corrected
 * @return error code
 * - OPRT_OK success
 * - Others fail
 */
OPERATE_RET tuya_ipc_ring_buffer_correct_timestamp(INT_T device, INT_T channel, IPC_STREAM_E stream, UINT_T frame_interval_ms, BOOL_T correct_all);

#ifdef __cplusplus
}
#endif

#endif  /*_TUYA_RING_BUFFER_*/


