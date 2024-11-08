//
// Created by zou on 2024/9/3.
//

#ifndef SKYMEDIAPLAYER_FFMEDIADECODER_H
#define SKYMEDIAPLAYER_FFMEDIADECODER_H

#include <thread>
#include "SkyDecoder.h"



class FFMediaDecoder : public SkyDecoder{
public:
    virtual bool Init(SkyMediaPlayer* mediaPlayer, AVStream* stream,
                      const std::shared_ptr<SkyPacketQueue>& packetQueue,
                      const std::shared_ptr<SkyFrameQueue>& frameQueue) override;
    virtual bool Start() override;
    virtual bool Stop() override;
    virtual bool Release() override;

private:
    void Decode();
    void DecodeOnPacket(AVPacket* packet, AVFrame* avFrame);
    SkyMediaPlayer* m_media_player = nullptr;
    AVCodecContext* m_codec_context = nullptr;



    std::thread m_thread;
    bool m_running = false;

    int m_video_width = 0;
    int m_video_height = 0;

};


#endif //SKYMEDIAPLAYER_FFMEDIADECODER_H
