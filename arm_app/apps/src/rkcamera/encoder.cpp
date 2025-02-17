#include "encoder.h"

#if 1

static RK_S32 get_mdinfo_size(rkencoder *p, MppCodingType type)
{
    RockchipSocType soc_type = mpp_get_soc_type();
    RK_S32 md_size;
    RK_U32 w = p->hor_stride, h = p->ver_stride;

    if (soc_type == ROCKCHIP_SOC_RK3588) {
        md_size = (MPP_ALIGN(w, 64) >> 6) * (MPP_ALIGN(h, 64) >> 6) * 32;
    } else {
        md_size = (MPP_VIDEO_CodingHEVC == type) ?
                  (MPP_ALIGN(w, 32) >> 5) * (MPP_ALIGN(h, 32) >> 5) * 16 :
                  (MPP_ALIGN(w, 64) >> 6) * (MPP_ALIGN(h, 16) >> 4) * 16;
    }

    return md_size;
}

void rkencoder_setup_1080p(rkencoder *encoder)
{
    encoder->width = 1920;
    encoder->height = 1080;
    encoder->hor_stride = MPP_ALIGN(encoder->width, 8);;
    encoder->ver_stride = MPP_ALIGN(encoder->height, 2);
    encoder->fmt = MPP_FMT_YUV420SP;
    encoder->type = MPP_VIDEO_CodingAVC;
    encoder->bps = 2 * 1024 * 1024;
    encoder->bps_min = 1 * 1024 * 1024;
    encoder->bps_max = 3 * 1024 * 1024;    
    encoder->rc_mode = MPP_ENC_RC_MODE_VBR;
    encoder->frame_num = 0;
    encoder->gop_mode = 0;
    encoder->gop_len = 30;
    encoder->vi_len = 0;
    encoder->fps_in_flex = 0;
    encoder->fps_in_den = 1;
    encoder->fps_in_num = 30;
    encoder->fps_out_flex = 0;
    encoder->fps_out_den = 1;
    encoder->fps_out_num = 30;
    encoder->scene_mode = 0;
    encoder->cu_qp_delta_depth = 0;
    encoder->anti_flicker_str = 0;
    encoder->atr_str_i = 0;
    encoder->atr_str_p = 0;
    encoder->atl_str = 0;
    encoder->sao_str_i = 0;
    encoder->sao_str_p = 0;
    encoder->mdinfo_size = get_mdinfo_size(encoder, encoder->type);
    encoder->frame_size = MPP_ALIGN(encoder->hor_stride, 64) * MPP_ALIGN(encoder->ver_stride, 64) * 3 / 2;
     if (MPP_FRAME_FMT_IS_FBC(encoder->fmt)) {
        if ((encoder->fmt & MPP_FRAME_FBC_MASK) == MPP_FRAME_FBC_AFBC_V1)
            encoder->header_size = MPP_ALIGN(MPP_ALIGN(encoder->width, 16) * MPP_ALIGN(encoder->height, 16) / 16, SZ_4K);
        else
            encoder->header_size = MPP_ALIGN(encoder->width, 16) * MPP_ALIGN(encoder->height, 16) / 16;
    } else {
        encoder->header_size = 0;
    }
}

void rkencode_setup_720p(rkencoder *encoder)
{
    encoder->width = 1280;
    encoder->height = 720;
    encoder->hor_stride = MPP_ALIGN(encoder->width, 8);
    encoder->ver_stride = MPP_ALIGN(encoder->height, 2);
    encoder->fmt = MPP_FMT_YUV420SP;
    encoder->type = MPP_VIDEO_CodingAVC;
    encoder->bps = 1 * 1024 * 1024;
    encoder->bps_min = 0.5 * 1024 * 1024;
    encoder->bps_max = 1.5 * 1024 * 1024;
    encoder->rc_mode = MPP_ENC_RC_MODE_VBR;
    encoder->frame_num = 0;
    encoder->gop_mode = 0;
    encoder->gop_len = 30;
    encoder->vi_len = 0;
    encoder->fps_in_flex = 0;
    encoder->fps_in_den = 1;
    encoder->fps_in_num = 30;
    encoder->fps_out_flex = 0;
    encoder->fps_out_den = 1;
    encoder->fps_out_num = 30;
    encoder->scene_mode = 0;
    encoder->cu_qp_delta_depth = 0;
    encoder->anti_flicker_str = 0;
    encoder->atr_str_i = 0;
    encoder->atr_str_p = 0;
    encoder->atl_str = 0;
    encoder->sao_str_i = 0;
    encoder->sao_str_p = 0;
    encoder->mdinfo_size = get_mdinfo_size(encoder, encoder->type);
    encoder->frame_size = MPP_ALIGN(encoder->hor_stride, 64) * MPP_ALIGN(encoder->ver_stride, 64) * 3 / 2;
     if (MPP_FRAME_FMT_IS_FBC(encoder->fmt)) {
        if ((encoder->fmt & MPP_FRAME_FBC_MASK) == MPP_FRAME_FBC_AFBC_V1)
            encoder->header_size = MPP_ALIGN(MPP_ALIGN(encoder->width, 16) * MPP_ALIGN(encoder->height, 16) / 16, SZ_4K);
        else
            encoder->header_size = MPP_ALIGN(encoder->width, 16) * MPP_ALIGN(encoder->height, 16) / 16;
    } else {
        encoder->header_size = 0;
    }
}

