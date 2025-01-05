#ifndef __RTP_H__
#define __RTP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define RTP_HDR_LEN            (12)       /**RTP HEADER LEN*/
#define MAX_PAYLOAD_SIZE       (1100)     /**MAX PAYLOAD SIZE*/
#define H264_PAY_LOAD          (96)       /**H264 PAYLOAD*/
#define H265_PAY_LOAD          (95)       /**H265 PAYLOAD*/
#define RTP_PCMU_PAYLOAD       (0)        /**PCMU PAYLOAD*/
#define RTP_PCMA_PAYLOAD       (8)        /**PCMU PAYLOAD*/
#define RTP_PCM_PAYLOAD        (99)       /**PCM PAYLOAD*/

#define RTP_MTU_LEN            MAX_PAYLOAD_SIZE
#define RTP_SPLIT_LEN          RTP_MTU_LEN
#define TUYA_RTP_HEAD          0x12345678  //自定义rtp识别包头

/**
* @enum RTCP_TYPE_E
*
* @brief rtcp type
*/
typedef enum {
    RTCP_TYPE_SR = 200,   /**SR TYPE*/
    RTCP_TYPE_RR = 201,   /**RR TYPE*/
    RTCP_TYPE_SDES = 202, /**RTP HEADER LEN*/
    RTCP_TYPE_BYE = 203,  /**RTP BYE TYPE*/
    RTCP_TYPE_APP = 204,  /**RTP APP TYPE*/
} RTCP_TYPE_E;

/**
* @enum RTCP_TYPE_E
*
* @brief rtp code
*/
typedef enum {
    RTP_CODEC_INVALID = 0,  /**invalid code*/
    RTP_CODEC_H264,         /**h264 code*/
    RTP_CODEC_H265,         /**h265 code*/
    RTP_CODEC_JPEG,         /**jpeg code*/
    RTP_CODEC_PCM,          /**pcm code*/
    RTP_CODEC_PS,           /**ps code*/
    RTP_CODEC_G711A,        /**g711a code*/
    RTP_CODEC_G711U,        /**g711u code*/
    RTP_CODEC_MAX,          /**max code*/
} RTP_CODEC_E;

/**
* @enum RTSP_FRAME_TYPE_E
*
* @brief frame type
*/
typedef enum {
    E_RTSP_VIDEO_PB_FRAME = 0,  /**b frame*/
    E_RTSP_VIDEO_I_FRAME,       /**i frame*/
    E_RTSP_VIDEO_SPS_FRAME,     /**sps frame*/
    E_RTSP_VIDEO_PPS_FRAME,     /**pps frame*/
    E_RTSP_VIDEO_VPS_FRAME,     /**vps frame*/
    E_RTSP_VIDEO_SEI_FRAME,     /**i frame*/
    E_RTSP_AUDIO_FRAME,         /**i frame*/
    E_RTSP_MEDIA_FRAME_TYPE_MAX
}RTSP_FRAME_TYPE_E;

/**
* @struct RTSP_DATA_PACKET_T
*
* @brief rtsp data  packet
*/
typedef struct
{
    RTSP_FRAME_TYPE_E  type;         /**frame type*/
    RTP_CODEC_E        codec;        /**rtp code*/
    int                frame_start;  /**frame start*/
    int                frame_end;    /**frame end*/
    char*              buf;          /**frame buffer*/
    uint32_t           size;         /**frame size*/
    uint64_t           timestamp;    /**frame timestamp*/
}RTSP_DATA_PACKET_T;

/**
* @enum RTCP_APP_SUBTYPE_E
*
* @brief rtcp app subscrible type
*/
typedef enum {
    RTCP_APP_SUBTYPE_NACK = 1,    /**nack*/
} RTCP_APP_SUBTYPE_E;

/**
* @enum RTCP_APP_SUBTYPE_E
*
* @brief rtcp app subscrible type
*/
typedef struct {
    uint32_t csrc_cnt:4;          /**csrc_cnt*/
    uint32_t extension:1;         /**extension*/
    uint32_t padding:1;           /**padding*/
    uint32_t version:2;           /**version*/

    uint32_t payload_type:7;     /**payload_type*/
    uint32_t marker:1;           /**marker*/

    uint16_t seq:16;            /**seq*/
    uint32_t timestamp:32;      /**timestamp*/
    uint32_t ssrc:32;           /**ssrc*/
    uint32_t csrc_list[15];     /**csrc_list*/
} RTP_HEADER_T; 

