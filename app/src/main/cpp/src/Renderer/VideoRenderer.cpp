//
// Created by zou on 2024/8/30.
//

#include "VideoRenderer.h"
#include "AndroidLog.h"
#include "GLUtils.h"
#include "libyuv.h"

// BT.709, which is the standard for HDTV.
static const GLfloat g_bt709[] = {
        1.164,  1.164,  1.164,
        0.0,   -0.213,  2.112,
        1.793, -0.533,  0.0,
};

static const GLfloat g_bt601[] = {
        1.164,  1.164, 1.164,
        0.0,   -0.392, 2.017,
        1.596, -0.813, 0.0,
};


#define SKY_GLES_STRINGIZE(x)   #x
#define SKY_GLES_STRINGIZE2(x)  SKY_GLES_STRINGIZE(x)
#define SKY_GLES_STRING(x)      SKY_GLES_STRINGIZE2(x)
static const char vShaderStr[] = SKY_GLES_STRING(
        precision highp float;
        varying highp vec2 vv2_Texcoord;
        attribute highp vec4 av4_Position;
        attribute highp vec2 av2_Texcoord;
        uniform mat4 um4_ModelViewProjection;

        void main() {
            gl_Position = um4_ModelViewProjection * av4_Position;
            vv2_Texcoord = av2_Texcoord.xy;
        }
);

static char fShaderStr[] = SKY_GLES_STRING(
        precision highp float;
        varying highp vec2 vv2_Texcoord;
        uniform mat3 um3_ColorConversion;
        uniform lowp sampler2D us2_SamplerX;
        uniform lowp sampler2D us2_SamplerY;
        uniform lowp sampler2D us2_SamplerZ;
        uniform int u_nImgType;  // 1:RGBA, 2:NV21, 3:NV12, 4:I420

        void main() {
            if (u_nImgType == 1) //RGBA
            {
                gl_FragColor = vec4(texture2D(us2_SamplerX, vv2_Texcoord).rgb, 1);
            } else if (u_nImgType == 2) //NV21\n"
            {
                mediump vec3 yuv;
                lowp vec3 rgb;
                yuv.x = (texture2D(us2_SamplerX, vv2_Texcoord).r - (16.0 / 255.0));
                yuv.y = (texture2D(us2_SamplerY, vv2_Texcoord).a - 0.5);
                yuv.z = (texture2D(us2_SamplerY, vv2_Texcoord).r - 0.5);
                rgb = um3_ColorConversion * yuv;
                gl_FragColor = vec4(rgb, 1);
            } else if (u_nImgType == 3) //NV12
            {
                mediump vec3 yuv;
                lowp vec3 rgb;
                yuv.x = (texture2D(us2_SamplerX, vv2_Texcoord).r - (16.0 / 255.0));
                yuv.y = (texture2D(us2_SamplerY, vv2_Texcoord).r - 0.5);
                yuv.z = (texture2D(us2_SamplerY, vv2_Texcoord).a - 0.5);
                rgb = um3_ColorConversion * yuv;
                gl_FragColor = vec4(rgb, 1);
            } else if (u_nImgType == 4)
            {
                mediump vec3 yuv;
                lowp    vec3 rgb;
                yuv.x = (texture2D(us2_SamplerX, vv2_Texcoord).r - (16.0 / 255.0));
                yuv.y = (texture2D(us2_SamplerY, vv2_Texcoord).r - 0.5);
                yuv.z = (texture2D(us2_SamplerZ, vv2_Texcoord).r - 0.5);
                rgb = um3_ColorConversion * yuv;
                gl_FragColor = vec4(rgb, 1.0);
            } else {
                gl_FragColor = vec4(1.0,0.0,0.0,1.0);
            }
        });

GLfloat verticesCoords[] = {
        -1.0f, 1.0f, 0.0f,  // Position 0
        -1.0f, -1.0f, 0.0f,  // Position 1
        1.0f, -1.0f, 0.0f,  // Position 2
        1.0f, 1.0f, 0.0f,  // Position 3
};

GLfloat textureCoords[] = {
        0.0f, 0.0f,        // TexCoord 0
        0.0f, 1.0f,        // TexCoord 1
        1.0f, 1.0f,        // TexCoord 2
        1.0f, 0.0f         // TexCoord 3
};


GLushort indices[] = {0, 1, 2, 0, 2, 3};


bool VideoRenderer::Init() {


    bool ret = true;
    ret = ret && InitEGL();

    ret = ret && PrepareDraw();

    return ret;
}

