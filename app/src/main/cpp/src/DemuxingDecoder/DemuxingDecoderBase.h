//
// Created by zou on 2022/9/14.
//

#ifndef SKYMEDIAPLAYER_DEMUXINGDECODERBASE_H
#define SKYMEDIAPLAYER_DEMUXINGDECODERBASE_H

#include "utils/PlayerFormats.h"
#include "Render/Video/RenderBase.h"
#include "Render/Audio/AudioPlayer.h"
#include "Decoder/VideoDecoderObserver.h"
#include "Decoder/AudioDecoderObserver.h"

class DemuxingDecoderBase {
public:
    virtual bool Init(const char* url, const std::shared_ptr<RenderBase>& render,
                      const std::shared_ptr<AudioPlayer>& audio_player,
                      const std::shared_ptr<VideoDecoderObserver>& video_decoder_observer,
                      const std::shared_ptr<AudioDecoderObserver>& audio_decoder_observer) = 0;
    virtual bool Start() = 0;
    virtual bool Stop() = 0;
    virtual bool Destroy() = 0;

    int GetVideoWidth(){return m_frame_params.width;}
    int GetVideoHeight(){return m_frame_params.height;}
    virtual ~DemuxingDecoderBase(){};

protected:
    std::shared_ptr<RenderBase> m_render;
    std::shared_ptr<AudioPlayer> m_audio_player;
    std::shared_ptr<VideoDecoderObserver> m_video_decoder_observer;
    std::shared_ptr<AudioDecoderObserver> m_audio_decoder_observer;

    FrameParams m_frame_params;
    PcmParams m_pcm_params;

};


#endif //SKYMEDIAPLAYER_DEMUXINGDECODERBASE_H
