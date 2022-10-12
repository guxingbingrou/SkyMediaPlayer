//
// Created by zou on 2022/10/12.
//

#include "GLRender.h"


static char vShaderStr[] =
        "#version 300 es\n"
        "layout(location = 0) in vec4 a_position;\n"
        "layout(location = 1) in vec2 a_texCoord;\n"
//        "uniform mat4 u_MVPMatrix;\n"
        "out vec2 v_texCoord;\n"
        "void main()\n"
        "{\n"
        " gl_Position = a_position; \n"
//        "    gl_Position = u_MVPMatrix * a_position;\n"
        "    v_texCoord = a_texCoord;\n"
        "}";

static char fShaderStr[] =
        "#version 300 es\n"
        "precision highp float;\n"
        "in vec2 v_texCoord;\n"
        "layout(location = 0) out vec4 outColor;\n"
        "uniform sampler2D s_texture0;\n"
        "uniform sampler2D s_texture1;\n"
        "uniform sampler2D s_texture2;\n"
        "uniform int u_nImgType;// 1:RGBA, 2:NV21, 3:NV12, 4:I420\n"
        "\n"
        "void main()\n"
        "{\n"
        "\n"
        "    if(u_nImgType == 1) //RGBA\n"
        "    {\n"
        "        outColor = texture(s_texture0, v_texCoord);\n"
        "    }\n"
        "    else if(u_nImgType == 2) //NV21\n"
        "    {\n"
        "        vec3 yuv;\n"
        "        yuv.x = texture(s_texture0, v_texCoord).r;\n"
        "        yuv.y = texture(s_texture1, v_texCoord).a - 0.5;\n"
        "        yuv.z = texture(s_texture1, v_texCoord).r - 0.5;\n"
        "        highp vec3 rgb = mat3(1.0,       1.0,     1.0,\n"
        "        0.0, \t-0.344, \t1.770,\n"
        "        1.403,  -0.714,     0.0) * yuv;\n"
        "        outColor = vec4(rgb, 1.0);\n"
        "\n"
        "    }\n"
        "    else if(u_nImgType == 3) //NV12\n"
        "    {\n"
        "        vec3 yuv;\n"
        "        yuv.x = texture(s_texture0, v_texCoord).r;\n"
        "        yuv.y = texture(s_texture1, v_texCoord).r - 0.5;\n"
        "        yuv.z = texture(s_texture1, v_texCoord).a - 0.5;\n"
        "        highp vec3 rgb = mat3(1.0,       1.0,     1.0,\n"
        "        0.0, \t-0.344, \t1.770,\n"
        "        1.403,  -0.714,     0.0) * yuv;\n"
        "        outColor = vec4(rgb, 1.0);\n"
        "    }\n"
        "    else if(u_nImgType == 4) //I420\n"
        "    {\n"
        "        vec3 yuv;\n"
        "        yuv.x = texture(s_texture0, v_texCoord).r;\n"
        "        yuv.y = texture(s_texture1, v_texCoord).r - 0.5;\n"
        "        yuv.z = texture(s_texture2, v_texCoord).r - 0.5;\n"
        "        highp vec3 rgb = mat3(1.0,       1.0,     1.0,\n"
        "                              0.0, \t-0.344, \t1.770,\n"
        "                              1.403,  -0.714,     0.0) * yuv;\n"
        "        outColor = vec4(rgb, 1.0);\n"
        "    }\n"
        "    else\n"
        "    {\n"
        "        outColor = vec4(1.0);\n"
        "    }\n"
        "}";

GLfloat verticesCoords[] = {
        -1.0f,  1.0f, 0.0f,  // Position 0
        -1.0f, -1.0f, 0.0f,  // Position 1
        1.0f,  -1.0f, 0.0f,  // Position 2
        1.0f,   1.0f, 0.0f,  // Position 3
};

GLfloat textureCoords[] = {
        0.0f,  0.0f,        // TexCoord 0
        0.0f,  1.0f,        // TexCoord 1
        1.0f,  1.0f,        // TexCoord 2
        1.0f,  0.0f         // TexCoord 3
};

GLushort indices[] = { 0, 1, 2, 0, 2, 3 };

