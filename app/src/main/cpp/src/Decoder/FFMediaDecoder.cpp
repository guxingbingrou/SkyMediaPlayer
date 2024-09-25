//
// Created by zou on 2024/9/3.
//

#include "FFMediaDecoder.h"
#include "AndroidLog.h"
#include "SkyMediaPlayer.h"


bool FFMediaDecoder::Init(SkyMediaPlayer* mediaPlayer, AVStream* stream,
                          const std::shared_ptr<SkyPacketQueue>& packetQueue,
                          const std::shared_ptr<SkyFrameQueue>& frameQueue) {
    m_media_player = mediaPlayer;
    m_packet_queue = packetQueue;
    m_frame_queue = frameQueue;


    const AVCodec* codec = avcodec_find_decoder(stream->codecpar->codec_id);
    if(!codec){
        ERROR("failed to find %s codec", avcodec_get_name(stream->codecpar->codec_id));
        return false;
    }

    m_codec_context = avcodec_alloc_context3(codec);
    if(!m_codec_context){
        ERROR("failed to allocate %s codec context", avcodec_get_name(stream->codecpar->codec_id));
        return false;
    }

    if( avcodec_parameters_to_context(m_codec_context, stream->codecpar) < 0){
        ERROR("failed to copy %s codec parameters to decoder context", avcodec_get_name(stream->codecpar->codec_id));
        return false;
    }

    if( avcodec_open2(m_codec_context, codec, nullptr) < 0){
        ERROR("failed to open %s codec", avcodec_get_name(stream->codecpar->codec_id));
        return false;
    }


    return true;
}

bool FFMediaDecoder::Start() {
    m_packet_queue->Start();
    m_frame_queue->Start();
    m_running = true;
    m_thread = std::thread(&FFMediaDecoder::Decode, this);
    return true;
}

bool FFMediaDecoder::Stop() {
    m_running = false;
    m_packet_queue->Pause();
    m_frame_queue->Pause();

    if(m_thread.joinable()){
        m_thread.join();
    }
    return true;
}

bool FFMediaDecoder::Release() {
    m_running = false;
    m_packet_queue->StopAndRelease();
    m_frame_queue->Release();
    if(m_thread.joinable()){
        m_thread.join();
    }

    if(m_codec_context){
        avcodec_free_context(&m_codec_context);
        m_codec_context = nullptr;
    }




    return true;
}

void FFMediaDecoder::Decode() {
    AVFrame* avFrame = av_frame_alloc();
    if(!avFrame){
        ERROR("can not allocate frame");
        exit(-1);
    }

    AVPacket* packet = av_packet_alloc();
    if(!packet){
        ERROR("can not allocate packet");
        exit(-1);
    }

    int ret = 0;

    while (m_running){
//        INFO("DequeuePacket");
        if(!m_packet_queue->DequeuePacket(packet)){
            if(!m_running){
                INFO("Decode Stopped");
                break;
            }else{
                ERROR("DequeuePacket failed !!!");
                continue;
            }
        }
        ret = avcodec_send_packet(m_codec_context, packet);
        if(ret < 0){
            ERROR("failed to submitting a packet for decoding (%s)", av_err2str(ret));
            return ;
        }

        while (ret >=0){

            ret = avcodec_receive_frame(m_codec_context, avFrame);
            if(ret < 0) {
                if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN)) {
                    break;
                }

                ERROR("error during decoding (%s)", av_err2str(ret));
                return;
            }

            if(m_codec_context->codec->type == AVMEDIA_TYPE_VIDEO){
//                INFO("GetWriteableFrame");
                SkyFrame* skyFrame = m_frame_queue->GetWriteableFrame();
                if(skyFrame == nullptr || skyFrame->frame == nullptr){
                    ERROR("GetWriteableFrame failed");
                    break;
                }

                if(m_video_width != avFrame->width || m_video_height != avFrame->height){

                    m_video_width = avFrame->width;
                    m_video_height = avFrame->height;
                    m_media_player->OnSizeChanged(m_video_width, m_video_height);

                }

//                INFO("GetWriteableFrame success");
                av_frame_ref(skyFrame->frame, avFrame);
                m_frame_queue->FlushWriteableFrame();
            }else if(m_codec_context->codec->type == AVMEDIA_TYPE_AUDIO){
//                INFO("GetWriteableFrame");
                SkyFrame* skyFrame = m_frame_queue->GetWriteableFrame();
                if(skyFrame == nullptr || skyFrame->frame == nullptr){
                    ERROR("GetWriteableFrame failed");
                    break;
                }
//                INFO("GetWriteableFrame success");
                av_frame_ref(skyFrame->frame, avFrame);
                m_frame_queue->FlushWriteableFrame();
            }

            av_frame_unref(avFrame);

        }


//        INFO("av_packet_unref");
        av_packet_unref(packet);

    }



    if(packet){
        av_packet_free(&packet);
        packet = nullptr;
    }

    if(avFrame){
        av_frame_free(&avFrame);
        avFrame = nullptr;
    }

}
