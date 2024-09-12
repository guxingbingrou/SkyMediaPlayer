//
// Created by zou on 2024/9/3.
//

#ifndef SKYMEDIAPLAYER_FFMEDIADECODER_H
#define SKYMEDIAPLAYER_FFMEDIADECODER_H

#include <thread>
#include "SkyDecoder.h"

class FFMediaDecoder : public SkyDecoder{
public:
    virtual bool Init(AVCodecParameters *codecpar,
                      const std::shared_ptr<SkyPacketQueue>& packetQueue,
                      const std::shared_ptr<SkyFrameQueue>& frameQueue) override;
    virtual bool Start() override;
    virtual bool Stop() override;
    virtual bool Release() override;

private:
    void Decode();
    AVCodecContext* m_codec_context = nullptr;


    std::thread m_thread;
    bool m_running = false;

    int m_video_width = 1080;
    int m_video_height = 1920;

};


#endif //SKYMEDIAPLAYER_FFMEDIADECODER_H