void rkencode_setup_480p(rkencoder *encoder)
{
    encoder->width = 1280;
    encoder->height = 720;
    encoder->hor_stride = MPP_ALIGN(encoder->width, 8);
    encoder->ver_stride = MPP_ALIGN(encoder->height, 2);
    encoder->fmt = MPP_FMT_YUV420SP;
    encoder->type = MPP_VIDEO_CodingAVC;
    encoder->bps = 0.5 * 1024 * 1024;
    encoder->bps_min = 0.25 * 1024 * 1024;
    encoder->bps_max = 0.75 * 1024 * 1024;
    encoder->rc_mode = MPP_ENC_RC_MODE_VBR;
    encoder->frame_num = 0;
    encoder->gop_mode = 0;
    encoder->gop_len = 30;
    encoder->vi_len = 0;
    encoder->fps_in_flex = 0;
    encoder->fps_in_den = 1;
    encoder->fps_in_num = 30;
    encoder->fps_out_flex = 0;
    encoder->fps_out_den = 1;
    encoder->fps_out_num = 30;
    encoder->scene_mode = 0;
    encoder->cu_qp_delta_depth = 0;
    encoder->anti_flicker_str = 0;
    encoder->atr_str_i = 0;
    encoder->atr_str_p = 0;
    encoder->atl_str = 0;
    encoder->sao_str_i = 0;
    encoder->sao_str_p = 0;
    encoder->mdinfo_size = get_mdinfo_size(encoder, encoder->type);
    encoder->frame_size = MPP_ALIGN(encoder->hor_stride, 64) * MPP_ALIGN(encoder->ver_stride, 64) * 3 / 2;
     if (MPP_FRAME_FMT_IS_FBC(encoder->fmt)) {
        if ((encoder->fmt & MPP_FRAME_FBC_MASK) == MPP_FRAME_FBC_AFBC_V1)
            encoder->header_size = MPP_ALIGN(MPP_ALIGN(encoder->width, 16) * MPP_ALIGN(encoder->height, 16) / 16, SZ_4K);
        else
            encoder->header_size = MPP_ALIGN(encoder->width, 16) * MPP_ALIGN(encoder->height, 16) / 16;
    } else {
        encoder->header_size = 0;
    }
}

static RK_S32 aq_thd_smart[16] = {
    1,  3,  3,  3,  3,  3,  5,  5,
    8,  8,  8, 15, 15, 20, 25, 28
};

static RK_S32 aq_step_smart[16] = {
    -8, -7, -6, -5, -4, -3, -2, -1,
    0,  1,  2,  3,  4,  6,  8, 10
};

static RK_S32 aq_thd[16] = {
    0,  0,  0,  0,
    3,  3,  5,  5,
    8,  8,  8,  15,
    15, 20, 25, 25
};

static RK_S32 aq_step_i_ipc[16] = {
    -8, -7, -6, -5,
    -4, -3, -2, -1,
    0,  1,  2,  3,
    5,  7,  7,  8,
};

static RK_S32 aq_step_p_ipc[16] = {
    -8, -7, -6, -5,
    -4, -2, -1, -1,
    0,  2,  3,  4,
    6,  8,  9,  10,
};