bool VideoRenderer::RenderPicture(const SkyFrame *skyFrame) {
//    INFO("RenderPicture");
    if (!MakeCurrent()) {
        return false;
    }

    EGLint egl_width = 0;
    if (!eglQuerySurface(m_display, m_surface, EGL_WIDTH, &egl_width)) {
        ERROR("[EGL] eglQuerySurface(EGL_WIDTH) returned error %d", eglGetError());
        return 0;
    }

    EGLint egl_height = 0;
    if (!eglQuerySurface(m_display, m_surface, EGL_HEIGHT, &egl_height)) {
        ERROR("[EGL] eglQuerySurface(EGL_HEIGHT) returned error %d", eglGetError());
        return 0;
    }

    if(egl_width != skyFrame->frame->width || egl_height != skyFrame->frame->height){
        int format = ANativeWindow_getFormat(m_native_window);
        if (ANativeWindow_setBuffersGeometry(m_native_window, skyFrame->frame->width, skyFrame->frame->height, format)) {
            ERROR("ANativeWindow_setBuffersGeometry failed");
            eglTerminate(m_display);
            return false;
        }

        int32_t width = ANativeWindow_getWidth(m_native_window);
        int32_t height = ANativeWindow_getHeight(m_native_window);
        INFO("egl: %dx%d,  native:%dx%d, format:%d", egl_width,egl_height, width, height, format);
    }

//
//
//    INFO("glViewport:%dx%d", skyFrame->frame->width, skyFrame->frame->height);
//    INFO("frame:%d,%d.%d", skyFrame->frame->linesize[0], skyFrame->frame->linesize[1],skyFrame->frame->linesize[2]);

    glViewport(0, 0, skyFrame->frame->width, skyFrame->frame->height);

    glClear(GL_COLOR_BUFFER_BIT);

    int frame_type = 0;

    switch (skyFrame->frame->format) {
        case AV_PIX_FMT_RGBA:
            glBindTexture(GL_TEXTURE_2D, m_textures[0]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                         skyFrame->frame->width, skyFrame->frame->height,
                         0, GL_RGBA, GL_UNSIGNED_BYTE, skyFrame->frame->data);

            glBindTexture(GL_TEXTURE_2D, 0);
            frame_type = 1;
            break;
        case AV_PIX_FMT_NV12:
            frame_type = 3;
        case AV_PIX_FMT_NV21:
            if(frame_type != 3)
                frame_type = 2;
            glBindTexture(GL_TEXTURE_2D, m_textures[0]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,
                         skyFrame->frame->width, skyFrame->frame->height,
                         0, GL_LUMINANCE, GL_UNSIGNED_BYTE, skyFrame->frame->data[0]);

            glBindTexture(GL_TEXTURE_2D, m_textures[1]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA,
                         skyFrame->frame->width, skyFrame->frame->height/2,
                         0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, skyFrame->frame->data[1]);
            glBindTexture(GL_TEXTURE_2D, 0);
            break;
        case AV_PIX_FMT_YUVJ420P:
        case AV_PIX_FMT_YUV420P:
            frame_type = 4;
            glBindTexture(GL_TEXTURE_2D, m_textures[0]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,
                         skyFrame->frame->linesize[0], skyFrame->frame->height,
                         0, GL_LUMINANCE, GL_UNSIGNED_BYTE, skyFrame->frame->data[0]);

            glBindTexture(GL_TEXTURE_2D, m_textures[1]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,
                         skyFrame->frame->linesize[1], skyFrame->frame->height/2,
                         0, GL_LUMINANCE, GL_UNSIGNED_BYTE, skyFrame->frame->data[1]);

            glBindTexture(GL_TEXTURE_2D, m_textures[2]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,
                         skyFrame->frame->linesize[2], skyFrame->frame->height/2,
                         0, GL_LUMINANCE, GL_UNSIGNED_BYTE, skyFrame->frame->data[2]);

            break;
    }

    glBindVertexArray(m_vao);

    glUniform1i(m_image_type_location, frame_type);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);

    glBindVertexArray(0);

    eglSwapBuffers(m_display, m_surface);
//    INFO("return :%d", count);
    ReleaseCurrent();
    return true;
}

bool VideoRenderer::Release() {
    return false;
}

bool VideoRenderer::InitEGL() {

    m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (m_display == EGL_NO_DISPLAY) {
        ERROR("eglGetDisplay failed");
        return false;
    }

    EGLint major, minor;
    if (!eglInitialize(m_display, &major, &minor)) {
        ERROR("eglInitialize failed");
        return false;
    }

    INFO("eglInitialize success, major:%d, minor:%d", major, minor);

    static const EGLint configAttribs[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_NONE
    };

    EGLConfig eglConfig;
    EGLint numConfig;
    if (!eglChooseConfig(m_display, configAttribs, &eglConfig, 1, &numConfig)) {
        ERROR("eglChooseConfig failed");
        eglTerminate(m_display);
        return false;
    }
    INFO("eglChooseConfig success, numConfig:%d", numConfig);
    static const EGLint contextAttribs[]{
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
    };

    m_context = eglCreateContext(m_display, eglConfig, EGL_NO_CONTEXT, contextAttribs);
    if (m_context == EGL_NO_CONTEXT) {
        ERROR("eglCreateContext failed");
        eglDestroySurface(m_display, m_surface);
        eglTerminate(m_display);
        return false;
    }
    INFO("eglCreateContext success");

    EGLint native_visual_id = 0;
    if (!eglGetConfigAttrib(m_display, eglConfig, EGL_NATIVE_VISUAL_ID, &native_visual_id)) {
        ERROR("eglGetConfigAttrib failed");
        eglTerminate(m_display);
        return false;
    }
    INFO("eglGetConfigAttrib success, native_visual_id:%d", native_visual_id);
    int32_t width = ANativeWindow_getWidth(m_native_window);
    int32_t height = ANativeWindow_getHeight(m_native_window);

    if (ANativeWindow_setBuffersGeometry(m_native_window, 0, 0, native_visual_id)) {
        ERROR("ANativeWindow_setBuffersGeometry failed");
        eglTerminate(m_display);
        return false;
    }

    INFO("ANativeWindow_setBuffersGeometry success width:%d, height:%d", width, height);

    m_surface = eglCreateWindowSurface(m_display, eglConfig, m_native_window, NULL);
    if (m_surface == EGL_NO_SURFACE) {
        ERROR("eglCreateWindowSurface failed");
        eglTerminate(m_display);
        return false;
    }
    INFO("eglCreateWindowSurface success ");



    return true;
}

