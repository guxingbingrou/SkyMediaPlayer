#include <jni.h>
#include <assert.h>
#include "jni/jvm.h"
#include "AndroidLog.h"
#include "jni/native-lib.h"
#include "global_java_class.h"

jclass gObserver;

static int registerNativeMethods(JNIEnv* env, const char* className,JNINativeMethod* getMethods,int methodsNum){
    jclass clazz;
    //找到声明native方法的类
    clazz = env->FindClass(className);
    if(clazz == NULL){
        return JNI_FALSE;
    }
    //注册函数 参数：java类 所要注册的函数数组 注册函数的个数
    if(env->RegisterNatives(clazz,getMethods,methodsNum) < 0){
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

static JNINativeMethod getMethods[] = {
        {"nativeCreateMediaPlayer","(ILcom/skystack/skymediaplayer/MediaPlayer/MediaPlayerObserver;)J", reinterpret_cast<void*>(nativeCreateMediaPlayer)},
        {"nativeSetSource","(JLjava/lang/String;)Z", reinterpret_cast<void*>(nativeSetSource)},
        {"nativeStartMediaPlayer","(J)Z", reinterpret_cast<void*>(nativeStartMediaPlayer)},
        {"nativeStopMediaPlayer","(J)Z", reinterpret_cast<void*>(nativeStopMediaPlayer)},
        {"nativeDestroyMediaPlayer","(J)Z", reinterpret_cast<void*>(nativeDestroyMediaPlayer)},
        {"nativeSetSurface","(JLandroid/view/Surface;)V", reinterpret_cast<void*>(nativeSetSurface)},
};

static void loadNativeClass(JNIEnv* env){
    jclass JavaObserver = env->FindClass(
            "com/skystack/skymediaplayer/MediaPlayer/MediaPlayerObserver");
    gObserver = static_cast<jclass>(env->NewGlobalRef(JavaObserver));
}

static void unloadNativeClass(JNIEnv* env){
    env->DeleteGlobalRef(gObserver);
}

extern "C" jint JNIEXPORT JNICALL JNI_OnLoad(JavaVM* jvm, void* reserved) {
    jint ret = sky::jni::InitGlobalJniVariables(jvm);
    if (ret < 0)return -1;

    JNIEnv* env =  sky::jni::GetEnv();
    assert(env != NULL);
    //注册函数 registerNatives ->registerNativeMethods ->env->RegisterNatives
    const char* className  = "com/skystack/skymediaplayer/MediaPlayer/SkyMediaPlayer";
    if(!registerNativeMethods(env,className,getMethods, sizeof(getMethods)/ sizeof(getMethods[0])))
        return -1;
    //返回jni 的版本

    loadNativeClass(env);

    return JNI_VERSION_1_6;
}

extern "C" void JNIEXPORT JNICALL JNI_OnUnLoad(JavaVM* jvm, void* reserved) {
    JNIEnv* env = sky::jni::AttachCurrentThreadIfNeeded();
    unloadNativeClass(env);
}