MPP_RET rkenc_cfg_setup(rkencoder *p)
{
    MppApi *mpi = p->mpi;
    MppCtx ctx = p->ctx;
    MppEncCfg cfg = p->cfg;
    MPP_RET ret;
    RK_U32 rotation;
    RK_U32 mirroring;
    RK_U32 flip;
    RK_U32 gop_mode = p->gop_mode;
    MppEncRefCfg ref = NULL;
    /* setup default parameter */
    if (p->fps_in_den == 0)
        p->fps_in_den = 1;
    if (p->fps_in_num == 0)
        p->fps_in_num = 30;
    if (p->fps_out_den == 0)
        p->fps_out_den = 1;
    if (p->fps_out_num == 0)
        p->fps_out_num = 30;

    if (!p->bps)
        p->bps = p->width * p->height / 8 * (p->fps_out_num / p->fps_out_den);

    if (p->rc_mode == MPP_ENC_RC_MODE_SMTRC) {
        mpp_enc_cfg_set_st(cfg, "hw:aq_thrd_i", aq_thd_smart);
        mpp_enc_cfg_set_st(cfg, "hw:aq_thrd_p", aq_thd_smart);
        mpp_enc_cfg_set_st(cfg, "hw:aq_step_i", aq_step_smart);
        mpp_enc_cfg_set_st(cfg, "hw:aq_step_p", aq_step_smart);
    } else {
        mpp_enc_cfg_set_st(cfg, "hw:aq_thrd_i", aq_thd);
        mpp_enc_cfg_set_st(cfg, "hw:aq_thrd_p", aq_thd);
        mpp_enc_cfg_set_st(cfg, "hw:aq_step_i", aq_step_i_ipc);
        mpp_enc_cfg_set_st(cfg, "hw:aq_step_p", aq_step_p_ipc);
    }

    mpp_enc_cfg_set_s32(cfg, "rc:max_reenc_times", 0);
    mpp_enc_cfg_set_s32(cfg, "rc:cu_qp_delta_depth", p->cu_qp_delta_depth);
    mpp_enc_cfg_set_s32(cfg, "tune:anti_flicker_str", p->anti_flicker_str);
    mpp_enc_cfg_set_s32(cfg, "tune:atr_str_i", p->atr_str_i);
    mpp_enc_cfg_set_s32(cfg, "tune:atr_str_p", p->atr_str_p);
    mpp_enc_cfg_set_s32(cfg, "tune:atl_str", p->atl_str);
    mpp_enc_cfg_set_s32(cfg, "tune:sao_str_i", p->sao_str_i);
    mpp_enc_cfg_set_s32(cfg, "tune:sao_str_p", p->sao_str_p);

    mpp_enc_cfg_set_s32(cfg, "tune:scene_mode", p->scene_mode);
    // mpp_enc_cfg_set_s32(cfg, "tune:deblur_en", cmd->deblur_en);
    // mpp_enc_cfg_set_s32(cfg, "tune:deblur_str", cmd->deblur_str);
    // mpp_enc_cfg_set_s32(cfg, "tune:rc_container", cmd->rc_container);
    mpp_enc_cfg_set_s32(cfg, "tune:vmaf_opt", 0);
    mpp_enc_cfg_set_s32(cfg, "hw:qbias_en", 1);
    // mpp_enc_cfg_set_s32(cfg, "hw:qbias_i", cmd->bias_i);
    // mpp_enc_cfg_set_s32(cfg, "hw:qbias_p", cmd->bias_p);
    mpp_enc_cfg_set_s32(cfg, "hw:skip_bias_en", 0);
    mpp_enc_cfg_set_s32(cfg, "hw:skip_bias", 4);
    mpp_enc_cfg_set_s32(cfg, "hw:skip_sad", 8);

    mpp_enc_cfg_set_s32(cfg, "prep:width", p->width);
    mpp_enc_cfg_set_s32(cfg, "prep:height", p->height);
    mpp_enc_cfg_set_s32(cfg, "prep:hor_stride", p->hor_stride);
    mpp_enc_cfg_set_s32(cfg, "prep:ver_stride", p->ver_stride);
    mpp_enc_cfg_set_s32(cfg, "prep:format", p->fmt);
    mpp_enc_cfg_set_s32(cfg, "prep:range", MPP_FRAME_RANGE_JPEG);

    mpp_enc_cfg_set_s32(cfg, "rc:mode", p->rc_mode);
    mpp_enc_cfg_set_u32(cfg, "rc:max_reenc_times", 0);
    mpp_enc_cfg_set_u32(cfg, "rc:super_mode", 0);

    /* fix input / output frame rate */
    mpp_enc_cfg_set_s32(cfg, "rc:fps_in_flex", p->fps_in_flex);
    mpp_enc_cfg_set_s32(cfg, "rc:fps_in_num", p->fps_in_num);
    mpp_enc_cfg_set_s32(cfg, "rc:fps_in_denom", p->fps_in_den);
    mpp_enc_cfg_set_s32(cfg, "rc:fps_out_flex", p->fps_out_flex);
    mpp_enc_cfg_set_s32(cfg, "rc:fps_out_num", p->fps_out_num);
    mpp_enc_cfg_set_s32(cfg, "rc:fps_out_denom", p->fps_out_den);

    /* drop frame or not when bitrate overflow */
    mpp_enc_cfg_set_u32(cfg, "rc:drop_mode", MPP_ENC_RC_DROP_FRM_DISABLED);
    mpp_enc_cfg_set_u32(cfg, "rc:drop_thd", 20);        /* 20% of max bps */
    mpp_enc_cfg_set_u32(cfg, "rc:drop_gap", 1);         /* Do not continuous drop frame */

    /* setup bitrate for different rc_mode */
    mpp_enc_cfg_set_s32(cfg, "rc:bps_target", p->bps);
    switch (p->rc_mode) {
    case MPP_ENC_RC_MODE_FIXQP : {
        /* do not setup bitrate on FIXQP mode */
    } break;
    case MPP_ENC_RC_MODE_CBR : {
        /* CBR mode has narrow bound */
        mpp_enc_cfg_set_s32(cfg, "rc:bps_max", p->bps_max ? p->bps_max : p->bps * 17 / 16);
        mpp_enc_cfg_set_s32(cfg, "rc:bps_min", p->bps_min ? p->bps_min : p->bps * 15 / 16);
    } break;
    case MPP_ENC_RC_MODE_VBR :
    case MPP_ENC_RC_MODE_AVBR : {
        /* VBR mode has wide bound */
        mpp_enc_cfg_set_s32(cfg, "rc:bps_max", p->bps_max ? p->bps_max : p->bps * 17 / 16);
        mpp_enc_cfg_set_s32(cfg, "rc:bps_min", p->bps_min ? p->bps_min : p->bps * 1 / 16);
    } break;
    default : {
        /* default use CBR mode */
        mpp_enc_cfg_set_s32(cfg, "rc:bps_max", p->bps_max ? p->bps_max : p->bps * 17 / 16);
        mpp_enc_cfg_set_s32(cfg, "rc:bps_min", p->bps_min ? p->bps_min : p->bps * 15 / 16);
    } break;
    }

    /* setup qp for different codec and rc_mode */
    switch (p->type) {
    case MPP_VIDEO_CodingAVC :
    case MPP_VIDEO_CodingHEVC : {
        switch (p->rc_mode) {
        case MPP_ENC_RC_MODE_FIXQP : {
            RK_S32 fix_qp = 0;

            mpp_enc_cfg_set_s32(cfg, "rc:qp_init", fix_qp);
            mpp_enc_cfg_set_s32(cfg, "rc:qp_max", fix_qp);
            mpp_enc_cfg_set_s32(cfg, "rc:qp_min", fix_qp);
            mpp_enc_cfg_set_s32(cfg, "rc:qp_max_i", fix_qp);
            mpp_enc_cfg_set_s32(cfg, "rc:qp_min_i", fix_qp);
            mpp_enc_cfg_set_s32(cfg, "rc:qp_ip", 0);
            mpp_enc_cfg_set_s32(cfg, "rc:fqp_min_i", fix_qp);
            mpp_enc_cfg_set_s32(cfg, "rc:fqp_max_i", fix_qp);
            mpp_enc_cfg_set_s32(cfg, "rc:fqp_min_p", fix_qp);
            mpp_enc_cfg_set_s32(cfg, "rc:fqp_max_p", fix_qp);
        } break;
        case MPP_ENC_RC_MODE_CBR :
        case MPP_ENC_RC_MODE_VBR :
        case MPP_ENC_RC_MODE_AVBR :
        case MPP_ENC_RC_MODE_SMTRC : {
            mpp_enc_cfg_set_s32(cfg, "rc:qp_init", -1);
            mpp_enc_cfg_set_s32(cfg, "rc:qp_max", 51);
            mpp_enc_cfg_set_s32(cfg, "rc:qp_min", 10);
            mpp_enc_cfg_set_s32(cfg, "rc:qp_max_i", 51);
            mpp_enc_cfg_set_s32(cfg, "rc:qp_min_i", 10);
            mpp_enc_cfg_set_s32(cfg, "rc:qp_ip", 2);
            mpp_enc_cfg_set_s32(cfg, "rc:fqp_min_i", 10);
            mpp_enc_cfg_set_s32(cfg, "rc:fqp_max_i", 45);
            mpp_enc_cfg_set_s32(cfg, "rc:fqp_min_p", 10);
            mpp_enc_cfg_set_s32(cfg, "rc:fqp_max_p", 45);
        } break;
        default : {
            printf("unsupport encoder rc mode %d\n", p->rc_mode);
        } break;
        }
    } break;
    case MPP_VIDEO_CodingVP8 : {
        /* vp8 only setup base qp range */
        mpp_enc_cfg_set_s32(cfg, "rc:qp_init", 40);
        mpp_enc_cfg_set_s32(cfg, "rc:qp_max",  127);
        mpp_enc_cfg_set_s32(cfg, "rc:qp_min",  0);
        mpp_enc_cfg_set_s32(cfg, "rc:qp_max_i", 127);
        mpp_enc_cfg_set_s32(cfg, "rc:qp_min_i", 0);
        mpp_enc_cfg_set_s32(cfg, "rc:qp_ip", 6);
    } break;
    case MPP_VIDEO_CodingMJPEG : {
        /* jpeg use special codec config to control qtable */
        mpp_enc_cfg_set_s32(cfg, "jpeg:q_factor", 80);
        mpp_enc_cfg_set_s32(cfg, "jpeg:qf_max", 99);
        mpp_enc_cfg_set_s32(cfg, "jpeg:qf_min", 1);
    } break;
    default : {
    } break;
    }

    /* setup codec  */
    mpp_enc_cfg_set_s32(cfg, "codec:type", p->type);
    switch (p->type) {
    case MPP_VIDEO_CodingAVC : {
        RK_U32 constraint_set;

        /*
         * H.264 profile_idc parameter
         * 66  - Baseline profile
         * 77  - Main profile
         * 100 - High profile
         */
        mpp_enc_cfg_set_s32(cfg, "h264:profile", 100);
        /*
         * H.264 level_idc parameter
         * 10 / 11 / 12 / 13    - qcif@15fps / cif@7.5fps / cif@15fps / cif@30fps
         * 20 / 21 / 22         - cif@30fps / half-D1@@25fps / D1@12.5fps
         * 30 / 31 / 32         - D1@25fps / 720p@30fps / 720p@60fps
         * 40 / 41 / 42         - 1080p@30fps / 1080p@30fps / 1080p@60fps
         * 50 / 51 / 52         - 4K@30fps
         */
        mpp_enc_cfg_set_s32(cfg, "h264:level", 40);
        mpp_enc_cfg_set_s32(cfg, "h264:cabac_en", 1);
        mpp_enc_cfg_set_s32(cfg, "h264:cabac_idc", 0);
        mpp_enc_cfg_set_s32(cfg, "h264:trans8x8", 1);

        mpp_env_get_u32("constraint_set", &constraint_set, 0);
        if (constraint_set & 0x3f0000)
            mpp_enc_cfg_set_s32(cfg, "h264:constraint_set", constraint_set);
    } break;
    case MPP_VIDEO_CodingHEVC :
    case MPP_VIDEO_CodingMJPEG :
    case MPP_VIDEO_CodingVP8 : {
    } break;
    default : {
        printf("unsupport encoder coding type %d\n", p->type);
    } break;
    }

    p->split_mode = 0;
    p->split_arg = 0;
    p->split_out = 0;

    mpp_env_get_u32("split_mode", &p->split_mode, MPP_ENC_SPLIT_NONE);
    mpp_env_get_u32("split_arg", &p->split_arg, 0);
    mpp_env_get_u32("split_out", &p->split_out, 0);

    if (p->split_mode) {
        // mpp_log_q(quiet, "%p split mode %d arg %d out %d\n", ctx,
        //           p->split_mode, p->split_arg, p->split_out);
        mpp_enc_cfg_set_s32(cfg, "split:mode", p->split_mode);
        mpp_enc_cfg_set_s32(cfg, "split:arg", p->split_arg);
        mpp_enc_cfg_set_s32(cfg, "split:out", p->split_out);
    }

    mpp_env_get_u32("mirroring", &mirroring, 0);
    mpp_env_get_u32("rotation", &rotation, 0);
    mpp_env_get_u32("flip", &flip, 0);

    mpp_enc_cfg_set_s32(cfg, "prep:mirroring", mirroring);
    mpp_enc_cfg_set_s32(cfg, "prep:rotation", rotation);
    mpp_enc_cfg_set_s32(cfg, "prep:flip", flip);

    // config gop_len and ref cfg
    mpp_enc_cfg_set_s32(cfg, "rc:gop", p->gop_len ? p->gop_len : p->fps_out_num * 2);

    mpp_env_get_u32("gop_mode", &gop_mode, gop_mode);
    if (gop_mode) {
        // mpp_enc_ref_cfg_init(&ref);

        // if (p->gop_mode < 4)
        //     mpi_enc_gen_ref_cfg(ref, gop_mode);
        // else
        //     mpi_enc_gen_smart_gop_ref_cfg(ref, p->gop_len, p->vi_len);

        // mpp_enc_cfg_set_ptr(cfg, "rc:ref_cfg", ref);
    }

    ret = mpi->control(ctx, MPP_ENC_SET_CFG, cfg);
    if (ret) {
        printf("mpi control enc set cfg failed ret %d\n", ret);
        goto RET;
    }

    if (p->type == MPP_VIDEO_CodingAVC || p->type == MPP_VIDEO_CodingHEVC) {
        RcApiBrief rc_api_brief;
        rc_api_brief.type = p->type;
        rc_api_brief.name = (p->rc_mode == MPP_ENC_RC_MODE_SMTRC) ?
                            "smart" : "default";

        ret = mpi->control(ctx, MPP_ENC_SET_RC_API_CURRENT, &rc_api_brief);
        if (ret) {
            printf("mpi control enc set rc api failed ret %d\n", ret);
            goto RET;
        }
    }

    if (ref)
        mpp_enc_ref_cfg_deinit(&ref);

    /* optional */
    {
        RK_U32 sei_mode;
        mpp_env_get_u32("sei_mode", &sei_mode, MPP_ENC_SEI_MODE_DISABLE);
        p->sei_mode = (MppEncSeiMode)sei_mode;
        ret = mpi->control(ctx, MPP_ENC_SET_SEI_CFG, &p->sei_mode);
        if (ret) {
            printf("mpi control enc set sei cfg failed ret %d\n", ret);
            goto RET;
        }
    }

    if (p->type == MPP_VIDEO_CodingAVC || p->type == MPP_VIDEO_CodingHEVC) {
        p->header_mode = MPP_ENC_HEADER_MODE_EACH_IDR;
        ret = mpi->control(ctx, MPP_ENC_SET_HEADER_MODE, &p->header_mode);
        if (ret) {
            printf("mpi control enc set header mode failed ret %d\n", ret);
            goto RET;
        }
    }

    /* setup test mode by env */
    mpp_env_get_u32("osd_enable", &p->osd_enable, 0);
    mpp_env_get_u32("osd_mode", &p->osd_mode, MPP_ENC_OSD_PLT_TYPE_DEFAULT);
    mpp_env_get_u32("roi_enable", &p->roi_enable, 0);
    mpp_env_get_u32("user_data_enable", &p->user_data_enable, 0);

    // if (p->roi_enable) {
    //     mpp_enc_roi_init(&p->roi_ctx, p->width, p->height, p->type, 4);
    //     mpp_assert(p->roi_ctx);
    // }

RET:
    return ret;
}

