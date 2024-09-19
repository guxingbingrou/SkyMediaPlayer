//
// Created by zou on 2024/9/2.
//

#ifndef SKYMEDIAPLAYER_SKYMEDIAPLAYER_H
#define SKYMEDIAPLAYER_SKYMEDIAPLAYER_H

#include <memory>
#include <android/native_window_jni.h>
#include "Decoder/DemuxThread.h"
#include "Renderer/RenderThread.h"
#include "SkyFrameQueue.h"

class SkyMediaPlayer {
public:
    bool Init();
    bool SetSource(const char* url);
    bool SetSurface( JNIEnv* env, jobject surface);
    bool Start();
    bool Pause();
    bool Stop();
    bool Release();

    void OnSizeChanged(int width, int height);
    void OnAudioParamsChanged(int sampleRate, int channels);

private:
    std::unique_ptr<DemuxThread> m_demuxer;
    std::unique_ptr<RenderThread> m_renderer;
    ANativeWindow* m_native_window = nullptr;
    std::shared_ptr<SkyFrameQueue> m_audio_frame_queue;
    std::shared_ptr<SkyFrameQueue> m_video_frame_queue;
    std::shared_ptr<SkyFrameQueue> m_subtitle_queue;

    std::string m_url;
    std::atomic<bool> m_init;
};


#endif //SKYMEDIAPLAYER_SKYMEDIAPLAYER_H
