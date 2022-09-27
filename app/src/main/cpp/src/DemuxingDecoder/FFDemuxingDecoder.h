//
// Created by zou on 2022/9/14.
//

#ifndef SKYMEDIAPLAYER_FFDEMUXINGDECODER_H
#define SKYMEDIAPLAYER_FFDEMUXINGDECODER_H

#include <thread>

#include "DemuxingDecoderBase.h"

extern "C"{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/frame.h"
#include "libavutil/imgutils.h"
#include "libswresample/swresample.h"
#include "libavutil/opt.h"
#include "libavutil/audio_fifo.h"
};


class FFDemuxingDecoder : public DemuxingDecoderBase{
public:
    FFDemuxingDecoder();
    ~FFDemuxingDecoder();
    bool Init(const char* url, const std::shared_ptr<RenderBase>& render,
              const std::shared_ptr<AudioPlayer>& audio_player,
            const std::shared_ptr<VideoDecoderObserver>& video_decoder_observer,
            const std::shared_ptr<AudioDecoderObserver>& audio_decoder_observer) override ;
    bool Start() override;
    bool Stop() override;
    bool Destroy() override;
    int GetVideoWidth(){return m_frame_params.width;}
    int GetVideoHeight(){return m_frame_params.height;}


private:
    bool OpenCodecContext(int* stream_idx, AVCodecContext** dec_ctx, AVFormatContext* fmt_ctx, enum AVMediaType media_type);
    void DecodeLoop();
    bool DecodeOnePacket(AVCodecContext* codec_context, AVPacket* packet);

    ImageFormat AVPixelFormatToImageFormat(AVPixelFormat pixelFormat);

    AVFormatContext* m_format_context = nullptr;
    AVCodecContext* m_video_codec_context = nullptr;
    AVCodecContext* m_audio_codec_context = nullptr;

    SwrContext* m_swr_context = nullptr;
    uint8_t * m_audio_data = nullptr;

    AVStream* m_video_stream = nullptr;
    AVStream* m_audio_stream = nullptr;
    int m_video_stream_index = -1;
    int m_audio_stream_index = -1;

    AVFrame* m_frame = nullptr;
    AVPacket* m_packet = nullptr;

    char* m_Url = nullptr;

    std::thread m_decode_thread;
    bool m_is_running = false;



};


#endif //SKYMEDIAPLAYER_FFDECODER_H
