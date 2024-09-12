//
// Created by zou on 2024/8/29.
//

#ifndef SKYMEDIAPLAYER_RENDERTHREAD_H
#define SKYMEDIAPLAYER_RENDERTHREAD_H

#include <thread>
#include <queue>
#include <memory>
#include <android/native_window.h>
#include "SkyFrameQueue.h"
#include "VideoRenderer.h"
#include "AudioPlayer.h"
class RenderThread{
public:
    void Init(  ANativeWindow* native_window,
            const std::shared_ptr<SkyFrameQueue>& audioFrameQueue,
            const std::shared_ptr<SkyFrameQueue>& videoFrameQueue,
            const std::shared_ptr<SkyFrameQueue>& subtitleFrameQueue);

    bool Start();
    bool Pause();
    bool Stop();
    bool Release();

private:
    void Loop();

    std::shared_ptr<SkyFrameQueue> m_audio_queue;
    std::shared_ptr<SkyFrameQueue> m_video_queue;
    std::shared_ptr<SkyFrameQueue> m_subtitle_queue;

    std::unique_ptr<AudioPlayer> m_audio_player;
    std::unique_ptr<VideoRenderer> m_video_renderer;
    std::thread m_thread;
    bool m_running = false;





};































































































#endif //SKYMEDIAPLAYER_RENDERTHREAD_H
