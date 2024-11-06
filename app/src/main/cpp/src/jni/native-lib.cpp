//
// Created by zou on 2024/8/29.
//
#include "native-lib.h"
#include "jni/jvm.h"
#include "SkyMediaPlayer.h"
#include "MediaPlayerProxy.h"
#include <memory>
void nativeInit(JNIEnv* env, jobject jobject1, jint type){
    MediaPlayerProxy::GetProxy()->Init();
}

void nativeSetup(JNIEnv* env, jobject jobject1, jobject obj){
    MediaPlayerProxy::GetProxy()->Setup(env->NewGlobalRef(obj));
}

jboolean nativeSetSource(JNIEnv* env, jobject jobject1, jstring path){
    bool ret = false;
    if(path != nullptr){
        const char* url = env->GetStringUTFChars(path, 0);
        ret = MediaPlayerProxy::GetProxy()->SetSource(url);
        env->ReleaseStringUTFChars(path, url);
    }
    return ret;
}

void nativeSetSurface(JNIEnv* env, jobject jobject1, jobject surface){
    MediaPlayerProxy::GetProxy()->SetSurface(env, surface);
}

void nativePrepareAsync(JNIEnv* env, jobject jobject1){
    MediaPlayerProxy::GetProxy()->PrepareAsync();
}

jboolean nativeStartMediaPlayer(JNIEnv* env, jobject jobject1){
    return MediaPlayerProxy::GetProxy()->Start();
}

jboolean nativePause(JNIEnv* env, jobject jobject1){
    return MediaPlayerProxy::GetProxy()->Pause();
}

jboolean nativeStopMediaPlayer(JNIEnv* env, jobject jobject1){
    return MediaPlayerProxy::GetProxy()->Stop();

}

jboolean nativeDestroyMediaPlayer(JNIEnv* env, jobject jobject1){
    jboolean ret = MediaPlayerProxy::GetProxy()->Release();
    return ret;
}