bool VideoRenderer::MakeCurrent() {
    if (m_display == EGL_NO_DISPLAY || m_surface == EGL_NO_SURFACE || m_context == EGL_NO_CONTEXT) {
        return false;
    }
    if (!eglMakeCurrent(m_display, m_surface, m_surface, m_context)) {
        ERROR("eglMakeCurrent failed");
        eglDestroyContext(m_display, m_context);
        eglDestroySurface(m_display, m_surface);
        eglTerminate(m_display);
        return false;
    }
    return true;
}

bool VideoRenderer::ReleaseCurrent() {
    if (m_display == EGL_NO_DISPLAY) {
        return false;
    }
    eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

    eglReleaseThread();

    return false;
}

bool VideoRenderer::PrepareDraw() {
    if (!MakeCurrent()) {
        return false;
    }

    //program
    m_program = GLUtils::CreateProgram(vShaderStr, fShaderStr);
    if(m_program == 0){
        ERROR("create program failed");
        return false;
    }

    m_mvp_location = glGetUniformLocation(m_program, "um4_ModelViewProjection");

    m_samplers_location[0] = glGetUniformLocation(m_program, "us2_SamplerX");
    m_samplers_location[1] = glGetUniformLocation(m_program, "us2_SamplerY");
    m_samplers_location[2] = glGetUniformLocation(m_program, "us2_SamplerZ");


    m_color_conversion_location = glGetUniformLocation(m_program, "um3_ColorConversion");

    m_image_type_location = glGetUniformLocation(m_program, "u_nImgType");




//vbo\vao
    glGenBuffers(3, m_vbos);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCoords), verticesCoords, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbos[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoords), textureCoords, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbos[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbos[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3* sizeof(GLfloat), nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbos[1]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2* sizeof(GLfloat), nullptr);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbos[2]);

    glBindVertexArray(0);


    //use program
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glUseProgram(m_program);

    glGenTextures(3, m_textures);
    for(int i=0; i<3; ++i){
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, m_textures[i]);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glUniform1i(m_samplers_location[i], i);
    }


    glUniformMatrix3fv(m_color_conversion_location, 1, GL_FALSE, g_bt709);

    Sky_GLES_Matrix matrix;
    GenerateMatrix(&matrix, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
    glUniformMatrix4fv(m_mvp_location, 1, GL_FALSE, matrix.m);


    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDisable(GL_DEPTH_TEST);

    ReleaseCurrent();
    return true;
}

void VideoRenderer::GenerateMatrix(Sky_GLES_Matrix *matrix, GLfloat left, GLfloat right, GLfloat bottom,
                              GLfloat top, GLfloat near, GLfloat far) {
    GLfloat r_l = right - left;
    GLfloat t_b = top - bottom;
    GLfloat f_n = far - near;
    GLfloat tx = - (right + left) / (right - left);
    GLfloat ty = - (top + bottom) / (top - bottom);
    GLfloat tz = - (far + near) / (far - near);

    matrix->m[0] = 2.0f / r_l;
    matrix->m[1] = 0.0f;
    matrix->m[2] = 0.0f;
    matrix->m[3] = 0.0f;

    matrix->m[4] = 0.0f;
    matrix->m[5] = 2.0f / t_b;
    matrix->m[6] = 0.0f;
    matrix->m[7] = 0.0f;

    matrix->m[8] = 0.0f;
    matrix->m[9] = 0.0f;
    matrix->m[10] = -2.0f / f_n;
    matrix->m[11] = 0.0f;

    matrix->m[12] = tx;
    matrix->m[13] = ty;
    matrix->m[14] = tz;
    matrix->m[15] = 1.0f;

}

void VideoRenderer::SetWindow(ANativeWindow *native_window) {
    m_native_window = native_window;
    ANativeWindow_acquire(m_native_window);
    INFO("SetWindow");

}
