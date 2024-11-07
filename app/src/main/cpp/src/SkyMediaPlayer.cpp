//
// Created by zou on 2024/9/2.
//

#include "SkyMediaPlayer.h"
#include "AndroidLog.h"

SkyMediaPlayer::SkyMediaPlayer(){

}
bool SkyMediaPlayer::SetSource(const char *url) {
    m_url = std::string(url);

    m_demuxer->Init(this, m_url, m_audio_frame_queue, m_video_frame_queue, m_subtitle_queue);

    return true;
}

bool SkyMediaPlayer::Init() {
    m_demuxer = std::make_unique<DemuxThread>();
    m_renderer = std::make_unique<RenderThread>();
    m_renderer->SetTimebase(AVMEDIA_TYPE_AUDIO, m_audio_timebase);
    m_renderer->SetTimebase(AVMEDIA_TYPE_VIDEO, m_video_timebase);
    m_renderer->SetTimebase(AVMEDIA_TYPE_SUBTITLE, m_subtitle_timebase);


    m_audio_frame_queue = std::make_shared<SkyFrameQueue>();
    m_video_frame_queue = std::make_shared<SkyFrameQueue>();
    m_subtitle_queue = std::make_shared<SkyFrameQueue>();

    m_audio_frame_queue->Init();
    m_video_frame_queue->Init();
    m_subtitle_queue->Init();


    return true;
}

bool SkyMediaPlayer::SetSurface( JNIEnv* env, jobject surface) {
    INFO("SetSurface");
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

    m_renderer->Init(m_native_window, m_audio_frame_queue, m_video_frame_queue, m_subtitle_queue);
    ANativeWindow_release(m_native_window);

    return true;
}

bool SkyMediaPlayer::Start() {
    INFO("Start");
//    if(m_start.load()){
//        return false;
//    }

    m_demuxer->Start();
    m_renderer->Start();
//    m_start.store(true);
    return true;
}

bool SkyMediaPlayer::Pause() {
    if(m_renderer)
        return m_renderer->Pause();
    return false;
}

bool SkyMediaPlayer::Stop() {
    m_demuxer->StopAndRelease();
    m_renderer->Release();
    return true;
}

bool SkyMediaPlayer::Release() {
    Stop();
    m_demuxer.release();
    m_renderer.release();
    return true;
}

void SkyMediaPlayer::OnSizeChanged(int width, int height) {
    INFO("OnSizeChanged: %dx%d", width, height);
    m_width = width;
    m_height = height;
    if(m_message_queue){
        m_message_queue->PushMessage(new Message(MEDIA_SET_VIDEO_SIZE, width, height));
    }
}

void SkyMediaPlayer::OnAudioParamsChanged(int sampleRate, int channels) {

}

void SkyMediaPlayer::OnTimeBaseChanged(const AVMediaType& mediaType, const AVRational& timebase) {
    if(mediaType == AVMEDIA_TYPE_AUDIO){
        m_audio_timebase = timebase;
    }else if(mediaType == AVMEDIA_TYPE_VIDEO){
        m_video_timebase = timebase;
    }else if(mediaType == AVMEDIA_TYPE_SUBTITLE){
        m_subtitle_timebase = timebase;
    }
    if(m_renderer)
        m_renderer->SetTimebase(mediaType, timebase);
}

bool SkyMediaPlayer::PrepareAsync(const std::shared_ptr<MessageQueue> &messageQueue) {
    m_message_queue = messageQueue;

    if(m_message_queue){
        m_message_queue->PushMessage(new Message(MEDIA_SET_VIDEO_SIZE, m_width, m_height));
        m_message_queue->PushMessage(new Message(MEDIA_PREPARED));
    }

    return true;
}

int SkyMediaPlayer::GetDuration() {
    if(m_demuxer)
        return m_demuxer->GetDuration();
    return 0;
}

int SkyMediaPlayer::GetCurrentPosition() {
    if(m_renderer && m_demuxer)
        return m_renderer->GetCurrentTime() - m_demuxer->GetStartTime();
    return 0;
}
