//
// Created by zou on 2024/9/2.
//

#include "SkyMediaPlayer.h"
#include "AndroidLog.h"

bool SkyMediaPlayer::SetSource(const char *url) {
    m_url = std::string(url);

    if(!m_init.load() && m_native_window && m_demuxer && m_renderer){
        m_demuxer->Init(this, m_url, m_audio_frame_queue, m_video_frame_queue, m_subtitle_queue);
        m_renderer->Init(m_native_window, m_audio_frame_queue, m_video_frame_queue, m_subtitle_queue);
        m_init.store(true);
        INFO("Init");
        ANativeWindow_release(m_native_window);
    }
    return true;
}

bool SkyMediaPlayer::Init() {
    m_demuxer = std::make_unique<DemuxThread>();
    m_renderer = std::make_unique<RenderThread>();

    m_audio_frame_queue = std::make_shared<SkyFrameQueue>();
    m_video_frame_queue = std::make_shared<SkyFrameQueue>();
    m_subtitle_queue = std::make_shared<SkyFrameQueue>();

    m_audio_frame_queue->Init();
    m_video_frame_queue->Init();
    m_subtitle_queue->Init();

    if(!m_init.load() && m_native_window && !m_url.empty()){
        m_demuxer->Init(this, m_url, m_audio_frame_queue, m_video_frame_queue, m_subtitle_queue);
        m_renderer->Init(m_native_window, m_audio_frame_queue, m_video_frame_queue, m_subtitle_queue);
        m_init.store(true);
        INFO("Init");
        ANativeWindow_release(m_native_window);
    }
    return true;
}

bool SkyMediaPlayer::SetSurface( JNIEnv* env, jobject surface) {
    INFO("SetSurface: 0x%x", surface);
    if(surface == nullptr){
        if(m_native_window){
            ANativeWindow_release(m_native_window);
            m_native_window = nullptr;
        }
        return true;
    }


    m_native_window = ANativeWindow_fromSurface(env, surface);
    if(!m_native_window){
        ERROR("native window is null");
        return false;
    }

    if(!m_init.load() && !m_url.empty() && m_demuxer && m_renderer){
        m_demuxer->Init(this, m_url, m_audio_frame_queue, m_video_frame_queue, m_subtitle_queue);
        m_renderer->Init(m_native_window, m_audio_frame_queue, m_video_frame_queue, m_subtitle_queue);
        m_init.store(true);
        INFO("Init");
        ANativeWindow_release(m_native_window);
    }
    return true;
}

bool SkyMediaPlayer::Start() {
    INFO("Start");
    if(!m_init.load()){
        return false;
    }

    INFO("Start Enter");
    m_demuxer->Start();
    m_renderer->Start();
    return true;
}

bool SkyMediaPlayer::Pause() {
    return false;
}

bool SkyMediaPlayer::Stop() {
    return false;
}

bool SkyMediaPlayer::Release() {
    return false;
}

void SkyMediaPlayer::OnSizeChanged(int width, int height) {
    INFO("OnSizeChanged: %dx%d", width, height);
}

void SkyMediaPlayer::OnAudioParamsChanged(int sampleRate, int channels) {

}