MPP_RET rkencode_reset(rkencoder * p)
{
    MPP_RET ret = MPP_OK;
    ret = p->mpi->reset(p->ctx);
    return ret;
}

MPP_RET rkencode_setup(rkencoder * p)
{
    MPP_RET ret = MPP_OK;
    MppPollType timeout = MPP_POLL_BLOCK;

    ret = mpp_buffer_group_get_internal(&p->buf_grp, (MppBufferType)(MPP_BUFFER_TYPE_DRM | MPP_BUFFER_FLAGS_CACHABLE));
    if (ret) {
        printf("failed to get mpp buffer group ret %d\n", ret);
        return ret;
    }

    ret = mpp_buffer_get(p->buf_grp, &p->frm_buf, p->frame_size + p->header_size);
    if (ret) {
        printf("failed to get buffer for input frame ret %d\n", ret);
        return ret;
    }

    ret = mpp_buffer_get(p->buf_grp, &p->pkt_buf, p->frame_size);
    if (ret) {
        printf("failed to get buffer for output packet ret %d\n", ret);
        return ret;
    }

    ret = mpp_buffer_get(p->buf_grp, &p->md_info, p->mdinfo_size);
    if (ret) {
        printf("failed to get buffer for motion info output packet ret %d\n", ret);
        return ret;
    }

    // encoder demo
    ret = mpp_create(&p->ctx, &p->mpi);
    if (ret) {
        printf("mpp_create failed ret %d\n", ret);
        return ret;
    }

    // mpp_log_q(1, "%p encoder test start w %d h %d type %d\n",
    //           p->ctx, p->width, p->height, p->type);

    ret = p->mpi->control(p->ctx, MPP_SET_OUTPUT_TIMEOUT, &timeout);
    if (MPP_OK != ret) {
        printf("mpi control set output timeout %d ret %d\n", timeout, ret);
        return ret;
    }

    ret = mpp_init(p->ctx, MPP_CTX_ENC, p->type);
    if (ret) {
        printf("mpp_init failed ret %d\n", ret);
        return ret;
    }

    ret = mpp_enc_cfg_init(&p->cfg);
    if (ret) {
        printf("mpp_enc_cfg_init failed ret %d\n", ret);
        return ret;
    }

    ret = p->mpi->control(p->ctx, MPP_ENC_GET_CFG, p->cfg);
    if (ret) {
        printf("get enc cfg failed ret %d\n", ret);
        return ret;
    }

    ret = rkenc_cfg_setup(p);
    if (ret) {
        printf("test mpp setup failed ret %d\n", ret);
        return ret;
    }
    return ret;
}

