//
// Created by zou on 2024/8/30.
//

#ifndef SKYMEDIAPLAYER_VIDEORENDERER_H
#define SKYMEDIAPLAYER_VIDEORENDERER_H

#include <android/native_window_jni.h>
#include <android/native_window.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include "SkyFrameQueue.h"


typedef struct Sky_GLES_Matrix
{
    GLfloat m[16];
} Sky_GLES_Matrix;

class VideoRenderer {
public:
    VideoRenderer() = default;
    void SetWindow(ANativeWindow* native_window);
    bool Init();

    bool RenderPicture(const SkyFrame* skyFrame);

    bool Release();


private:
    bool InitEGL();
    bool MakeCurrent();
    bool ReleaseCurrent();
    bool PrepareDraw();
    void GenerateMatrix(Sky_GLES_Matrix* matrix, GLfloat left, GLfloat right,
                        GLfloat bottom, GLfloat top, GLfloat near, GLfloat far);


    ANativeWindow* m_native_window = nullptr;
    EGLDisplay m_display;
    EGLSurface m_surface;
    EGLContext m_context;
    GLuint m_program;

    GLint m_mvp_location = -1;
    GLuint m_samplers_location[3];
    GLuint m_color_conversion_location = 0;
    GLuint m_image_type_location = 0;
    GLuint m_texcoords_location = 0;
    GLuint m_position_location = 0;

    GLuint m_textures[3];
    GLuint m_vbos[3];
    GLuint m_vao;
};


#endif //SKYMEDIAPLAYER_VIDEORENDERER_H
