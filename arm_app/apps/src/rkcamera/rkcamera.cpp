#include <iostream>
#include <csignal>
#include <unistd.h>
#include <mutex>
#include "rkisp_api.h"
#include "encoder.h"
#include "rkcamera.h"

typedef struct rkcamera {
    bool got_sps;
    bool frame_encoded;
    bool is_open;
    rkencoder encoder;
    const struct rkisp_api_buf* frame;
    const struct rkisp_api_ctx* ctx;
    std::mutex mutex; 
} rkcamera;

void* rkcamera_open(void)
{
    static rkcamera camera;
    std::lock_guard<std::mutex> lock(camera.mutex);
    const char* dev_path = "/dev/video0";
    camera.ctx = rkisp_open_device(dev_path, 0);
    if (rkisp_set_fmt(camera.ctx, 1280, 720, V4L2_PIX_FMT_NV12) != 0) {
        std::cerr << "Failed to set format" << std::endl;
        rkisp_close_device(camera.ctx);
        return NULL;
    }

    // Request buffers. Using 4 buffers as an example.
    if (rkisp_set_buf(camera.ctx, 4, nullptr, 0) != 0) {
        std::cerr << "Failed to set buffers" << std::endl;
        rkisp_close_device(camera.ctx);
        return NULL;
    }

    // Start the capture stream.
    if (rkisp_start_capture(camera.ctx) != 0) {
        std::cerr << "Failed to start capture" << std::endl;
        rkisp_close_device(camera.ctx);
        return NULL;
    }
    rkencode_init(&camera.encoder);
    camera.got_sps = false;
    camera.frame_encoded = true;
    camera.is_open = true;
    return &camera;
}

int rkcamera_get_frame(void* dev, void* frame_buf, int wait_ms, bool *is_key_frame)
{
    rkcamera* camera = (rkcamera*)dev;
    if(!dev || !camera->is_open)
    {
        return 0;
    }
    std::lock_guard<std::mutex> lock(camera->mutex);
    printf("Getting frame %d %d\n", camera->got_sps, camera->frame_encoded);
    if(!camera->got_sps)
    {
        //printf("Getting frame sps\n");
        camera->got_sps = true;
        size_t len;
        rkencode_get_sps(&camera->encoder, frame_buf, &len);
        return len;
    }
    if(camera->frame_encoded)
    {
        //printf("Getting frame encoded\n");
        camera->frame = rkisp_get_frame(camera->ctx, wait_ms);
        if(!camera->frame)
        {
            return 0;
        }
        camera->frame_encoded = 0;
    }
    if (camera->frame) {
        //printf("Captured frame, sequence: %d\n", camera->frame->sequence);
        MppBuffer mpp_buf;
        MppBufferInfo info;
        memset(&info, 0, sizeof(MppBufferInfo));
        info.type = MPP_BUFFER_TYPE_EXT_DMA;
        info.fd =  camera->frame->fd;
        info.size = camera->frame->size & 0x07ffffff;
        info.index = (camera->frame->size & 0xf8000000) >> 27;
        mpp_buffer_import(&mpp_buf, &info);
        rkencode_put_frame(&camera->encoder, mpp_buf);
        size_t len;
        int eoi;
        rkencode_get_data(&camera->encoder, frame_buf, &len, &eoi, is_key_frame);
        //printf("Encoded frame size: %zu\n", len);
        camera->frame_encoded = eoi;
        if(eoi)
        {
            rkencode_packet_release(&camera->encoder);
            rkisp_put_frame(camera->ctx, camera->frame);
            camera->frame = NULL;
        }
        return len;
    }
    return 0;
}

int rkcamera_close(void* dev)
{
    rkcamera* camera = (rkcamera*)dev;
    std::lock_guard<std::mutex> lock(camera->mutex);
    rkisp_stop_capture(camera->ctx);
    rkisp_close_device(camera->ctx);
    rkencode_deinit(&camera->encoder);
    camera->is_open = false;
    return 0;
}


/**
 * @brief 读取 H264 帧并判断是否为 I 帧
 * 
 * @param fp      H264 文件流（以二进制模式打开）
 * @param buf     存储读取帧数据的缓冲区
 * @param bufSize 缓冲区大小
 * @param frameLen 输出读取到的帧长度
 * @param isIFrame 输出该帧是否为 I 帧，1 表示 I 帧，0 表示非 I 帧
 * @return 0 成功，非 0 失败
 */
