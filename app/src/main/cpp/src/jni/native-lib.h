//
// Created by zou on 2024/8/29.
//

#ifndef SKYMEDIAPLAYER_NATIVE_LIB_H
#define SKYMEDIAPLAYER_NATIVE_LIB_H

#include "jni.h"

extern "C" jlong nativeCreateMediaPlayer(JNIEnv* env, jobject jobject1, jint type);
extern "C" jboolean nativeSetSource(JNIEnv* env, jobject jobject1, jlong nativeMediaPlayer, jstring path);
extern "C" jboolean nativeStartMediaPlayer(JNIEnv* env, jobject jobject1, jlong nativeMediaPlayer);
extern "C" jboolean nativeStopMediaPlayer(JNIEnv* env, jobject jobject1, jlong nativeMediaPlayer);
extern "C" jboolean nativeDestroyMediaPlayer(JNIEnv* env, jobject jobject1, jlong nativeMediaPlayer);
extern "C" void nativeSetSurface(JNIEnv* env, jobject jobject1, jlong nativeMediaPlayer, jobject surface);



#endif //SKYMEDIAPLAYER_NATIVE_LIB_H
