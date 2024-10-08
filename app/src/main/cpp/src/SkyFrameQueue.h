//
// Created by zou on 2024/9/4.
//

#ifndef SKYMEDIAPLAYER_SKYFRAMEQUEUE_H
#define SKYMEDIAPLAYER_SKYFRAMEQUEUE_H
#include <mutex>
#include <condition_variable>
extern "C" {
#include "libavcodec/avcodec.h"
}

#define MAX_FRAMES 16

enum FrameType{
    FrameNone,
    FrameVideo,
    FrameAudio,
    FrameSubtitle,
    FrameMax
};


struct SkyFrame {
//    FrameType frameType;
    AVFrame* frame = nullptr;
};

class SkyFrameQueue{
public:
    void Init();
    void Start();
    void Pause();
    void Release();
    bool Empty(); //可读的大小为空

    SkyFrame* GetWriteableFrame();
    void FlushWriteableFrame();

    const SkyFrame* GetReadableFrame();
    void FlushReadableFrame();
private:
    SkyFrame m_frame_queue[MAX_FRAMES];
    std::mutex m_mutex;
    std::condition_variable m_condition_write;
    std::condition_variable m_condition_read;
    uint32_t m_read_pos = 0;
    uint32_t m_write_pos = 0;
    uint32_t m_live_size = MAX_FRAMES;  //可写的大小
    bool m_running = false;

};



#endif //SKYMEDIAPLAYER_SKYFRAMEQUEUE_H
