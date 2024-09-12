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
    void Pause();
    void StopAndRelease();
    bool QueuePacket(AVPacket* packet);
    bool DequeuePacket(AVPacket* packet);

private:
    static constexpr int MAX_PACKETS = 20;
    std::mutex m_mutex;
    std::condition_variable m_condition_push;
    std::condition_variable m_condition_pop;
    std::queue<AVPacket> m_queue;
    bool m_running = false;
};


#endif //SKYMEDIAPLAYER_SKYPACKETQUEUE_H
