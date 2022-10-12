//
// Created by zou on 2022/10/12.
//

#ifndef SKYMEDIAPLAYER_GLRENDER_H
#define SKYMEDIAPLAYER_GLRENDER_H

#include "utils/PlayerFormats.h"
#include "utils/BufferQueue.h"

#include "RenderBase.h"
#include "GLUtils.h"
class GLRender : public RenderBase{
public:
    GLRender();
    virtual ~GLRender();
    void InitRender(int targetWidth, int targetHeight, JNIEnv* env, jobject surface) override ;
    void OnDecodedFrame(const FrameParams* frame) override ;
    void DestroyRender() override ;

    void OnSurfaceCreated() override ;
    void OnSurfaceChanged(int width, int height) override;
    void OnDrawFrame() override;

private:
    std::unique_ptr<BufferQueue<FrameParams>> m_buffer_queue;
    GLuint m_program = 0;
    GLuint m_textures[3];
    GLuint m_vbos[3];
    GLuint m_vao = 0;

    int m_texture_location[3];
    int m_image_type_location = 0;
};


#endif //SKYMEDIAPLAYER_GLRENDER_H
