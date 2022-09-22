//
// Created by zou on 2022/9/19.
//

#ifndef SKYMEDIAPLAYER_NATIVE_METHOD_H
#define SKYMEDIAPLAYER_NATIVE_METHOD_H

#include <jni.h>
#include "Decoder/VideoDecoderObserver.h"

class Observer : public VideoDecoderObserver{
public:
    Observer(jobject observer);
    void OnDecoderReady(int width, int height) override ;
    void OnDecoderOneFrame(FrameParams* params) override ;
    void OnDecoderDone() override ;

private:
    jobject m_java_observer;
};


extern jclass gObserver;

extern "C" jboolean NativeInitMediaPLayer(JNIEnv* env, jobject jobject1, jstring url, jobject surface,  jobject observer);
extern "C" jboolean NativeStartMediaPlayer(JNIEnv* env, jobject jobject1);
extern "C" jboolean NativeStopMediaPlayer(JNIEnv* env, jobject jobject1);
extern "C" jboolean NativeDestroyMediaPlayer(JNIEnv* env, jobject jobject1);
#endif //SKYMEDIAPLAYER_NATIVE_METHOD_H
