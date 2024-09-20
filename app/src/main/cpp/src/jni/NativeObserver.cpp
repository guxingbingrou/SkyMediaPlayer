//
// Created by zou on 2024/9/20.
//

#include "NativeObserver.h"
#include "global_java_class.h"
#include "AndroidLog.h"

NativeObserver::NativeObserver(jobject jobserver) {
    m_java_observer = jobserver;
}

void NativeObserver::OnSizeChanged(int width, int height) {
    JNIEnv* env = sky::jni::AttachCurrentThreadIfNeeded();

    if(!m_methodOnResolutionChanged)
        m_methodOnResolutionChanged = env->GetMethodID(gObserver, "OnResolutionChanged", "(II)V");

    env->CallVoidMethod(m_java_observer, m_methodOnResolutionChanged, width, height);

}
