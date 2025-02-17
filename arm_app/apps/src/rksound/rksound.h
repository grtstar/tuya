#ifndef __RKSOUND_H__
#define __RKSOUND_H__

#include <stdint.h>
#include <portaudio.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef int (*audioCallback)(const void* inputBuffer, void* outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void* userData);

void rksound_init(void);
void rksound_record_open(int sample_rate, int frame_size, int channels, audioCallback callback, void* userdata);
void rksound_record_close(void);
void rksound_play_open(int sample_rate, int frame_size, int channels, audioCallback callback, void* userdata);
void rksound_play_close(void);
int  rksound_record_get_frame(uint8_t* frame, int frame_size);
int  rksound_play_pcm(uint8_t* frame, int frame_size);

#ifdef __cplusplus
}
#endif

#endif // __RKSOUND_H__