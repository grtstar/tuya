#ifndef __RKSOUND_H__
#define __RKSOUND_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
    
    void rksound_record_open(void);
    void rksound_record_close(void);
    int rksound_record_get_frame_size(void);
    int rksound_record_get_frame(uint8_t *frame, int frame_size);
    void rksound_play_open(void);
    void rksound_play_close(void);
    void rksound_play_pcm(const uint8_t *pcm_data, int data_size);
#ifdef __cplusplus
}
#endif

#endif // __RKSOUND_H__