MPP_RET rkencode_put_frame(rkencoder *p, void* frame_buf)
{
    MppApi *mpi = p->mpi;
    MppCtx ctx = p->ctx;
    MppPacket packet = p->packet;
    MppMeta meta = NULL;
    MppFrame frame = NULL;
    RK_U32 eoi = 1;
    MPP_RET ret = MPP_OK;

    ret = mpp_frame_init(&frame);
    if (ret) {
        printf("mpp_frame_init failed\n");
        return ret;
    }

    mpp_frame_set_width(frame, p->width);
    mpp_frame_set_height(frame, p->height);
    mpp_frame_set_hor_stride(frame, p->hor_stride);
    mpp_frame_set_ver_stride(frame, p->ver_stride);
    mpp_frame_set_fmt(frame, p->fmt);
    mpp_frame_set_eos(frame, p->frm_eos);

    mpp_frame_set_buffer(frame, frame_buf);

    meta = mpp_frame_get_meta(frame);
    mpp_packet_init_with_buffer(&packet, p->pkt_buf);
    /* NOTE: It is important to clear output packet length!! */
    mpp_packet_set_length(packet, 0);
    mpp_meta_set_packet(meta, KEY_OUTPUT_PACKET, packet);
    mpp_meta_set_buffer(meta, KEY_MOTION_INFO, p->md_info);

    ret = mpi->encode_put_frame(ctx, frame);
    if (ret) {
        printf("chn encode put frame failed\n");
        mpp_frame_deinit(&frame);
        return ret;
    }

    mpp_frame_deinit(&frame);
    return ret;
}

MPP_RET rkencode_get_sps(rkencoder *p, void* data, size_t *len)
{
    MPP_RET ret;
    MppApi *mpi = p->mpi;
    MppCtx ctx = p->ctx;
    if (p->type == MPP_VIDEO_CodingAVC || p->type == MPP_VIDEO_CodingHEVC) {
        MppPacket packet = NULL;
        mpp_packet_init_with_buffer(&packet, p->pkt_buf);
        /* NOTE: It is important to clear output packet length!! */
        mpp_packet_set_length(packet, 0);
        ret = mpi->control(ctx, MPP_ENC_GET_HDR_SYNC, packet);
        if (ret) {
            printf("mpi control enc get extra info failed\n");
            return ret;
        } else {
            /* get and write sps/pps for H.264 */
            memcpy(data, mpp_packet_get_pos(packet), mpp_packet_get_length(packet));
            *len = mpp_packet_get_length(packet);
        }
        mpp_packet_deinit(&packet);
        return MPP_OK;
    }
    return MPP_NOK;
}