/**
* @struct RTP_FRAME_T
*
* @brief rtp frame
*/
typedef struct {
    RTP_HEADER_T        hdr;        /**hdr*/
    char*               payload;    /**payload*/
    int                 pld_len;    /**pld_len*/
} RTP_FRAME_T;

/**
* @struct RTCP_HEADER_T
*
* @brief rtc header
*/
typedef struct {
    uint32_t subtype : 5;  /**subtype*/
    uint32_t padding:1;    /**padding*/
    uint32_t version:2;    /**version*/

    uint32_t payload_type:8;  /**payload_type*/
    uint32_t length:16;       /**length*/

} RTCP_HEADER_T;

/**
* @struct RTP_PARAM_T
*
* @brief rtp param
*/
typedef struct {
    uint16_t*             pseq;    /**pseq*/
	RTP_CODEC_E           codec;   /**codec*/
    uint32_t              rtp_ts;  /**rtp_ts*/
    uint32_t              ssrc;    /**ssrc*/
    uint32_t              payload; /**payload*/
} RTP_PARAM_T;

/**
* @struct RTP_PACKAGER_FRAGMENT_T
*
* @brief rtp package
*/
typedef struct {
    const char*               buf;              /**fragment buf*/
    int                       len;              /**fragment len*/
    int                       offset;           /**fragment ofset*/
    int                       fragment_id;      /**fragment id*/
    char                      priv_buf[1024];   /**fragment priv buf*/
    uint8_t                   is_eof;           /**fragment is_eof*/
} RTP_PACKAGER_FRAGMENT_T;

/**
* @brief  pack rtp header
*
* @param[in] phdr: rtp heaher 
* @param[out] buf:  rtp data
* @param[in] len:  rtp heaher len
*
* @return  rtp heaher len
*/
int tuya_ipc_rtp_pack_header(RTP_HEADER_T* phdr, char* buf, int len);

/**
* @brief  parse rtp header
*
* @param[out] phdr: rtp heaher 
* @param[in] buf:  rtp data
* @param[in] len:  rtp data len
*
* @return  rtp heaher len
*/
int tuya_ipc_rtp_unpack_header(RTP_HEADER_T* phdr, char* buf, int len);

/**
* @brief  pack rctp header
*
* @param[in] phdr: rctp heaher 
* @param[out] buf:  rctp data
* @param[in] len:  rctp heaher len
*
* @return  rctp heaher len
*/
int tuya_ipc_rtcp_pack_header(RTCP_HEADER_T* phdr, char* buf, int len);

/**
* @brief  parse rtcp header
*
* @param[out] phdr: rtcp heaher 
* @param[in] buf:  rtcp data
* @param[in] len:  rtcp data len
*
* @return  rtcp heaher len
*/
int tuya_ipc_rtcp_unpack_header(RTCP_HEADER_T* phdr, char* buf, int len);

/**
* @brief rtp pkt need to process, it is callback 
*
* @param[in] buf: rtp buffer 
* @param[in] len: rtp len
* @param[in] arg:  user data
*
* @return  0:success others:fail
*/
typedef int (*tuya_ipc_rtp_pkt_handler_cb)(char* buf, int len, void* arg);

/**
* @brief pack a video/audio frame to multiple rtp packets
*
* @param[in] rtp_param: rtp header param
* @param[in] frm_buf: rtp frame buffer
* @param[in] frm_len: rtp frame len
* @param[in] handler: callback ; need to process rtp frame
* @param[in] arg:  user data
*
* @return  0:success others:fail
*/
int tuya_ipc_rtp_pack_nal(RTP_PARAM_T rtp_param, char* frm_buf, int frm_len,tuya_ipc_rtp_pkt_handler_cb handler, void* arg);

/**
* @brief pack a packet to multiple fragments
*
* @param[in] rtp_param: rtp header param
* @param[in] pfrag: fragment buffer ptr
* @param[in] handler: callback ; need to process rtp frame
* @param[in] arg:  user data
*
* @return  0:success others:fail
*/
int tuya_ipc_rtp_pack_fragment(RTP_PARAM_T rtp_param, RTP_PACKAGER_FRAGMENT_T *pfrag, tuya_ipc_rtp_pkt_handler_cb handler, void* arg);

/**
* @brief parse data frame
*
* @param[in] pfrm: rtp header param
* @param[in] rtp_buf: rtp buffer
* @param[in] buf_len:rtp buffer len
*
* @return  0:success others:fail
*/
int tuya_ipc_rtp_frame_parse(RTP_FRAME_T *pfrm, char* rtp_buf, int buf_len);

#ifdef __cplusplus
} 
#endif

#endif 
