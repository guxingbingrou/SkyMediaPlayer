//
// Created by zou on 2024/8/29.
//
#include "native-lib.h"
#include "jni/jvm.h"
#include "SkyMediaPlayer.h"
#include "NativeObserver.h"

jlong nativeCreateMediaPlayer(JNIEnv* env, jobject jobject1, jint type, jobject observer){
    SkyMediaPlayer* skyMediaPlayer = new SkyMediaPlayer(
            std::make_shared<NativeObserver>(env->NewGlobalRef(observer)));
    skyMediaPlayer->Init();
    return reinterpret_cast<jlong>(skyMediaPlayer);
}

jboolean nativeSetSource(JNIEnv* env, jobject jobject1, jlong nativeMediaPlayer, jstring path){
    SkyMediaPlayer* skyMediaPlayer = reinterpret_cast<SkyMediaPlayer*>(nativeMediaPlayer);

    bool ret = false;
    if(path != nullptr){
        const char* url = env->GetStringUTFChars(path, 0);
        ret = skyMediaPlayer->SetSource(url);
        env->ReleaseStringUTFChars(path, url);
    }
    return ret;
}

jboolean nativeStartMediaPlayer(JNIEnv* env, jobject jobject1, jlong nativeMediaPlayer){
    SkyMediaPlayer* skyMediaPlayer = reinterpret_cast<SkyMediaPlayer*>(nativeMediaPlayer);
    return skyMediaPlayer->Start();
}

jboolean nativeStopMediaPlayer(JNIEnv* env, jobject jobject1, jlong nativeMediaPlayer){
    SkyMediaPlayer* skyMediaPlayer = reinterpret_cast<SkyMediaPlayer*>(nativeMediaPlayer);
    return skyMediaPlayer->Stop();

}

jboolean nativeDestroyMediaPlayer(JNIEnv* env, jobject jobject1, jlong nativeMediaPlayer){
    SkyMediaPlayer* skyMediaPlayer = reinterpret_cast<SkyMediaPlayer*>(nativeMediaPlayer);
    jboolean ret = skyMediaPlayer->Release();
    delete skyMediaPlayer;
    return ret;
}

void nativeSetSurface(JNIEnv* env, jobject jobject1, jlong nativeMediaPlayer, jobject surface){
    SkyMediaPlayer* skyMediaPlayer = reinterpret_cast<SkyMediaPlayer*>(nativeMediaPlayer);
    skyMediaPlayer->SetSurface(env, surface);

}

