//
// Created by zou on 2024/9/2.
//

#include "DemuxThread.h"
#include "AndroidLog.h"
#include "SkyMediaPlayer.h"
bool DemuxThread::Init(SkyMediaPlayer* mediaPlayer, const std::string& url,
                       const std::shared_ptr<SkyFrameQueue>& audioFrameQueue,
                       const std::shared_ptr<SkyFrameQueue>& videoFrameQueue,
                       const std::shared_ptr<SkyFrameQueue>& subtitleFrameQueue) {
    m_avformat_context = avformat_alloc_context();

    avformat_network_init();

    int ret = 0;
    if((ret = avformat_open_input(&m_avformat_context, url.c_str(), nullptr, nullptr))){
        ERROR("avformat open input failed, url=%s, (%s)", url.c_str(), av_err2str(ret));
        return false;
    }

    if(avformat_find_stream_info(m_avformat_context, nullptr)){
        ERROR("avformat find stream info failed, url=%s", url.c_str());
        return false;
    }

    AVStream* stream = nullptr;
    ret = av_find_best_stream(m_avformat_context, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if(ret < 0){
        WARNING("can not find %s stream in input file", av_get_media_type_string(AVMEDIA_TYPE_AUDIO));
    }else{
        m_audio_stream_index = ret;
        stream = m_avformat_context->streams[ret];
        m_audio_decoder = std::unique_ptr<SkyDecoder>(MediaDecoderFactory::CreateMediaDecoder());
        m_audio_packet_queue = std::make_shared<SkyPacketQueue>();
        m_audio_decoder->Init(mediaPlayer, stream->codecpar, m_audio_packet_queue, audioFrameQueue);
    }

    ret = av_find_best_stream(m_avformat_context, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if(ret < 0){
        WARNING("can not find %s stream in input file", av_get_media_type_string(AVMEDIA_TYPE_VIDEO));
    }else{
        m_video_stream_index = ret;
        stream = m_avformat_context->streams[ret];

        mediaPlayer->OnSizeChanged(stream->codecpar->width, stream->codecpar->height);


        m_video_decoder = std::unique_ptr<SkyDecoder>(MediaDecoderFactory::CreateMediaDecoder());
        m_video_packet_queue = std::make_shared<SkyPacketQueue>();
        m_video_decoder->Init(mediaPlayer, stream->codecpar, m_video_packet_queue, videoFrameQueue);
    }

    ValidMediaSteams();
    INFO("INIT success");
    return true;
}

bool DemuxThread::Start() {
    INFO("Start enter");
    ValidMediaSteams();

    m_running = true;
    if(m_audio_decoder)
        m_audio_decoder->Start();

    if(m_video_decoder)
        m_video_decoder->Start();

    m_thread = std::thread(&DemuxThread::Loop, this);
    INFO("Start leave");
    return true;
}

bool DemuxThread::Pause() {
    ValidMediaSteams();
    return false;
}

bool DemuxThread::StopAndRelease() {
    ValidMediaSteams();
    m_running = false;
    if(m_audio_decoder)
        m_audio_decoder->Release();

    if(m_video_decoder)
        m_video_decoder->Release();
    return false;
}

void DemuxThread::Loop() {
    ValidMediaSteams();
    m_packet = av_packet_alloc();
    if(!m_packet){
        ERROR("can not allocate packet");
        exit(-1);
    }
    int ret = 0;
    while (m_running){
        ret = av_read_frame(m_avformat_context, m_packet);
        if(ret < 0){
            INFO("decode done");
            break;
        }

        if(m_packet->stream_index == m_audio_stream_index){
            m_audio_packet_queue->QueuePacket(m_packet);
        }else if(m_packet->stream_index == m_video_stream_index){
            m_video_packet_queue->QueuePacket(m_packet);
        }

        av_packet_unref(m_packet);
    }
    av_packet_free(&m_packet);
    m_packet = nullptr;
}

void DemuxThread::ValidMediaSteams() {
    if(!m_audio_decoder && !m_video_decoder){
        ERROR("can not find video or audio stream in input file");
        exit(-1);
    }
}
