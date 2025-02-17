#ifndef ENCODER_H
#define ENCODER_H

#include "rk_mpi.h"
#include "mpp_env.h"
#include "mpp_mem.h"
#include "mpp_time.h"
#include "mpp_debug.h"
#include "mpp_common.h"
#include "mpp_soc.h"

#include "utils.h"
#include "mpi_enc_utils.h"
#include "camera_source.h"
#include "mpp_enc_roi_utils.h"
#include "mpp_rc_api.h"

#include <functional>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct rkencoder {
    MppCtx             ctx;             // MPP 上下文
    MppApi           *mpi;             // MPP API 接口
    RK_S32             chn;             // 频道编号

    RK_U32             frm_eos;         // 输入帧结束标志
    RK_U32             pkt_eos;         // 输出包结束标志
    RK_U32             frm_pkt_cnt;     // 帧与包计数器

    RK_S32             frame_num;       // 待编码的帧数
    RK_S32             frame_count;     // 已编码的帧数
    RK_U64             stream_size;     // 输出码流总大小（字节）
    volatile RK_U32    loop_end;        // 编码循环结束标志

    FILE              *fp_input;        // 输入文件指针
    FILE              *fp_output;       // 输出文件指针
    FILE              *fp_verify;       // 校验数据文件指针

    MppEncCfg          cfg;             // 编码器配置句柄
    MppEncPrepCfg      prep_cfg;        // 预处理配置
    MppEncRcCfg        rc_cfg;          // 码率控制配置
    MppEncCodecCfg     codec_cfg;       // 编码器参数配置
    MppEncSliceSplit   split_cfg;       // 切片分割配置
    MppEncOSDPltCfg    osd_plt_cfg;     // OSD 调色板配置
    MppEncOSDPlt       osd_plt;         // OSD 调色板
    MppEncOSDData      osd_data;        // OSD 数据信息
    RoiRegionCfg       roi_region;      // ROI 区域配置
    MppEncROICfg       roi_cfg;         // ROI 编码配置

    MppBufferGroup     buf_grp;         // 缓冲组
    MppBuffer          frm_buf;         // 输入帧缓冲
    MppBuffer          pkt_buf;         // 输出包缓冲
    MppBuffer          md_info;         // 运动信息缓冲
    MppEncSeiMode      sei_mode;        // SEI 数据模式
    MppEncHeaderMode   header_mode;     // 头信息获取模式

    RK_U32             width;           // 图像宽度
    RK_U32             height;          // 图像高度
    RK_U32             hor_stride;      // 水平步幅
    RK_U32             ver_stride;      // 垂直步幅
    MppFrameFormat     fmt;             // 帧的像素格式
    MppCodingType      type;            // 编码类型
    RK_S32             loop_times;      // 循环编码计数
    CamSource         *cam_ctx;         // 摄像头源上下文
    MppEncRoiCtx       roi_ctx;         // ROI 编码上下文

    size_t             header_size;     // 头信息大小
    size_t             frame_size;      // 单帧数据大小
    size_t             mdinfo_size;     // 运动信息大小
    size_t             packet_size;     // 包缓冲大小

    RK_U32             osd_enable;      // OSD 功能使能标志
    RK_U32             osd_mode;        // OSD 显示模式
    RK_U32             split_mode;      // 分片编码模式
    RK_U32             split_arg;       // 分片编码参数
    RK_U32             split_out;       // 分片输出标志

    RK_U32             user_data_enable;// 用户数据使能标志
    RK_U32             roi_enable;      // ROI 功能使能标志

    RK_S32             fps_in_flex;     // 输入帧率灵活性参数
    RK_S32             fps_in_den;      // 输入帧率分母
    RK_S32             fps_in_num;      // 输入帧率分子
    RK_S32             fps_out_flex;    // 输出帧率灵活性参数
    RK_S32             fps_out_den;     // 输出帧率分母
    RK_S32             fps_out_num;     // 输出帧率分子
    RK_S32             bps;             // 目标码率
    RK_S32             bps_max;         // 最大码率
    RK_S32             bps_min;         // 最小码率
    RK_S32             rc_mode;         // 码率控制模式
    RK_S32             gop_mode;        // GOP 模式设定
    RK_S32             gop_len;         // GOP 长度
    RK_S32             vi_len;          // 长间隔参考帧长度
    RK_S32             scene_mode;      // 场景模式设定
    RK_S32             cu_qp_delta_depth;// CU QP 差异深度
    RK_S32             anti_flicker_str;// 防闪烁强度
    RK_S32             atr_str_i;       // I 帧自适应调节强度
    RK_S32             atr_str_p;       // P 帧自适应调节强度
    RK_S32             atl_str;         // 自动亮度调节强度
    RK_S32             sao_str_i;       // I 帧 SAO 强度
    RK_S32             sao_str_p;       // P 帧 SAO 强度
    RK_S64             first_frm;       // 第一个输入帧的时间戳
    RK_S64             first_pkt;       // 第一个输出包的时间戳
    MppPacket          packet;
} rkencoder;

// 编码器配置函数
void rkencoder_setup_1080p(rkencoder *encoder);
void rkencode_setup_720p(rkencoder *encoder);
void rkencode_setup_480p(rkencoder *encoder);

// 编码器流程相关函数
MPP_RET rkenc_cfg_setup(rkencoder *p);
MPP_RET rkencode_reset(rkencoder *p);
MPP_RET rkencode_setup(rkencoder *p);
MPP_RET rkencode_frame(rkencoder *p, void* frame_buf, std::function<void(void*, size_t, bool)> fn, bool is_init);
void rkencode_deinit(rkencoder *enc);
MPP_RET rkencode_init(rkencoder *p, bool high_q);

MPP_RET rkencode_put_frame(rkencoder *p, void* frame_buf);
MPP_RET rkencode_get_sps(rkencoder *p, void* data, size_t *len);
MPP_RET rkencode_get_data(rkencoder *p, void* data, size_t *len, int *eoi, bool *is_key_frame);
MPP_RET rkencode_packet_release(rkencoder *p);

#ifdef __cplusplus
}
#endif

#endif // ENCODER_H