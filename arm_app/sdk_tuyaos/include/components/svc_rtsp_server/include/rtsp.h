/**
* @file      rtsp.h
* @brief     rtsp types and api
* @version   1.0
* @date      2021-12-6
*
* copyright  Copyright (c) tuya.inc 2021
*/
#ifndef __RTSP_H__
#define __RTSP_H__


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "tuya_cloud_types.h"
#include "tuya_error_code.h"
#include "rtp.h"

#define RTSP_URL_LEN 256

/**
* @enum RTSP_METHOD_E
* @brief rtsp method
*/
typedef enum {
    //required
    OPTIONS = 0,
    PLAY,
    SETUP,
    TEARDOWN,
    //recommended
    DESCRIBE,
    PAUSE,
    //optional
    ANNOUNCE,
    GET_PARAMETER,
    RECORD,
    REDIRECT,
    SET_PARAMETER,

    RTSP_METHOD_NUM,
} RTSP_METHOD_E;

/**
* @enum RTSP_HEADER_E
* @brief rtsp header type
*/
typedef enum {
    ACCEPT = 0,
    ACCEPT_ENCODING,
    ACCEPT_LANGUAGE,
    AUTHORIZATION,
    FROM,
    IF_MODIFIED_SINCE,
    RANGE,
    REFERER,
    USER_AGENT,
    BANDWIDTH,
    BLOCKSIZE,
    CONFERENCE,
    CSEQ,

    //general header
    CACHE_CONTROL,
    CONNECTION,
    DATE,
    VIA,
    //response header
    LOCATION,
    PROXY_AUTHENTICATE,
    PUBLIC,
    RETRY_AFTER,
    SERVER,
    VARY,
    WWW_AUTHENTICATE,
    //entity header
    CONTENT_BASE,
    ALLOW,
    CONTENT_ENCODING,
    CONTENT_LANGUAGE,
    CONTENT_LENGTH,
    CONTENT_LOCATION,
    CONTENT_TYPE,
    EXPIRES,
    LAST_MODIFIED,
    RTP_INFO,
    SESSION,
    TRANSPORT,

    RTSP_HEADER_NUM,
} RTSP_HEADER_E;

/**
* @enum RTSP_STATUS_CODE_E
* @brief rtsp status code
*/
typedef enum {
    CONTINUE                      = 100,
    OK                            = 200,
    CREATED                       = 201,
    LOW_ON_STORAGE_SPACE          = 250,
    MULTIPLE_CHOICES              = 300,
    MOVED_PERMANENTLY             = 301,
    MOVED_TEMPORARILY             = 302,
    SEE_OTHER                     = 303,
    NOT_MODIFIED                  = 304,
    USE_PROXY                     = 305,
    BAD_REQUEST                   = 400,
    UNAUTHORIZED                  = 401,
    PAYMENT_REQUIRED              = 402,
    FORBIDDEN                     = 403,
    NOT_FOUND                     = 404,
    METHOD_NOT_ALLOWED            = 405,
    NOT_ACCEPTABLE                = 406,
    PROXY_AUTHENTICATION_REQUIRED = 407,
    REQUEST_TIMEOUT               = 408,
    GONE                          = 410,
    LENGTH_REQUIRED               = 411,
    PRECONDITION_FAILED           = 412,
    REQUEST_ENTITY_TOO_LARGE      = 413,
    REQUEST_URI_TOO_LARGE         = 414,
    UNSUPPORTED_MEDIA_TYPE        = 415,
    PARAMETER_NOT_UNDERSTOOD      = 451,
    CONFERENCE_NOT_FOUND          = 452,
    NOT_ENOUGH_BANDWIDTH          = 453,
    SESSION_NOT_FOUND             = 454,
    METHOD_NOT_VALID_IN_THIS_STATE = 455,
    HEADER_FIELD_NOT_VALID_FOR_RESOURCE = 456,
    INVALID_RANGE                 = 457,
    PARAMETER_IS_READ_ONLY        = 458,
    AGGREGATE_OPERATION_NOT_ALLOWED = 459,
    ONLY_AGGREGATE_OPERATION_ALLOWED = 460,
    UNSUPPORTED_TRANSPORT         = 461,
    DESTINATION_UNREACHABLE       = 462,
    INTERNAL_SERVER_ERROR         = 500,
    NOT_IMPLEMENTED               = 501,
    BAD_GATEWAY                   = 502,
    SERVICE_UNAVAILABLE           = 503,
    GATEWAY_TIMEOUT               = 504,
    RTSP_VERSION_NOT_SUPPORTED    = 505,
    OPTION_NOT_SUPPORTED          = 551, 
 } RTSP_STATUS_CODE_E;

 /**
 * @enum RTSP_TRANSPORT_MODE_E
 * @brief rtsp transport mode
 */
