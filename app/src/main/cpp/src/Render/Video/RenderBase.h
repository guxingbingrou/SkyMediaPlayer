//
// Created by zou on 2022/9/20.
//

#ifndef SKYMEDIAPLAYER_RENDERBASE_H
#define SKYMEDIAPLAYER_RENDERBASE_H


#include <jni.h>
class RenderBase{
public:
    virtual ~RenderBase(){};
    virtual void InitRender(int targetWidth, int targetHeight, JNIEnv* env, jobject surface) = 0;
    virtual void OnDecodedFrame(const FrameParams* frame) = 0;
    virtual void DestroyRender() = 0;

    virtual void OnSurfaceCreated() = 0;
    virtual void OnSurfaceChanged(int width, int height) = 0;
    virtual void OnDrawFrame() = 0;
protected:
    int m_width;
    int m_height;
    jobject m_surface;
};





#endif //SKYMEDIAPLAYER_RENDERBASE_H