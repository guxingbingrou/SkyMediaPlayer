//
// Created by zou on 2022/9/14.
//

#ifndef SKYMEDIAPLAYER_DEMUXINGDECODERBASE_H
#define SKYMEDIAPLAYER_DEMUXINGDECODERBASE_H

#include "utils/PlayerFormats.h"
#include "Render/Video/RenderBase.h"
#include "Decoder/VideoDecoderObserver.h"

class DemuxingDecoderBase {
public:
    virtual bool Init(const char* url, const std::shared_ptr<RenderBase>& render,
                      const std::shared_ptr<VideoDecoderObserver>& video_decoder_observer) = 0;
    virtual bool Start() = 0;
    virtual bool Stop() = 0;
    virtual bool Destroy() = 0;

    virtual ~DemuxingDecoderBase(){};

protected:
    std::shared_ptr<RenderBase> m_render;
    std::shared_ptr<VideoDecoderObserver> m_video_decoder_observer;

};


#endif //SKYMEDIAPLAYER_DEMUXINGDECODERBASE_H
