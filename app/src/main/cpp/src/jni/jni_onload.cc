#include <jni.h>
#include <assert.h>
#include "jni/jvm.h"
#include "AndroidLog.h"
#include "jni/native-lib.h"
#include "global_java_class.h"


jclass gJavaMediaPlayer;


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
        {"nativeInit","(I)V", reinterpret_cast<void*>(nativeInit)},
        {"nativeSetup","(Ljava/lang/Object;)V", reinterpret_cast<void*>(nativeSetup)},
        {"nativeSetSource","(Ljava/lang/String;)Z", reinterpret_cast<void*>(nativeSetSource)},
        {"nativeSetSurface","(Landroid/view/Surface;)V", reinterpret_cast<void*>(nativeSetSurface)},
        {"nativePrepareAsync","()V", reinterpret_cast<void*>(nativePrepareAsync)},
        {"nativeStartMediaPlayer","()Z", reinterpret_cast<void*>(nativeStartMediaPlayer)},
        {"nativePause","()Z", reinterpret_cast<void*>(nativePause)},
        {"nativeStopMediaPlayer","()Z", reinterpret_cast<void*>(nativeStopMediaPlayer)},
        {"nativeDestroyMediaPlayer","()Z", reinterpret_cast<void*>(nativeDestroyMediaPlayer)},

};

static void loadNativeClass(JNIEnv* env){
    jclass JavaMediaPlayer = env->FindClass(
            "com/skystack/skymediaplayer/MediaPlayer/SkyMediaPlayer");
    gJavaMediaPlayer = static_cast<jclass>(env->NewGlobalRef(JavaMediaPlayer));
}

static void unloadNativeClass(JNIEnv* env){
    env->DeleteGlobalRef(gJavaMediaPlayer);
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