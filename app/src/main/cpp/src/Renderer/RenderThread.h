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
#include "SkyClock.h"
class RenderThread{
public:
    void Init(  ANativeWindow* native_window,
            const std::shared_ptr<SkyFrameQueue>& audioFrameQueue,
            const std::shared_ptr<SkyFrameQueue>& videoFrameQueue,
            const std::shared_ptr<SkyFrameQueue>& subtitleFrameQueue);

    void SetTimebase(const AVMediaType& mediaType, const AVRational& timebase);

    bool Start();
    bool Pause();
    bool Stop();
    bool Release();

    int GetCurrentTime();

private:
    void Loop();

    std::shared_ptr<SkyFrameQueue> m_audio_queue;
    std::shared_ptr<SkyFrameQueue> m_video_queue;
    std::shared_ptr<SkyFrameQueue> m_subtitle_queue;

    AVRational m_audio_timebase;
    AVRational m_video_timebase;
    AVRational m_subtitle_timebase;

    std::unique_ptr<AudioPlayer> m_audio_player;
    std::unique_ptr<VideoRenderer> m_video_renderer;
    std::thread m_thread;



    std::shared_ptr<SkyClock> m_audio_clock;
    std::shared_ptr<SkyClock> m_video_clock;
    bool m_running = false;

    int m_current_position = 0;

    static constexpr double REFRESH_RATE = 0.01;  //10ms
    static constexpr double MAX_DURATION = 10;  //10s


    static constexpr double SYNC_THRESHOLD_MIN = 0.04;
    static constexpr double SYNC_THRESHOLD_MAX = 0.1;
    static constexpr double SYNC_FRAMEDUP_THRESHOLD = 0.15; //单帧间隔阈值15ms






};































































































#endif //SKYMEDIAPLAYER_RENDERTHREAD_H
