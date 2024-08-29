//
// Created by zou on 2024/8/29.
//

#ifndef SKYMEDIAPLAYER_NATIVE_LIB_H
#define SKYMEDIAPLAYER_NATIVE_LIB_H

#include "jni.h"

extern "C" jlong nativeCreateMediaPlayer(JNIEnv* env, jint type);
extern "C" jboolean nativeSetSource(jlong nativeMediaPlayer, jobject path);
extern "C" jboolean nativeStartMediaPlayer(jlong nativeMediaPlayer);
extern "C" jboolean nativeStopMediaPlayer(jlong nativeMediaPlayer);
extern "C" jboolean nativeDestroyMediaPlayer(jlong nativeMediaPlayer);
extern "C" jboolean nativeOnSurfaceCreated(jlong nativeMediaPlayer);
extern "C" jboolean nativeOnSurfaceChanged(jlong nativeMediaPlayer, jint width, jint height);
extern "C" jboolean nativeOnDrawFrame(jlong nativeMediaPlayer);


#endif //SKYMEDIAPLAYER_NATIVE_LIB_H
