//
// Created by zou on 2024/8/29.
//

#ifndef SKYMEDIAPLAYER_NATIVE_LIB_H
#define SKYMEDIAPLAYER_NATIVE_LIB_H

#include "jni.h"

extern "C" void nativeInit(JNIEnv* env, jobject jobject1, jint type);
extern "C" void nativeSetup(JNIEnv* env, jobject jobject1, jobject obj);
extern "C" jboolean nativeSetSource(JNIEnv* env, jobject jobject1, jstring path);
extern "C" void nativeSetSurface(JNIEnv* env, jobject jobject1, jobject surface);
extern "C" void nativePrepareAsync(JNIEnv* env, jobject jobject1);
extern "C" jboolean nativeStartMediaPlayer(JNIEnv* env, jobject jobject1);
extern "C" jboolean nativePause(JNIEnv* env, jobject jobject1);
extern "C" jboolean nativeStopMediaPlayer(JNIEnv* env, jobject jobject1);
extern "C" jboolean nativeDestroyMediaPlayer(JNIEnv* env, jobject jobject1);

extern "C" jint nativeGetDuration(JNIEnv* env, jobject jobject1);
extern "C" jint nativeGetCurrentPosition(JNIEnv* env, jobject jobject1);
extern "C" void nativeSeekTo(JNIEnv* env, jobject jobject1, jint msec);

#endif //SKYMEDIAPLAYER_NATIVE_LIB_H
