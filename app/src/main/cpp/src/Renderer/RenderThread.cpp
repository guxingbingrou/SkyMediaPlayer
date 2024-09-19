//
// Created by zou on 2024/8/29.
//

#include "RenderThread.h"
#include "AndroidLog.h"

void RenderThread::Init(ANativeWindow *native_window,
                        const std::shared_ptr<SkyFrameQueue> &audioFrameQueue,
                        const std::shared_ptr<SkyFrameQueue> &videoFrameQueue,
                        const std::shared_ptr<SkyFrameQueue> &subtitleFrameQueue) {
    m_audio_queue = audioFrameQueue;
    m_video_queue = videoFrameQueue;
    m_subtitle_queue = subtitleFrameQueue;

    m_video_renderer = std::make_unique<VideoRenderer>();
    m_video_renderer->SetWindow(native_window);

    m_audio_player = std::make_unique<AudioPlayer>();
    m_audio_player->Init(audioFrameQueue);


}

bool RenderThread::Start() {
    m_audio_player->Start();
    m_running = true;
    m_thread = std::thread(&RenderThread::Loop, this);
    return false;
}

bool RenderThread::Pause() {
    return false;
}

bool RenderThread::Stop() {
    return false;
}

bool RenderThread::Release() {
    return false;
}

void RenderThread::Loop() {
    if(!m_video_renderer->Init()){
        ERROR("video renderer init failed");
        return;
    }
    INFO("video render init success");
    int failed_count = 5;



    while (m_running){
        const SkyFrame* skyFrame = m_video_queue->GetReadableFrame();
        if(skyFrame == nullptr || skyFrame->frame == nullptr){
            INFO("GetReadableFrame failed");
            if(--failed_count)
                continue;
            else{
                ERROR("try get readable frame enough time");
                break;
            }
        }

//        INFO("GetReadableFrame success");

        m_video_renderer->RenderPicture(skyFrame);
//        INFO("RenderPicture success");
        m_video_queue->FlushReadableFrame();
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }

}
