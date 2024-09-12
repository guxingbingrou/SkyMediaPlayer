//
// Created by zou on 2024/9/3.
//

#ifndef SKYMEDIAPLAYER_SKYDECODER_H
#define SKYMEDIAPLAYER_SKYDECODER_H
extern "C"{
#include "libavcodec/avcodec.h"
};
#include "SkyPacketQueue.h"
#include "SkyFrameQueue.h"
class SkyDecoder{
public:
    virtual bool Init(AVCodecParameters *codecpar,
                      const std::shared_ptr<SkyPacketQueue>& packetQueue,
                      const std::shared_ptr<SkyFrameQueue>& frameQueue) = 0;
    virtual bool Start() = 0;
    virtual bool Stop() = 0;
    virtual bool Release() = 0;

protected:
    std::shared_ptr<SkyPacketQueue> m_packet_queue;
    std::shared_ptr<SkyFrameQueue> m_frame_queue;
};

#endif //SKYMEDIAPLAYER_SKYDECODER_H