MPP_RET rkencode_get_data(rkencoder *p, void* buf, size_t *len, int *eoi, bool *is_key_frame)
{
    MppApi *mpi = p->mpi;
    MppCtx ctx = p->ctx;
    MppPacket packet = p->packet;
    *eoi = 1;
    MPP_RET ret = MPP_OK;

    do {
        ret = mpi->encode_get_packet(ctx, &packet);
        if (ret) {
            printf("encode get packet failed\n");
            return ret;
        }

        mpp_assert(packet);

        if (packet) {
            if (!p->first_pkt)
                p->first_pkt = mpp_time();

            *len = mpp_packet_get_length(packet);
            void *data = mpp_packet_get_pos(packet);
            memcpy(buf, data, *len);
            {
                // 判断 data 是否是 I 帧（IDR 帧）
                RK_U8 *pData = (RK_U8 *)data;
                // 判断起始码长度：有时起始码可能为 3 字节（0x000001）或 4 字节（0x00000001）
                int offset = (pData[2] == 0x01) ? 3 : 4;
                int nal_unit_type = pData[offset] & 0x1F;
                if (nal_unit_type == 5) {
                    *is_key_frame = true;
                } else {
                    *is_key_frame = false;
                }
            }

            p->pkt_eos = mpp_packet_get_eos(packet);
            if (mpp_packet_is_partition(packet)) {
                *eoi = mpp_packet_is_eoi(packet);
                p->frm_pkt_cnt = (eoi) ? (0) : (p->frm_pkt_cnt + 1);
            }

            if (p->pkt_eos) {
                mpp_assert(p->frm_eos);
            }
        }
    } while (0);

    return ret;
}

MPP_RET rkencode_packet_release(rkencoder *p)
{
    return mpp_packet_deinit(&p->packet);
}