GLRender::GLRender() {

}

GLRender::~GLRender() {

}

void GLRender::InitRender(int targetWidth, int targetHeight, JNIEnv *env, jobject surface) {
    m_height = targetHeight;
    m_width = targetWidth;

    m_buffer_queue = std::make_unique<BufferQueue<FrameParams>>();
}

void GLRender::OnDecodedFrame(const FrameParams *frame) {
    FrameParams temp;
    temp.height = frame->height;
    temp.width = frame->width;
    temp.format = frame->format;
    temp.timestamp = frame->timestamp;
    switch (temp.format) {
        case IMAGE_FORMAT_RGBA:
            temp.video_raw_linesize[0] = temp.width * 4;
            temp.video_raw_data[0] = static_cast<uint8_t *>(malloc(
                    temp.video_raw_linesize[0] * temp.height));

            if(temp.video_raw_linesize[0] == frame->video_raw_linesize[0]){
                memcpy(temp.video_raw_data[0], frame->video_raw_data[0], temp.video_raw_linesize[0] * temp.height);
            }else{
                for(int i=0 ; i<temp.height; ++i){
                    memcpy(temp.video_raw_data[0] + i * temp.video_raw_linesize[0],
                            frame->video_raw_data[0] + i * frame->video_raw_linesize[0], temp.video_raw_linesize[0]);
                }
            }

            break;
        case IMAGE_FORMAT_NV21:
        case IMAGE_FORMAT_NV12:
            temp.video_raw_linesize[0] = temp.width;
            temp.video_raw_linesize[1] = temp.width;
            temp.video_raw_data[0] = static_cast<uint8_t *>(malloc(
                    temp.width * temp.height * 3 / 2));
            temp.video_raw_data[1] = temp.video_raw_data[0] + temp.width * temp.height;

            //y
            if(temp.video_raw_linesize[0] == frame->video_raw_linesize[0]){
                memcpy(temp.video_raw_data[0], frame->video_raw_data[0], temp.video_raw_linesize[0] * temp.height);
            }else{
                for(int i=0 ; i<temp.height; ++i){
                    memcpy(temp.video_raw_data[0] + i * temp.video_raw_linesize[0],
                           frame->video_raw_data[0] + i * frame->video_raw_linesize[0], temp.video_raw_linesize[0]);
                }
            }

            //uv
            if(temp.video_raw_linesize[1] == frame->video_raw_linesize[1]){
                memcpy(temp.video_raw_data[1], frame->video_raw_data[1], temp.video_raw_linesize[1] * temp.height/2);
            }else{
                for(int i=0 ; i<temp.height/2; ++i){
                    memcpy(temp.video_raw_data[1] + i * temp.video_raw_linesize[1],
                           frame->video_raw_data[1] + i * frame->video_raw_linesize[1], temp.video_raw_linesize[1]);
                }
            }
            break;
        case IMAGE_FORMAT_I420:
        case IMAGE_FORMAT_J420:
            temp.video_raw_linesize[0] = temp.width;
            temp.video_raw_linesize[1] = temp.width / 2;
            temp.video_raw_linesize[2] = temp.width / 2;

            temp.video_raw_data[0] = static_cast<uint8_t *>(malloc(
                    temp.width * temp.height * 3 / 2));
            temp.video_raw_data[1] = temp.video_raw_data[0] + temp.width * temp.height;
            temp.video_raw_data[2] = temp.video_raw_data[1] + temp.width * temp.height / 4;

            //y
            if(temp.video_raw_linesize[0] == frame->video_raw_linesize[0]){
                memcpy(temp.video_raw_data[0], frame->video_raw_data[0], temp.video_raw_linesize[0] * temp.height);
            }else{
                for(int i=0 ; i<temp.height; ++i){
                    memcpy(temp.video_raw_data[0] + i * temp.video_raw_linesize[0],
                           frame->video_raw_data[0] + i * frame->video_raw_linesize[0], temp.video_raw_linesize[0]);
                }
            }

            //u
            if(temp.video_raw_linesize[1] == frame->video_raw_linesize[1]){
                memcpy(temp.video_raw_data[1], frame->video_raw_data[1], temp.video_raw_linesize[1] * temp.height/2);
            }else{
                for(int i=0 ; i<temp.height/2; ++i){
                    memcpy(temp.video_raw_data[1] + i * temp.video_raw_linesize[1],
                           frame->video_raw_data[1] + i * frame->video_raw_linesize[1], temp.video_raw_linesize[1]);
                }
            }

            //v
            if(temp.video_raw_linesize[2] == frame->video_raw_linesize[2]){
                memcpy(temp.video_raw_data[2], frame->video_raw_data[2], temp.video_raw_linesize[2] * temp.height/2);
            }else{
                for(int i=0 ; i<temp.height/2; ++i){
                    memcpy(temp.video_raw_data[2] + i * temp.video_raw_linesize[2],
                           frame->video_raw_data[2] + i * frame->video_raw_linesize[2], temp.video_raw_linesize[2]);
                }
            }
            break;
        default:
            ERROR("unknown format(%d)", temp.format);
    }

    m_buffer_queue->WaitePushBuffer(temp);
}