int read_h264_frame(FILE *fp, unsigned char *buf, int bufSize, int *frameLen, int *isIFrame)
{
    if (fp == NULL || buf == NULL || frameLen == NULL || isIFrame == NULL) {
        return -1;
    }
    int i = 0;
    int state = 0; // 用来判断是否找到第一个起始码
    int c;
    *isIFrame = 0; // 默认非 I 帧

    // 搜索第一个起始码 (0x00 0x00 0x00 0x01)
    while ((c = fgetc(fp)) != EOF) {
        if (i >= bufSize) {
            return -2;
        }
        buf[i++] = (unsigned char)c;
        if (i >= 4 &&
            buf[i - 4] == 0x00 &&
            buf[i - 3] == 0x00 &&
            buf[i - 2] == 0x00 &&
            buf[i - 1] == 0x01)
        {
            state = 1;
            break;
        }
    }
    if (!state) {
        // 文件读取结束，重置文件指针到文件开始处
        rewind(fp);
        return -3;
    }
    
    // 继续读取数据直到下一个起始码 (或 EOF) 被检测到
    while ((c = fgetc(fp)) != EOF) {
        if (i >= bufSize) {
            return -2;
        }
        buf[i++] = (unsigned char)c;
        
        // 检查是否有新起始码
        if (i >= 4 &&
            buf[i - 4] == 0x00 &&
            buf[i - 3] == 0x00 &&
            buf[i - 2] == 0x00 &&
            buf[i - 1] == 0x01)
        {
            // 新起始码属于下一帧，把文件指针回退 4 字节
            if (fseek(fp, -4, SEEK_CUR) != 0) {
                return -4;
            }
            i -= 4; // 不包含新起始码
            break;
        }
    }
    
    // 若文件读取结束，将文件指针设置到开始
    if (feof(fp)) {
        rewind(fp);
    }
    
    *frameLen = i;
    
    // 判断是否为 I 帧
    // H264 NAL 单元的类型存储在起始码后第一个字节的低 5 位中，值为 5 通常表示 IDR 帧（I 帧）
    if (*frameLen > 4) {
        unsigned char nal_type = buf[4] & 0x1F;
        if (nal_type == 5) {
            *isIFrame = 1;
        }
    }
    
    return 0;
}

#if 0
volatile std::sig_atomic_t stop = 0;
void signalHandler(int)
{
    stop = 1;
}
int main() {
    std::signal(SIGINT, signalHandler);
    rkencoder encoder;
    rkencode_init(&encoder);

    FILE *fp = fopen("output.h264", "wb");

    // Open device. Change device path if necessary.
    const char* dev_path = "/dev/video0";
    const struct rkisp_api_ctx* ctx = rkisp_open_device(dev_path, 0);
    if (!ctx) {
        std::cerr << "Failed to open device at " << dev_path << std::endl;
        return -1;
    }

    // Set camera format to 1920x1080 with NV12 format.
    if (rkisp_set_fmt(ctx, 1280, 720, V4L2_PIX_FMT_NV12) != 0) {
        std::cerr << "Failed to set format" << std::endl;
        rkisp_close_device(ctx);
        return -1;
    }

    // Request buffers. Using 4 buffers as an example.
    if (rkisp_set_buf(ctx, 4, nullptr, 0) != 0) {
        std::cerr << "Failed to set buffers" << std::endl;
        rkisp_close_device(ctx);
        return -1;
    }

    // Start the capture stream.
    if (rkisp_start_capture(ctx) != 0) {
        std::cerr << "Failed to start capture" << std::endl;
        rkisp_close_device(ctx);
        return -1;
    }

    std::cout << "Capturing frames... Press Ctrl+C to exit." << std::endl;
    bool first_frame = true;
    while (!stop) {
        // Wait for a frame with a timeout of 1000ms.
        const struct rkisp_api_buf* frame = rkisp_get_frame(ctx, 1000);
        if (frame) {
            std::cout << "Captured frame, sequence: " << frame->sequence << std::endl;
            // Process the frame here.
            MppBuffer mpp_buf;
            MppBufferInfo info;
            memset(&info, 0, sizeof(MppBufferInfo));
            info.type = MPP_BUFFER_TYPE_EXT_DMA;
            info.fd =  frame->fd;
            info.size = frame->size & 0x07ffffff;
            info.index = (frame->size & 0xf8000000) >> 27;
            mpp_buffer_import(&mpp_buf, &info);

            rkencode_frame(&encoder, mpp_buf, [fp](void* ptr, size_t len){
                fwrite(ptr, 1, len, fp);
                printf("Encoded frame size: %zu\n", len);
            }, first_frame);
            if(first_frame){
                first_frame = false;
            }
            rkisp_put_frame(ctx, frame);
        } else {
            std::cerr << "Failed to capture frame or timeout" << std::endl;
        }
    }

    rkisp_stop_capture(ctx);
    rkisp_close_device(ctx);
    rkencode_deinit(&encoder);
    fclose(fp);
    std::cout << "Capture stopped. Exiting." << std::endl;
    return 0;
}
#endif