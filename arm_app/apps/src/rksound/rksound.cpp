#include <stdio.h>
#include "rksound.h"

struct rksound
{
    bool pa_initialized;
    PaStream *playback_stream;
    PaStream *record_stream;
} rk_sound;


void rksound_init()
{
    if(rk_sound.pa_initialized == true) {
        return;
    }
    PaError err;
    // 初始化 PortAudio 库
    err = Pa_Initialize();
    if (err != paNoError) {
        printf("初始化 PortAudio 失败：%s\n", Pa_GetErrorText(err));
        return;
    }
    rk_sound.pa_initialized = true;
}

void rksound_record_open(int sample_rate, int frame_size, int channels, audioCallback callback, void* userdata)
{
    rksound_init();
    PaError err;
    PaStreamParameters input_params;
    input_params.device = 1;
    input_params.channelCount = channels;
    input_params.sampleFormat = paInt16;
    input_params.suggestedLatency = Pa_GetDeviceInfo(input_params.device)->defaultLowInputLatency;
    input_params.hostApiSpecificStreamInfo = NULL;
    printf("打开录音设备\n");
    err = Pa_OpenStream(&rk_sound.record_stream, &input_params, NULL, sample_rate, frame_size, paClipOff, callback, userdata);
    if (err != paNoError) {
        printf("打开录音设备失败：%s\n", Pa_GetErrorText(err));
        return;
    }
    printf("启动录音设备\n");
    err = Pa_StartStream(rk_sound.record_stream);
    if (err != paNoError) {
        printf("启动录音设备失败：%s\n", Pa_GetErrorText(err));
        return;
    }
}

void rksound_record_close()
{
    PaError err;
    err = Pa_StopStream(rk_sound.record_stream);
    if (err != paNoError) {
        printf("停止录音设备失败：%s\n", Pa_GetErrorText(err));
        return;
    }

    err = Pa_CloseStream(rk_sound.record_stream);
    if (err != paNoError) {
        printf("关闭录音设备失败：%s\n", Pa_GetErrorText(err));
        return;
    }
}

void rksound_play_open(int sample_rate, int frame_size, int channels, audioCallback callback, void* userdata)
{
    rksound_init();
    PaError err;
    PaStreamParameters output_params;
    output_params.device = Pa_GetDefaultOutputDevice();
    output_params.channelCount = channels;
    output_params.sampleFormat = paInt16;
    output_params.suggestedLatency = Pa_GetDeviceInfo(output_params.device)->defaultLowOutputLatency;
    output_params.hostApiSpecificStreamInfo = NULL;

    err = Pa_OpenStream(&rk_sound.playback_stream, NULL, &output_params, sample_rate, frame_size, paClipOff, callback, userdata);
    if (err != paNoError) {
        printf("打开播放设备失败：%s\n", Pa_GetErrorText(err));
        return;
    }

    err = Pa_StartStream(rk_sound.playback_stream);
    if (err != paNoError) {
        printf("启动播放设备失败：%s\n", Pa_GetErrorText(err));
        return;
    }
}

void rksound_play_close()
{
    PaError err;
    err = Pa_StopStream(rk_sound.playback_stream);
    if (err != paNoError) {
        printf("停止播放设备失败：%s\n", Pa_GetErrorText(err));
        return;
    }

    err = Pa_CloseStream(rk_sound.playback_stream);
    if (err != paNoError) {
        printf("关闭播放设备失败：%s\n", Pa_GetErrorText(err));
        return;
    }
}

int rksound_record_get_frame(uint8_t *frame, int frame_size)
{
    PaError err;
    err = Pa_ReadStream(rk_sound.record_stream, frame, frame_size);
    if (err != paNoError) {
        printf("读取录音设备失败：%s\n", Pa_GetErrorText(err));
        return -1;
    }
    return 0;
}

int rksound_play_pcm(uint8_t *frame, int frame_size)
{
    PaError err;
    err = Pa_WriteStream(rk_sound.playback_stream, frame, frame_size);
    if (err != paNoError) {
        printf("写入播放设备失败：%s\n", Pa_GetErrorText(err));
        return -1;
    }
    return 0;
}

#if 0
#include <alsa/asoundlib.h>
#include <vector>
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
    snd_pcm_hw_params_set_channels(rk_sound.record_handle, params, 4);
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
    // 单声道数据转为双声道
    std::vector<uint8_t> stereo_buffer(data_size * 2);
    for (int i = 0, j = 0; i < data_size; i += 2, j += 4) {
        stereo_buffer[j]     = pcm_data[i];
        stereo_buffer[j + 1] = pcm_data[i + 1];
        stereo_buffer[j + 2] = pcm_data[i];
        stereo_buffer[j + 3] = pcm_data[i + 1];
    }
    pcm_data = stereo_buffer.data();
    data_size *= 2;
    int bytes_per_frame = 4; // 单通道 16 位 = 2 字节
    {
        snd_pcm_uframes_t total_frames = data_size / bytes_per_frame;
        const uint8_t* ptr = pcm_data;
        printf("开始播放音频数据 L，总帧数:%lu\n", (unsigned long)total_frames);
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
}
#endif