void GLRender::DestroyRender() {

}

void GLRender::OnSurfaceCreated() {
    m_program = GLUtils::CreateProgram(vShaderStr, fShaderStr);
    if(!m_program){
        ERROR("create program failed");
        return;
    }


    for(int i=0; i<3; ++i){
        char samplerName[64] = {0};
        sprintf(samplerName, "s_texture%d", i);
        m_texture_location[i] = glGetUniformLocation(m_program, samplerName);
    }
    m_image_type_location = glGetUniformLocation(m_program, "u_nImgType");


    glGenTextures(3, m_textures);
    for(int i=0; i<3; ++i){
        glBindTexture(GL_TEXTURE_2D, m_textures[i]);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    glBindTexture(GL_TEXTURE_2D, 0);

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbos[1]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbos[2]);


}

void GLRender::OnSurfaceChanged(int width, int height) {
    glViewport(0, 0, width, height);
    glClearColor(0, 0, 0, 0);

    m_width = width;
    m_height = height;
}

void GLRender::OnDrawFrame() {
//    if(m_program == 0 || m_buffer_queue->IsEmpty()){
//        WARNING("program or frame is not ready, skip one frame");
//        return;
//    }
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0, 0, 0, 0);
    FrameParams frame;
    m_buffer_queue->WaitePopBuffer(frame);
    int frame_type = 0; //1:RGBA, 2:NV21, 3:NV12, 4:I420
    switch (frame.format) {
        case IMAGE_FORMAT_RGBA:
            glBindTexture(GL_TEXTURE_2D, m_textures[0]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frame.width, frame.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, frame.video_raw_data[0]);
            glBindTexture(GL_TEXTURE_2D, 0);
            frame_type = 1;
            break;
        case IMAGE_FORMAT_NV12:
            frame_type = 3;
        case IMAGE_FORMAT_NV21:
            if(frame_type != 3)
                frame_type = 2;
            glBindTexture(GL_TEXTURE_2D, m_textures[0]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, frame.width, frame.height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, frame.video_raw_data[0]);

            glBindTexture(GL_TEXTURE_2D, m_textures[1]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, frame.width, frame.height/2, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, frame.video_raw_data[1]);

            break;
        case IMAGE_FORMAT_J420:
        case IMAGE_FORMAT_I420:
            frame_type = 4;
            glBindTexture(GL_TEXTURE_2D, m_textures[0]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, frame.width, frame.height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, frame.video_raw_data[0]);

            glBindTexture(GL_TEXTURE_2D, m_textures[1]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, frame.width/2, frame.height/2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, frame.video_raw_data[1]);

            glBindTexture(GL_TEXTURE_2D, m_textures[2]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, frame.width/2, frame.height/2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, frame.video_raw_data[2]);
            break;
        default:
            ERROR("unknown format(%d)", frame.format);
            break;
    }

    glUseProgram(m_program);

    glBindVertexArray(m_vao);

    for(int i=0; i<3; ++i){
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, m_textures[i]);
        glUniform1i(m_texture_location[i], i);
    }

    glUniform1i(m_image_type_location, frame_type);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);


    free(frame.video_raw_data[0]);
}

