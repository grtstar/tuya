#ifndef RKCAMERA_H
#define RKCAMERA_H
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif
void* rkcamera_open(void);
int rkcamera_get_frame(void* dev, void* frame_buf, int wait_ms, bool *is_key_frame);
int rkcamera_close(void* dev);
#ifdef __cplusplus
}
#endif
#endif // RKCAMERA_H