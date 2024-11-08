//
// Created by zou on 2024/9/2.
//

#ifndef SKYMEDIAPLAYER_SKYPACKETQUEUE_H
#define SKYMEDIAPLAYER_SKYPACKETQUEUE_H

#include <mutex>
#include <queue>
#include <condition_variable>
extern "C" {
#include "libavcodec/avcodec.h"
}

class SkyPacketQueue {
public:
    SkyPacketQueue() = default;
    ~SkyPacketQueue();


    void Start();
    void StopAndRelease();
    bool QueuePacket(AVPacket* packet);
    bool DequeuePacket(AVPacket* packet, int* serial);

    void Clear();
    void FlushPacket();
    bool IsFlushPacket(AVPacket* packet);
    inline int GetSerial(){return m_serial;}

private:
    struct SkyPacket{
        int serial = 0;
        AVPacket packet;
    };

    std::mutex m_mutex;
    std::condition_variable m_condition_pop;
    std::queue<SkyPacket> m_queue;
    bool m_running = false;

    int m_serial = 0;
    AVPacket m_flush_pkt;
};


#endif //SKYMEDIAPLAYER_SKYPACKETQUEUE_H
