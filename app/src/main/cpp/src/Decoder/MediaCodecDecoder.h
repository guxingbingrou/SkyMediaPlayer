//
// Created by zou on 2024/9/3.
//

#ifndef SKYMEDIAPLAYER_MEDIACODECDECODER_H
#define SKYMEDIAPLAYER_MEDIACODECDECODER_H

#include "SkyDecoder.h"
class MediaCodecDecoder : public SkyDecoder{
public:
    virtual bool Init(AVCodecParameters *codecpar,
                      const std::shared_ptr<SkyPacketQueue>& packetQueue,
                      const std::shared_ptr<SkyFrameQueue>& frameQueue) override;
    virtual bool Start() override;
    virtual bool Stop() override;
    virtual bool Release() override;
};


#endif //SKYMEDIAPLAYER_MEDIACODECDECODER_H
