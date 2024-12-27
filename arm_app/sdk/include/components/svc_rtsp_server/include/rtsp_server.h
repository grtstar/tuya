#ifndef __RTSP_SERVER_H__
#define __RTSP_SERVER_H__


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "rtsp.h"
#include "rtp.h"

typedef struct {
    RTSP_MEDIA_TYPE_E         type;          //video, audio
    CHAR_T*                   buf;           //data buffer
    INT_T                     len;           //data buffer len
    UINT64_T                  pts;           //timestamp in ms
    UINT8_T                   is_eof;        //whether this is the last fragment of the entire frame
    UINT8_T                   fragment_id;   //the first fragment should be 0, second fragment be 1, and so on
} RTSP_FRAME_FRAGMENT_T;

/**
 * @brief information of a stream source
 */
typedef struct {
    /**
     * @brief get sample rate of corresponding media type
     * @param[in] type media type, video or audio
     * @return sample rate, for example h264 video will be 90000, G711U audio can be 8000, 16000, ...
     */
    INT_T (*get_sample_rate)(RTSP_MEDIA_TYPE_E type);
    /**
     * @brief get one media frame
     * @param[in] user_id returned by start
     * @param[in] type media type, video or audio
     * @param[out] buf data buffer, the buffer should be maintained before the next call of get_frame
     * @param[out] plen data length
     * @param[out] pts timestamp in ms of the frame
     * @return 0 if success, others if fail
     * @warning in h264/h265, the first call of user_id should return a KEY frame
     */
    INT_T (*get_frame)(INT_T user_id, RTSP_MEDIA_TYPE_E type, CHAR_T** buf, INT_T *plen, UINT64_T *pts);
    /**
     * @brief get a fragment of one frame, this can be used in small memory devices which can't hold an entire frame
     * @param[in] user_id returned by start
     * @param[out] pframe fragment information
     * @return 0 if success, others if fail
     * @warning h264/h265 is currently not supported
     */ 
    INT_T (*get_frame_fragment)(INT_T user_id, RTSP_FRAME_FRAGMENT_T* pframe);
    /**
     * @brief get codec type of the media type
     * @param[in] type media type, video or audio
     * @return codec type
     */
    RTP_CODEC_E (*get_codec)(RTSP_MEDIA_TYPE_E type);
    /**
     * @brief start a session
     * @return user_id, each different session should have a different user_id
     */
    INT_T (*start)(VOID);
    /**
     * @brief stop a session
     * @param[in] user_id returned by start
     */
    INT_T (*stop)(INT_T user_id);
    /**
     * @brief get the name of stream, for example name is "abc", then the url will be rtsp://ip:port/abc
     * @param[out] buf buffer to store the name
     * @param[out] buf_len buffer length, should be modified to the actual name length
     * @return 0
     */
    INT_T (*get_name)(CHAR_T* buf, INT_T* buf_len);
} RTSP_STREAM_SRC_T;

typedef INT_T STREAM_SRC_ID;

/**
 * @brief register a stream source, should be called after server start
 * @param[in] psrc stream source information
 * @return stream source id
 */
STREAM_SRC_ID tuya_ipc_rtsp_server_register_stream_src(IN RTSP_STREAM_SRC_T* psrc);

/**
 * @brief start rtsp server
 * @param[in] username client should use this username to visit the stream, can be NULL
 * @param[in] password client should use this password to visit the stream, can be NULL
 * @return error code
 */
OPERATE_RET tuya_ipc_rtsp_server_start(IN CHAR_T* username, IN CHAR_T* password);

/**
 * @brief stop rtsp server
 * @return error code
 */
OPERATE_RET tuya_ipc_rtsp_server_stop(VOID);

/**
 * @brief set password of server, should be called after server start
 * @param[in] password the new password
 * @return error code
 */
OPERATE_RET tuya_ipc_rtsp_server_set_password(IN CHAR_T* password);

/**
 * @brief get the full url, like "rtsp://ip:port/stream0"
 * @param[in] id stream source id
 * @param[out] url url buffer
 * @param[out] plen buffer length, should be modified to the actual url length
 * @return error code
 */
OPERATE_RET tuya_ipc_rtsp_server_get_url(IN STREAM_SRC_ID id, OUT CHAR_T* url, OUT INT_T* plen);
/**
 * @brief set the get_name callback in the stream source
 * @param[in] id stream source id
 * @param[in] get_name the new callback
 * @return error code
 */
OPERATE_RET tuya_ipc_rtsp_server_set_url(IN STREAM_SRC_ID id, INT_T (*get_name)(CHAR_T* buf, INT_T* buf_len));

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __RTSP_SERVER_H__
