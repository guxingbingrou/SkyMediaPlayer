#include <jni.h>
#include <assert.h>
#include "jni/jvm.h"
#include "utils/AndroidLog.h"
#include "native_method.h"

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
        {"NativeInitMediaPLayer","(Ljava/lang/String;Landroid/view/Surface;Lcom/skystack/skymediaplayer/MediaPlayer/VideoDecoderObserver;)Z", reinterpret_cast<void*>(NativeInitMediaPLayer)},
        {"NativeStartMediaPlayer","()Z", reinterpret_cast<void*>(NativeStartMediaPlayer)},
        {"NativeStopMediaPlayer","()Z", reinterpret_cast<void*>(NativeStopMediaPlayer)},
        {"NativeDestroyMediaPlayer","()Z", reinterpret_cast<void*>(NativeDestroyMediaPlayer)},
        {"NativeOnSurfaceCreated","()V", reinterpret_cast<void*>(NativeOnSurfaceCreated)},
        {"NativeOnSurfaceChanged","(II)V", reinterpret_cast<void*>(NativeOnSurfaceChanged)},
        {"NativeOnDrawFrame","()V", reinterpret_cast<void*>(NativeOnDrawFrame)},
};

static void loadNativeClass(JNIEnv* env){
    jobject javaVideoDecoderObserver = env->FindClass(
            "com/skystack/skymediaplayer/MediaPlayer/VideoDecoderObserver");
    gObserver = static_cast<jclass>(env->NewGlobalRef(javaVideoDecoderObserver));
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
    const char* className  = "com/skystack/skymediaplayer/MediaPlayer/FFMediaPlayer";
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