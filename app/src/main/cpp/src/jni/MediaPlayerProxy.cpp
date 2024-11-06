//
// Created by zou on 2024/11/5.
//

#include "MediaPlayerProxy.h"
#include "AndroidLog.h"
#include "jvm.h"
#include "global_java_class.h"
std::mutex MediaPlayerProxy::s_mutex;
std::shared_ptr<MediaPlayerProxy> MediaPlayerProxy::s_proxy;

std::shared_ptr<MediaPlayerProxy> MediaPlayerProxy::GetProxy() {
    if(!s_proxy){
        std::unique_lock<std::mutex> lck(s_mutex);
        if(!s_proxy){
            s_proxy = std::shared_ptr<MediaPlayerProxy>(new MediaPlayerProxy());
        }
    }
    return s_proxy;
}

bool MediaPlayerProxy::Init() {
    m_player = std::make_unique<SkyMediaPlayer>();
    m_player->Init();
    return true;
}

bool MediaPlayerProxy::Setup(jobject obj) {
    m_java_player = obj;
    return true;
}

bool MediaPlayerProxy::PrepareAsync() {
    if(!m_message_queue){
        m_message_queue = std::make_shared<MessageQueue>();
    }

    m_running = true;
    m_thread = std::thread(&MediaPlayerProxy::Loop, this);

    return m_player->PrepareAsync(m_message_queue);
}

bool MediaPlayerProxy::SetSource(const char *url) {
    return m_player->SetSource(url);
}

bool MediaPlayerProxy::SetSurface(JNIEnv *env, jobject surface) {
    return m_player->SetSurface(env, surface);
}

bool MediaPlayerProxy::Start() {
    return m_player->Start();
}

bool MediaPlayerProxy::Pause() {
    return m_player->Pause();
}

bool MediaPlayerProxy::Stop() {
    return m_player->Stop();
}

bool MediaPlayerProxy::Release() {
    INFO("Release");
    bool ret = m_player->Release();
    m_player.release();

    m_running = false;
    m_message_queue->Abort();
    m_message_queue.reset();

    if(m_thread.joinable())
        m_thread.join();

    return ret;
}

void MediaPlayerProxy::Loop() {
    Message* msg = nullptr;
    bool ret;
    while (m_running){
        ret = m_message_queue->GetMessage(&msg);
        if(!ret){
            WARNING("get message failed");
            continue;
        }
        switch (msg->m_what) {
            case MEDIA_PREPARED:
            case MEDIA_PLAYBACK_COMPLETE:
            case MEDIA_BUFFERING_UPDATE:
            case MEDIA_SEEK_COMPLETE:
            case MEDIA_SET_VIDEO_SIZE:
            case MEDIA_ERROR:
            case MEDIA_INFO:
            case MEDIA_SET_VIDEO_SAR:
                PostEvent(msg->m_what, msg->m_arg1, msg->m_arg2, nullptr);
                break;
            case MEDIA_TIMED_TEXT:
                if(msg->m_obj){
                    JNIEnv* env = sky::jni::AttachCurrentThreadIfNeeded();
                    jstring text = env->NewStringUTF(static_cast<char*>(msg->m_obj));
                    PostEvent(msg->m_what, msg->m_arg1, msg->m_arg2, text);
//                    env->ReleaseStringUTFChars(text, static_cast<char*>(msg->m_obj));
                    env->DeleteLocalRef(text);
                }else{
                    PostEvent(msg->m_what, msg->m_arg1, msg->m_arg2, nullptr);
                }
                break;

            default:
                ERROR("unknown message type");
                break;
        }
    }
}

void MediaPlayerProxy::PostEvent(jint what, jint arg1, jint arg2, jobject obj ) {
    JNIEnv* env = sky::jni::AttachCurrentThreadIfNeeded();

    jclass javaSkyMediaPlayer = gJavaMediaPlayer;
    if(m_method_post_event == nullptr){
        m_method_post_event = env->GetMethodID(javaSkyMediaPlayer, "postEventFromNative", "(IIILjava/lang/Object;)V");
    }

    INFO("PostEvent: %d", what);

    env->CallVoidMethod(m_java_player, m_method_post_event, what, arg1, arg2, obj);

}

MediaPlayerProxy::~MediaPlayerProxy() {

}

MediaPlayerProxy::MediaPlayerProxy() {

}