MPP_RET rkencode_frame(rkencoder *p, void* frame_buf, std::function<void(void*, size_t, bool)> fn, bool is_init)
{
    MppApi *mpi = p->mpi;
    MppCtx ctx = p->ctx;
    MPP_RET ret = MPP_OK;
    if(is_init)
    {
         // 第一帧需要写入 sps/pps
        if (p->type == MPP_VIDEO_CodingAVC || p->type == MPP_VIDEO_CodingHEVC) {
            MppPacket packet = NULL;
            mpp_packet_init_with_buffer(&packet, p->pkt_buf);
            /* NOTE: It is important to clear output packet length!! */
            mpp_packet_set_length(packet, 0);
            ret = mpi->control(ctx, MPP_ENC_GET_HDR_SYNC, packet);
            if (ret) {
                printf("mpi control enc get extra info failed\n");
                goto RET;
            } else {
                /* get and write sps/pps for H.264 */
                fn(mpp_packet_get_pos(packet), mpp_packet_get_length(packet), false);
            }
            mpp_packet_deinit(&packet);
        }
    }
    else
    {
        MppMeta meta = NULL;
        MppFrame frame = NULL;
        MppPacket packet = NULL;
        RK_U32 eoi = 1;

        ret = mpp_frame_init(&frame);
        if (ret) {
            printf("mpp_frame_init failed\n");
            goto RET;
        }
        mpp_frame_set_width(frame, p->width);
        mpp_frame_set_height(frame, p->height);
        mpp_frame_set_hor_stride(frame, p->hor_stride);
        mpp_frame_set_ver_stride(frame, p->ver_stride);
        mpp_frame_set_fmt(frame, p->fmt);
        mpp_frame_set_eos(frame, p->frm_eos);

        mpp_frame_set_buffer(frame, frame_buf);
        
        meta = mpp_frame_get_meta(frame);
        mpp_packet_init_with_buffer(&packet, p->pkt_buf);
        /* NOTE: It is important to clear output packet length!! */
        mpp_packet_set_length(packet, 0);
        mpp_meta_set_packet(meta, KEY_OUTPUT_PACKET, packet);
        mpp_meta_set_buffer(meta, KEY_MOTION_INFO, p->md_info);

        ret = mpi->encode_put_frame(ctx, frame);
        if (ret) {
            printf("chn encode put frame failed\n");
            mpp_frame_deinit(&frame);
            goto RET;
        }

        mpp_frame_deinit(&frame);
        do {
            ret = mpi->encode_get_packet(ctx, &packet);
            if (ret) {
                printf("encode get packet failed\n");
                goto RET;
            }

            mpp_assert(packet);

            if (packet) {
                size_t len = mpp_packet_get_length(packet);
                void *ptr = mpp_packet_get_pos(packet);
                bool is_key_frame = false;
                {
                    // 判断 data 是否是 I 帧（IDR 帧）
                    RK_U8 *pData = (RK_U8 *)ptr;
                    // 判断起始码长度：有时起始码可能为 3 字节（0x000001）或 4 字节（0x00000001）
                    int offset = (pData[2] == 0x01) ? 3 : 4;
                    int nal_unit_type = pData[offset] & 0x1F;
                    if (nal_unit_type == 5 || nal_unit_type == 7) {
                        is_key_frame = true;
                    } else {
                        is_key_frame = false;
                    }
                }
                
                fn(ptr, len, is_key_frame);
                
                if (!p->first_pkt)
                    p->first_pkt = mpp_time();

                p->pkt_eos = mpp_packet_get_eos(packet);

                if (mpp_packet_is_partition(packet)) {
                    eoi = mpp_packet_is_eoi(packet);
                    p->frm_pkt_cnt = (eoi) ? (0) : (p->frm_pkt_cnt + 1);
                }

                mpp_packet_deinit(&packet);

                p->stream_size += len;
                p->frame_count += eoi;

                if (p->pkt_eos) {
                    mpp_assert(p->frm_eos);
                }
            }
        } while (!eoi);
    }
RET:
    return ret;
}
#if 0
MPP_RET rkencode_run(rkencoder *p)
{
    MppApi *mpi = p->mpi;
    MppCtx ctx = p->ctx;
    MPP_RET ret = MPP_OK;
    RK_FLOAT psnr_const = 0;
    RK_U32 sse_unit_in_pixel = 0;
    RK_U32 cap_num = 0;
    RK_U32 quiet = 0;

    if (p->type == MPP_VIDEO_CodingAVC || p->type == MPP_VIDEO_CodingHEVC) {
        MppPacket packet = NULL;

        /*
         * Can use packet with normal malloc buffer as input not pkt_buf.
         * Please refer to vpu_api_legacy.cpp for normal buffer case.
         * Using pkt_buf buffer here is just for simplifing demo.
         */
        mpp_packet_init_with_buffer(&packet, p->pkt_buf);
        /* NOTE: It is important to clear output packet length!! */
        mpp_packet_set_length(packet, 0);

        ret = mpi->control(ctx, MPP_ENC_GET_HDR_SYNC, packet);
        if (ret) {
            printf("mpi control enc get extra info failed\n");
            goto RET;
        } else {
            /* get and write sps/pps for H.264 */

            void *ptr   = mpp_packet_get_pos(packet);
            size_t len  = mpp_packet_get_length(packet);

            if (p->fp_output)
                fwrite(ptr, 1, len, p->fp_output);
        }

        mpp_packet_deinit(&packet);

        sse_unit_in_pixel = p->type == MPP_VIDEO_CodingAVC ? 16 : 8;
        psnr_const = (16 + log2(MPP_ALIGN(p->width, sse_unit_in_pixel) *
                                MPP_ALIGN(p->height, sse_unit_in_pixel)));
    }
    while (!p->pkt_eos) {
        MppMeta meta = NULL;
        MppFrame frame = NULL;
        MppPacket packet = NULL;
        void *buf = mpp_buffer_get_ptr(p->frm_buf);
        RK_S32 cam_frm_idx = -1;
        MppBuffer cam_buf = NULL;
        RK_U32 eoi = 1;

       {
            cam_frm_idx = camera_source_get_frame(p->cam_ctx);
            mpp_assert(cam_frm_idx >= 0);

            /* skip unstable frames */
            if (cap_num++ < 50) {
                camera_source_put_frame(p->cam_ctx, cam_frm_idx);
                continue;
            }

            cam_buf = camera_frame_to_buf(p->cam_ctx, cam_frm_idx);
            mpp_assert(cam_buf);
        }

        ret = mpp_frame_init(&frame);
        if (ret) {
            printf("mpp_frame_init failed\n");
            goto RET;
        }

        mpp_frame_set_width(frame, p->width);
        mpp_frame_set_height(frame, p->height);
        mpp_frame_set_hor_stride(frame, p->hor_stride);
        mpp_frame_set_ver_stride(frame, p->ver_stride);
        mpp_frame_set_fmt(frame, p->fmt);
        mpp_frame_set_eos(frame, p->frm_eos);

        if (p->fp_input && feof(p->fp_input))
            mpp_frame_set_buffer(frame, NULL);
        else if (cam_buf)
            mpp_frame_set_buffer(frame, cam_buf);
        else
            mpp_frame_set_buffer(frame, p->frm_buf);

        meta = mpp_frame_get_meta(frame);
        mpp_packet_init_with_buffer(&packet, p->pkt_buf);
        /* NOTE: It is important to clear output packet length!! */
        mpp_packet_set_length(packet, 0);
        mpp_meta_set_packet(meta, KEY_OUTPUT_PACKET, packet);
        mpp_meta_set_buffer(meta, KEY_MOTION_INFO, p->md_info);

        if (p->osd_enable || p->user_data_enable || p->roi_enable) {
            if (p->user_data_enable) {
                MppEncUserData user_data;
                char *str = "this is user data\n";

                if ((p->frame_count & 10) == 0) {
                    user_data.pdata = str;
                    user_data.len = strlen(str) + 1;
                    mpp_meta_set_ptr(meta, KEY_USER_DATA, &user_data);
                }
                static RK_U8 uuid_debug_info[16] = {
                    0x57, 0x68, 0x97, 0x80, 0xe7, 0x0c, 0x4b, 0x65,
                    0xa9, 0x06, 0xae, 0x29, 0x94, 0x11, 0xcd, 0x9a
                };

                MppEncUserDataSet data_group;
                MppEncUserDataFull datas[2];
                char *str1 = "this is user data 1\n";
                char *str2 = "this is user data 2\n";
                data_group.count = 2;
                datas[0].len = strlen(str1) + 1;
                datas[0].pdata = str1;
                datas[0].uuid = uuid_debug_info;

                datas[1].len = strlen(str2) + 1;
                datas[1].pdata = str2;
                datas[1].uuid = uuid_debug_info;

                data_group.datas = datas;

                mpp_meta_set_ptr(meta, KEY_USER_DATAS, &data_group);
            }

            if (p->osd_enable) {
                /* gen and cfg osd plt */
                mpi_enc_gen_osd_plt(&p->osd_plt, p->frame_count);

                p->osd_plt_cfg.change = MPP_ENC_OSD_PLT_CFG_CHANGE_ALL;
                p->osd_plt_cfg.type = MPP_ENC_OSD_PLT_TYPE_USERDEF;
                p->osd_plt_cfg.plt = &p->osd_plt;

                ret = mpi->control(ctx, MPP_ENC_SET_OSD_PLT_CFG, &p->osd_plt_cfg);
                if (ret) {
                    printf("mpi control enc set osd plt failed ret %d\n", ret);
                    goto RET;
                }

                /* gen and cfg osd plt */
                mpi_enc_gen_osd_data(&p->osd_data, p->buf_grp, p->width,
                                     p->height, p->frame_count);
                mpp_meta_set_ptr(meta, KEY_OSD_DATA, (void*)&p->osd_data);
            }

            if (p->roi_enable) {
                RoiRegionCfg *region = &p->roi_region;

                /* calculated in pixels */
                region->x = MPP_ALIGN(p->width / 8, 16);
                region->y = MPP_ALIGN(p->height / 8, 16);
                region->w = 128;
                region->h = 256;
                region->force_intra = 0;
                region->qp_mode = 1;
                region->qp_val = 24;

                mpp_enc_roi_add_region(p->roi_ctx, region);

                region->x = MPP_ALIGN(p->width / 2, 16);
                region->y = MPP_ALIGN(p->height / 4, 16);
                region->w = 256;
                region->h = 128;
                region->force_intra = 1;
                region->qp_mode = 1;
                region->qp_val = 10;

                mpp_enc_roi_add_region(p->roi_ctx, region);

                /* send roi info by metadata */
                mpp_enc_roi_setup_meta(p->roi_ctx, meta);
            }
        }

        if (!p->first_frm)
            p->first_frm = mpp_time();
        /*
         * NOTE: in non-block mode the frame can be resent.
         * The default input timeout mode is block.
         *
         * User should release the input frame to meet the requirements of
         * resource creator must be the resource destroyer.
         */
        ret = mpi->encode_put_frame(ctx, frame);
        if (ret) {
            printf("chn %d encode put frame failed\n", chn);
            mpp_frame_deinit(&frame);
            goto RET;
        }

        mpp_frame_deinit(&frame);

        do {
            ret = mpi->encode_get_packet(ctx, &packet);
            if (ret) {
                printf("chn %d encode get packet failed\n", chn);
                goto RET;
            }

            mpp_assert(packet);

            if (packet) {
                // write packet to file here
                void *ptr   = mpp_packet_get_pos(packet);
                size_t len  = mpp_packet_get_length(packet);
                char log_buf[256];
                RK_S32 log_size = sizeof(log_buf) - 1;
                RK_S32 log_len = 0;

                if (!p->first_pkt)
                    p->first_pkt = mpp_time();

                p->pkt_eos = mpp_packet_get_eos(packet);

                if (p->fp_output)
                    fwrite(ptr, 1, len, p->fp_output);

                log_len += snprintf(log_buf + log_len, log_size - log_len,
                                    "encoded frame %-4d", p->frame_count);

                /* for low delay partition encoding */
                if (mpp_packet_is_partition(packet)) {
                    eoi = mpp_packet_is_eoi(packet);

                    log_len += snprintf(log_buf + log_len, log_size - log_len,
                                        " pkt %d", p->frm_pkt_cnt);
                    p->frm_pkt_cnt = (eoi) ? (0) : (p->frm_pkt_cnt + 1);
                }

                log_len += snprintf(log_buf + log_len, log_size - log_len,
                                    " size %-7zu", len);

                if (mpp_packet_has_meta(packet)) {
                    meta = mpp_packet_get_meta(packet);
                    RK_S32 temporal_id = 0;
                    RK_S32 lt_idx = -1;
                    RK_S32 avg_qp = -1, bps_rt = -1;
                    RK_S32 use_lt_idx = -1;
                    RK_S64 sse = 0;
                    RK_FLOAT psnr = 0;

                    if (MPP_OK == mpp_meta_get_s32(meta, KEY_TEMPORAL_ID, &temporal_id))
                        log_len += snprintf(log_buf + log_len, log_size - log_len,
                                            " tid %d", temporal_id);

                    if (MPP_OK == mpp_meta_get_s32(meta, KEY_LONG_REF_IDX, &lt_idx))
                        log_len += snprintf(log_buf + log_len, log_size - log_len,
                                            " lt %d", lt_idx);

                    if (MPP_OK == mpp_meta_get_s32(meta, KEY_ENC_AVERAGE_QP, &avg_qp))
                        log_len += snprintf(log_buf + log_len, log_size - log_len,
                                            " qp %2d", avg_qp);

                    if (MPP_OK == mpp_meta_get_s32(meta, KEY_ENC_BPS_RT, &bps_rt))
                        log_len += snprintf(log_buf + log_len, log_size - log_len,
                                            " bps_rt %d", bps_rt);

                    if (MPP_OK == mpp_meta_get_s32(meta, KEY_ENC_USE_LTR, &use_lt_idx))
                        log_len += snprintf(log_buf + log_len, log_size - log_len, " vi");

                    if (MPP_OK == mpp_meta_get_s64(meta, KEY_ENC_SSE, &sse)) {
                        psnr = 3.01029996 * (psnr_const - log2(sse));
                        log_len += snprintf(log_buf + log_len, log_size - log_len,
                                            " psnr %.4f", psnr);
                    }
                }

                // mpp_log_q(quiet, "chn %d %s\n", chn, log_buf);

                mpp_packet_deinit(&packet);
                fps_calc_inc(cmd->fps);

                p->stream_size += len;
                p->frame_count += eoi;

                if (p->pkt_eos) {
                    // mpp_log_q(quiet, "chn %d found last packet\n", chn);
                    mpp_assert(p->frm_eos);
                }
            }
        } while (!eoi);

        if (cam_frm_idx >= 0)
            camera_source_put_frame(p->cam_ctx, cam_frm_idx);

        if (p->frame_num > 0 && p->frame_count >= p->frame_num)
            break;

        if (p->loop_end)
            break;

        if (p->frm_eos && p->pkt_eos)
            break;
    }
RET:
    MPP_FREE(checkcrc.sum);

    return ret;
}
#endif

