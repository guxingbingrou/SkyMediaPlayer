//
// Created by zou on 2022/10/12.
//

#include <cstdlib>
#include "GLUtils.h"
#include "AndroidLog.h"

GLuint GLUtils::LoadShader(GLenum type, const char *shader_source) {
    GLuint shader = 0;
    shader = glCreateShader(type);
    if(shader != 0){
        glShaderSource(shader, 1, &shader_source, nullptr);

        glCompileShader(shader);

        GLint ret = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &ret);
        if(!ret){
            GLint length = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
            if(length){
                char* buf = static_cast<char *>(malloc(length));

                glGetShaderInfoLog(shader, length, nullptr, buf);
                ERROR("Compile Shader(type:%d) failed: %s", type, buf);
                free(buf);
            }

            glDeleteShader(shader);
            shader = 0;
        }
    }
    return shader;
}

GLuint GLUtils::CreateProgram(const char *vertex_shader_source, const char *fragment_shader_source) {
    GLuint program = 0;

    GLuint vertex_shader = LoadShader(GL_VERTEX_SHADER, vertex_shader_source);
    if(vertex_shader == 0){
        ERROR("create vertex shader failed");
        return program;
    }

    GLuint fragment_shader = LoadShader(GL_FRAGMENT_SHADER, fragment_shader_source);
    if(fragment_shader == 0){
        ERROR("create fragment shader failed");
        return program;
    }

    program = glCreateProgram();

    if(program){
        glAttachShader(program, vertex_shader);

        glAttachShader(program, fragment_shader);

        glLinkProgram(program);

        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

        glDetachShader(program, vertex_shader);
        glDetachShader(program, fragment_shader);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);

        if(linkStatus != GL_TRUE){
            GLint length = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

            if(length){
                char* buf = static_cast<char *>(malloc(length));

                glGetProgramInfoLog(program, length, nullptr, buf);
                ERROR("Link program (%d) failed: %s", program, buf);
                free(buf);
            }

            glDeleteProgram(program);
            program = 0;
        }
    }

    return program;
}

void GLUtils::DeleteProgram(GLuint program) {
    if(program){
        GLuint current = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, reinterpret_cast<GLint *>(&current));
        if(current == program){
            glUseProgram(0);
        }

        glDeleteProgram(program);

    }
}
