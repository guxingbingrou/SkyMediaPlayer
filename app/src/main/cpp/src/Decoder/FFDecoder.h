//
// Created by zou on 2022/9/14.
//

#ifndef SKYMEDIAPLAYER_FFDECODER_H
#define SKYMEDIAPLAYER_FFDECODER_H

#include <thread>

#include "DecoderBase.h"

extern "C"{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/frame.h"
#include "libavutil/imgutils.h"
};


class FFDecoder : public DecoderBase{
public:
    FFDecoder();
    virtual ~FFDecoder();
    virtual bool InitDecoder() override ;

    virtual bool DestroyDecoder() override;

    virtual bool OnPacketAvailable(PacketParams* packetParams) override ;

protected:
    virtual bool DecodeOnePacket(AVCodecContext* codec_context, AVPacket* packet);

    enum AVCodecID m_codec_id = AV_CODEC_ID_NONE;
    AVCodecContext* m_codec_context = nullptr;

    AVFrame* m_frame = nullptr;
    AVPacket* m_packet = nullptr;

    AVCodecParserContext* m_parser_context = nullptr;

};


#endif //SKYMEDIAPLAYER_FFDECODER_H
