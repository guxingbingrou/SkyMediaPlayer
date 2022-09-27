//
// Created by zou on 2022/9/19.
//
#include "native_method.h"
#include <fstream>
#include "DemuxingDecoder/FFDemuxingDecoder.h"
#include "utils/AndroidLog.h"
#include "Render/Video/NativeRender.h"
#include "Render/Audio/OpenslPLayer.h"
#include "jni/jvm.h"

static std::unique_ptr<DemuxingDecoderBase> s_demuxing_base;
static std::shared_ptr<RenderBase> s_render;
static std::shared_ptr<AudioPlayer> s_audio_player;
static std::shared_ptr<Observer> s_video_observer;

jboolean NativeInitMediaPLayer(JNIEnv* env, jobject jobject1, jstring url, jobject surface, jobject observer){
    s_render = std::make_shared<NativeRender>();


    s_demuxing_base = std::make_unique<FFDemuxingDecoder>();

    const char* path = env->GetStringUTFChars(url, 0);


    s_video_observer = std::make_shared<Observer>(env->NewGlobalRef(observer));

    s_audio_player = std::make_shared<OpenslPLayer>();
    s_audio_player->InitAudioPlayer(2, 48000, AUDIO_FORMAT_S16);

    bool ret = s_demuxing_base->Init(path, s_render, s_audio_player, s_video_observer, nullptr);

    s_render->InitRender(s_demuxing_base->GetVideoWidth(), s_demuxing_base->GetVideoHeight(), env, surface);

    env->ReleaseStringUTFChars(url, path);

    return ret;
}

jboolean NativeStartMediaPlayer(JNIEnv* env, jobject jobject1){
    return s_demuxing_base->Start();
}

jboolean NativeStopMediaPlayer(JNIEnv* env, jobject jobject1){
    return s_demuxing_base->Stop();
}

jboolean NativeDestroyMediaPlayer(JNIEnv* env, jobject jobject1){

    return s_demuxing_base->Destroy();
}



Observer::Observer(jobject observer) {
    m_java_observer = observer;
}

void Observer::OnDecoderReady(int width, int height) {
    JNIEnv *env = sky::jni::AttachCurrentThreadIfNeeded();

    jmethodID method = env->GetMethodID(gObserver, "OnDecoderReady", "(II)V");

    env->CallVoidMethod(m_java_observer, method, width, height);
}

void Observer::OnDecoderOneFrame(FrameParams* params) {
//    JNIEnv *env = sky::jni::AttachCurrentThreadIfNeeded();
//
//    jmethodID method = env->GetMethodID(gObserver, "OnDecoderOneFrame", "()V");
//
//    env->CallVoidMethod(m_java_observer, method);
}

void Observer::OnDecoderDone() {
    JNIEnv *env = sky::jni::AttachCurrentThreadIfNeeded();

    jmethodID method = env->GetMethodID(gObserver, "OnDecoderDone", "()V");

    env->CallVoidMethod(m_java_observer, method);
}


