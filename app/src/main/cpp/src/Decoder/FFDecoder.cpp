//
// Created by zou on 2022/9/14.
//
#include "utils/AndroidLog.h"
#include "FFDecoder.h"

FFDecoder::FFDecoder() {

}

FFDecoder::~FFDecoder() {

}

bool FFDecoder::InitDecoder() {
    const AVCodec* codec = avcodec_find_encoder(m_codec_id);
    if(!codec){
        ERROR("can not find codec(%d)", m_codec_id);
        return false;
    }

    m_codec_context = avcodec_alloc_context3(codec);
    if(!m_codec_context){
        ERROR("can not allocate codec context");
        return false;
    }

    m_parser_context = av_parser_init(m_codec_id);
    if(!m_parser_context){
        ERROR("can not init parser context");
        return false;
    }

    if(avcodec_open2(m_codec_context, codec, nullptr)){
        ERROR("can not open codec");
        return false;
    }

    m_frame = av_frame_alloc();
    if(!m_frame){
        ERROR("can not allocate frame");
        return false;
    }

    m_packet = av_packet_alloc();
    if(!m_packet){
        ERROR("can not allocate packet");
        return false;
    }
}

bool FFDecoder::DestroyDecoder() {
    if(m_parser_context){
        av_parser_close(m_parser_context);
        m_parser_context = nullptr;
    }

    if(m_codec_context){
        avcodec_free_context(&m_codec_context);
        m_codec_context = nullptr;
    }

    if(m_frame){
        av_frame_free(&m_frame);
        m_frame = nullptr;
    }

    if(m_packet){
        av_packet_free(&m_packet);
        m_packet = nullptr;
    }
}

bool FFDecoder::OnPacketAvailable(PacketParams *packetParams) {
    size_t data_size = packetParams->size;
    uint8_t* data = packetParams->data;
    int ret = 0;
    while(data_size > 0){
        ret = av_parser_parse2(m_parser_context, m_codec_context, &m_packet->data, &m_packet->size,
                data, data_size, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);

        if(ret < 0){
            ERROR("error while parsing");
            return false;
        }

        data += ret;
        data_size -= ret;

        if(m_packet->size){
            DecodeOnePacket(m_codec_context, m_packet);
        }
    }

    return true;
}

bool FFDecoder::DecodeOnePacket(AVCodecContext *codec_context, AVPacket *packet) {
    int ret;
    ret = avcodec_send_packet(codec_context, packet);
    if(ret < 0){
        ERROR("error sending packet to decoder");
        return false;
    }

    while(ret > 0){
        ret = avcodec_receive_frame(codec_context, m_frame);
        if(ret == AVERROR(EAGAIN) || ret == AVERROR_EOF){
            return true;
        }else if(ret < 0){
            ERROR("decoder error");
            return false;
        }

        //TODO consuming frame;
    }
}



