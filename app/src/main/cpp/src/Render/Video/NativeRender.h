//
// Created by zou on 2022/9/20.
//

#ifndef SKYMEDIAPLAYER_NATIVERENDER_H
#define SKYMEDIAPLAYER_NATIVERENDER_H

#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "utils/PlayerFormats.h"

#include "RenderBase.h"
class NativeRender : public RenderBase{
public:
    NativeRender();
    virtual ~NativeRender();
    void InitRender(int targetWidth, int targetHeight, JNIEnv* env, jobject surface) override ;
    void OnDecodedFrame(const FrameParams* frame) override ;
    void DestroyRender() override ;

private:
    ANativeWindow* m_native_window = nullptr;
    ANativeWindow_Buffer m_native_buffer;

};


#endif //SKYMEDIAPLAYER_NATIVERENDER_H