void rkencode_deinit(rkencoder * enc)
{
    if (enc->ctx) {
        mpp_destroy(enc->ctx);
        enc->ctx = NULL;
    }

    if (enc->buf_grp) {
        mpp_buffer_group_put(enc->buf_grp);
        enc->buf_grp = NULL;
    }

    if (enc->cfg) {
        mpp_enc_cfg_deinit(enc->cfg);
        enc->cfg = NULL;
    }

    if (enc->frm_buf) {
        mpp_buffer_put(enc->frm_buf);
        enc->frm_buf = NULL;
    }

    if (enc->pkt_buf) {
        mpp_buffer_put(enc->pkt_buf);
        enc->pkt_buf = NULL;
    }

    if (enc->md_info) {
        mpp_buffer_put(enc->md_info);
        enc->md_info = NULL;
    }

    if (enc->roi_ctx) {
        // mpp_enc_roi_deinit(enc->roi_ctx);
        // enc->roi_ctx = NULL;
    }
}

MPP_RET rkencode_init(rkencoder * p, bool highQ)
{
    MPP_RET ret = MPP_OK;
    if(highQ)
    {
        rkencode_setup_720p(p);
    }
    else
    {
        rkencode_setup_480p(p);
    }
    ret = rkencode_setup(p);
    if (ret) {
        printf("test mpp setup failed ret %d\n", ret);
        rkencode_deinit(p);
        return ret;
    }
    return ret;
}
#endif