typedef enum {
    RTSP_TRANSPORT_MODE_UDP = 1,
    RTSP_TRANSPORT_MODE_TCP,
} RTSP_TRANSPORT_MODE_E;

/**
* @enum RTSP_CAST_MODE_E
* @brief rtsp cast mode
*/
typedef enum {
    RTSP_CAST_MODE_UNICAST = 1,
    RTSP_CAST_MODE_MULTICAST,
} RTSP_CAST_MODE_E;

/**
* @enum RTSP_MSG_TYPE_E
* @brief rtsp message type
*/
typedef enum {
    RTSP_MSG_REQUEST = 1,
    RTSP_MSG_RESPONSE,
} RTSP_MSG_TYPE_E;

/**
* @enum RTSP_MEDIA_TYPE_E
* @brief rtsp media type
*/
typedef enum {
    RTSP_MEDIA_TYPE_INVALID = -1,
    RTSP_MEDIA_TYPE_VIDEO = 0,
    RTSP_MEDIA_TYPE_AUDIO,
    RTSP_MEDIA_TYPE_NUM,
} RTSP_MEDIA_TYPE_E;

/**
* @struct RTSP_MSG_T
* @brief  rtsp message
*/
typedef struct {
    RTSP_MSG_TYPE_E         type;
    union {
        RTSP_METHOD_E       method;
        RTSP_STATUS_CODE_E  status;
    };
    union {
        CHAR_T              uri[1024];
        CHAR_T              status_msg[32];
    };
    CHAR_T                  version[4];
    CHAR_T*                 headers[RTSP_HEADER_NUM];
    CHAR_T*                 body;
    INT_T                   body_len;
    INT_T                   msg_len;
} RTSP_MSG_T;

/**
* @struct RTSP_TRANSPORT_T
* @brief  rtsp transport
*/
typedef struct {
    RTSP_TRANSPORT_MODE_E trans_mode;
    RTSP_CAST_MODE_E cast_mode;
    INT_T client_port[2];
    INT_T server_port[2];
    INT_T interleaved[2];
} RTSP_TRANSPORT_T;

/**
* @struct RTSP_MEDIA_INFO_T
* @brief  rtsp media info
*/
typedef struct {
    RTP_CODEC_E      codec;
    INT_T            sample_rate;
    INT_T            channels;
    CHAR_T           track_uri[RTSP_URL_LEN];
    INT_T            track_avail;
} RTSP_MEDIA_INFO_T;


/**
 * @brief serialize rtsp request to binary data
 * @param[in] preq rtsp request message
 * @param[in] buf binary data will be put in this buffer
 * @param[in] size binary data buffer size
 * @return the size of data put in the buffer
 */
int rtsp_pack_request(RTSP_MSG_T* preq, CHAR_T* buf, INT_T size);

/**
 * @brief deserialzie rtsp request from binary data
 * @param[in] preq deserialized rtsp request message
 * @param[in] buf binary data
 * @param[in] size binary data buffer size
 * @return error code
 * - 0 success
 * - -1 fail
 */
int rtsp_unpack_request(RTSP_MSG_T* preq, CHAR_T*buf, INT_T size);

/**
 * @brief serialize rtsp response to binary data
 * @param[in] presp rtsp response message
 * @param[in] buf binary data will be put in this buffer
 * @param[in] size binary data buffer size
 * @return the size of data put in the buffer
 */
int rtsp_pack_response(RTSP_MSG_T* presp, CHAR_T* buf, INT_T size);

/**
 * @brief deserialize rtsp response from binary data
 * @param[in] presp deserialized rtsp response message
 * @param[in] buf binary data
 * @param[in] size binary data buffer size
 * @return error code
 * - 0 success
 * - -1 fail
 */
int rtsp_unpack_response(RTSP_MSG_T* presp, CHAR_T*buf, INT_T size);

/**
 * @brief serialize rtsp transport header to binary data
 * @param[in] ptrans rtsp transport info
 * @param[in] buf binary data will be put in this buffer
 * @param[in] size binary data buffer size
 * @return the size of data put in the buffer
 */
int rtsp_pack_transport(RTSP_TRANSPORT_T* ptrans, CHAR_T* buf, INT_T size);

/**
 * @brief deserialzie rtsp transport info from binary data
 * @param[in] ptrans deserialized rtsp transport info
 * @param[in] buf binary data
 * @return error code
 * - 0 success
 * - -1 fail
 */
int rtsp_unpack_transport(RTSP_TRANSPORT_T* ptrans, CHAR_T* buf);

/**
 * @brief print rtsp msg in log, for debug only
 * @param[in] pmsg rtsp message
 * @return error code
 */
int rtsp_print_msg(RTSP_MSG_T* pmsg);
#ifdef __cplusplus
} // extern "C"
#endif

#endif // __RTSP_H__

