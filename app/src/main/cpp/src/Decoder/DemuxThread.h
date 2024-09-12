//
// Created by zou on 2024/9/2.
//

#ifndef SKYMEDIAPLAYER_DEMUXTHREAD_H
#define SKYMEDIAPLAYER_DEMUXTHREAD_H

#include <memory>
#include <string>
#include <thread>

#include "SkyPacketQueue.h"
#include "MediaDecoderFactory.h"
#include "SkyDecoder.h"
extern "C"{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"

};

class DemuxThread {
public:
    DemuxThread() = default;
    ~DemuxThread() = default;

    bool Init(const std::string& url,
              const std::shared_ptr<SkyFrameQueue>& audioFrameQueue,
              const std::shared_ptr<SkyFrameQueue>& videoFrameQueue,
              const std::shared_ptr<SkyFrameQueue>& subtitleFrameQueue);
    bool Start();
    bool Pause();
    bool StopAndRelease();

private:
    void Loop();
    void ValidMediaSteams();


    std::unique_ptr<SkyDecoder> m_audio_decoder;
    std::unique_ptr<SkyDecoder> m_video_decoder;
    std::shared_ptr<SkyPacketQueue> m_audio_packet_queue;
    std::shared_ptr<SkyPacketQueue> m_video_packet_queue;
    int m_video_stream_index = -1;
    int m_audio_stream_index = -1;
    AVFormatContext* m_avformat_context = nullptr;
    AVPacket* m_packet = nullptr;

    std::thread m_thread;
    bool m_running = false;


};


#endif //SKYMEDIAPLAYER_DEMUXTHREAD_H
