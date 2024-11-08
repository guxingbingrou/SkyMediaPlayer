//
// Created by zou on 2024/11/5.
//

#ifndef SKYMEDIAPLAYER_MEDIAPLAYERPROXY_H
#define SKYMEDIAPLAYER_MEDIAPLAYERPROXY_H

#include "SkyMediaPlayer.h"
#include "MessageQueue.h"
#include <mutex>
#include <thread>
#include <memory>

class MediaPlayerProxy {
public:
    static std::shared_ptr<MediaPlayerProxy> GetProxy();

    bool Init();
    bool Setup(jobject obj);
    bool PrepareAsync();
    bool SetSource(const char* url);
    bool SetSurface(JNIEnv* env, jobject surface);
    bool Start();
    bool Pause();
    bool Stop();
    bool Release();
    bool SeekTo(int msec);

    int GetDuration();
    int GetCurrentPosition();

    ~MediaPlayerProxy();
private:
    MediaPlayerProxy();
    void Loop();
    void PostEvent(jint what, jint arg1 = -1, jint arg2 = -1, jobject obj = nullptr);

    std::unique_ptr<SkyMediaPlayer> m_player;

    jobject m_java_player;
    jmethodID m_method_post_event = nullptr;

    static std::mutex s_mutex;
    std::shared_ptr<MessageQueue> m_message_queue;
    static std::shared_ptr<MediaPlayerProxy> s_proxy;
    std::thread m_thread;
    bool m_running;

};


#endif //SKYMEDIAPLAYER_MEDIAPLAYERPROXY_H
