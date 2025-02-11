#include <alsa/asoundlib.h>
#include <mutex>
#include "rksound.h"


#define RECORD_DEVICE "hw:1,0"
#define PLAYBACK_DEVICE "hw:0,0"

struct rksound
{
    snd_pcm_t* record_handle;
    int frame_size;
    bool is_record_open;
    std::mutex mtx;

    snd_pcm_t* playback_handle;
    bool is_playback_open;
    std::mutex playback_mtx;
};

rksound rk_sound;

void rksound_record_open(void)
{
    snd_pcm_hw_params_t* params;
    unsigned int sample_rate = 8000;
    snd_pcm_uframes_t frames = 32;
    int dir = 0, pcm_rc;

    std::lock_guard<std::mutex> lock(rk_sound.mtx);
    printf("打开音频设备\n");
    if ((pcm_rc = snd_pcm_open(&rk_sound.record_handle, RECORD_DEVICE, SND_PCM_STREAM_CAPTURE, 0)) < 0) {
        printf("ERROR: Can't open PCM device %s (%s)\n", RECORD_DEVICE, snd_strerror(pcm_rc));
        return;
    }
    printf("分配音频参数空间\n");
    snd_pcm_hw_params_alloca(&params);
    printf("初始化音频参数\n");
    snd_pcm_hw_params_any(rk_sound.record_handle, params);
    printf("设置音频参数\n");
    snd_pcm_hw_params_set_access(rk_sound.record_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    printf("设置音频格式\n");
    snd_pcm_hw_params_set_format(rk_sound.record_handle, params, SND_PCM_FORMAT_S16_LE);
    printf("设置音频通道\n");
    snd_pcm_hw_params_set_channels(rk_sound.record_handle, params, 1);
    printf("设置音频采样率\n");
    snd_pcm_hw_params_set_rate_near(rk_sound.record_handle, params, &sample_rate, &dir);
    printf("设置音频周期\n");
    snd_pcm_hw_params_set_period_size_near(rk_sound.record_handle, params, &frames, &dir);
    printf("将参数写入驱动\n");
    if ((pcm_rc = snd_pcm_hw_params(rk_sound.record_handle, params)) < 0) {
        printf("ERROR: Can't set hardware parameters (%s)\n", snd_strerror(pcm_rc));
        snd_pcm_close(rk_sound.record_handle);
        return;
    }
    printf("释放参数空间\n");
    //snd_pcm_hw_params_free(params);
    printf("准备音频接口\n");
    int err;
    if((err=snd_pcm_prepare(rk_sound.record_handle))<0) 
	{
		printf("无法使用音频接口 (%s)\n",snd_strerror(err));
        snd_pcm_close(rk_sound.record_handle);
        return;
	}

    printf("获取周期大小\n");
    snd_pcm_hw_params_get_period_size(params, &frames, &dir);
    rk_sound.frame_size = frames * 4;  // 2 bytes/sample * 2 channels
    rk_sound.is_record_open = true;
    printf("音频设备已打开 frame_size:%d\n", rk_sound.frame_size);
}

void rksound_record_close(void)
{
    std::lock_guard<std::mutex> lock(rk_sound.mtx);

    snd_pcm_close(rk_sound.record_handle);
    rk_sound.is_record_open = false;
}

int rksound_record_get_frame_size(void)
{
    return rk_sound.frame_size;
}

int rksound_record_get_frame(uint8_t *frame, int frame_size)
{
    std::lock_guard<std::mutex> lock(rk_sound.mtx);

    if(!rk_sound.is_record_open)
    {
        //printf("ERROR: PCM device is not open\n");
        return -1;
    }
    int rc = snd_pcm_readi(rk_sound.record_handle, frame, frame_size);
    if (rc < 0) {
        printf("ERROR: Read from PCM device failed (%s)\n", snd_strerror(rc));
        return -1;
    }
    return rc;
}

void rksound_play_open(void)
{
    snd_pcm_hw_params_t* params = nullptr;
    unsigned int sample_rate = 8000;
    snd_pcm_uframes_t frames = 32;
    int err, dir = 0;
    std::lock_guard<std::mutex> lock(rk_sound.playback_mtx);

    printf("打开播放设备\n");
    if ((err = snd_pcm_open(&rk_sound.playback_handle, PLAYBACK_DEVICE, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
        printf("ERROR: 无法打开播放设备 (%s)\n", snd_strerror(err));
        return;
    }

    snd_pcm_hw_params_alloca(&params);
    if ((err = snd_pcm_hw_params_any(rk_sound.playback_handle, params)) < 0) {
        printf("ERROR: 无法初始化播放设备参数 (%s)\n", snd_strerror(err));
        snd_pcm_close(rk_sound.playback_handle);
        return;
    }

    if ((err = snd_pcm_hw_params_set_access(rk_sound.playback_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
        printf("ERROR: 无法设置访问模式 (%s)\n", snd_strerror(err));
        snd_pcm_close(rk_sound.playback_handle);
        return;
    }

    if ((err = snd_pcm_hw_params_set_format(rk_sound.playback_handle, params, SND_PCM_FORMAT_S16_LE)) < 0) {
        printf("ERROR: 无法设置音频格式 (%s)\n", snd_strerror(err));
        snd_pcm_close(rk_sound.playback_handle);
        return;
    }

    if ((err = snd_pcm_hw_params_set_channels(rk_sound.playback_handle, params, 2)) < 0) {
        printf("ERROR: 无法设置音频通道 (%s)\n", snd_strerror(err));
        snd_pcm_close(rk_sound.playback_handle);
        return;
    }

    if ((err = snd_pcm_hw_params_set_rate_near(rk_sound.playback_handle, params, &sample_rate, &dir)) < 0) {
        printf("ERROR: 无法设置采样率 (%s)\n", snd_strerror(err));
        snd_pcm_close(rk_sound.playback_handle);
        return;
    }

    if ((err = snd_pcm_hw_params_set_period_size_near(rk_sound.playback_handle, params, &frames, &dir)) < 0) {
        printf("ERROR: 无法设置周期大小 (%s)\n", snd_strerror(err));
        snd_pcm_close(rk_sound.playback_handle);
        return;
    }

    if ((err = snd_pcm_hw_params(rk_sound.playback_handle, params)) < 0) {
        printf("ERROR: 无法写入播放参数 (%s)\n", snd_strerror(err));
        snd_pcm_close(rk_sound.playback_handle);
        return;
    }

    if ((err = snd_pcm_prepare(rk_sound.playback_handle)) < 0) {
        printf("ERROR: 播放设备准备失败 (%s)\n", snd_strerror(err));
        snd_pcm_close(rk_sound.playback_handle);
        return;
    }
    rk_sound.is_playback_open = true;
    printf("播放设备已打开\n");
}

void rksound_play_close(void)
{
    std::lock_guard<std::mutex> lock(rk_sound.playback_mtx);

    snd_pcm_drain(rk_sound.playback_handle);
    snd_pcm_close(rk_sound.playback_handle);
    rk_sound.is_playback_open = false;
    printf("播放设备已关闭\n");
}

void rksound_play_pcm(const uint8_t *pcm_data, int data_size)
{
    if(rk_sound.is_playback_open == false)
    {
        return;
    }
    std::lock_guard<std::mutex> lock(rk_sound.playback_mtx);

    int bytes_per_frame = 2; // 单通道 16 位 = 2 字节
    snd_pcm_uframes_t total_frames = data_size / bytes_per_frame;
    const uint8_t* ptr = pcm_data;
    printf("开始播放音频数据，总帧数:%lu\n", (unsigned long)total_frames);
    while (total_frames > 0) {
        int frames_written = snd_pcm_writei(rk_sound.playback_handle, ptr, total_frames);
        if (frames_written < 0) {
            frames_written = snd_pcm_recover(rk_sound.playback_handle, frames_written, 0);
            if (frames_written < 0) {
                printf("ERROR: 播放错误 (%s)\n", snd_strerror(frames_written));
                break;
            }
        }
        total_frames -= frames_written;
        ptr += frames_written * bytes_per_frame;
    }
}