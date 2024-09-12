//
// Created by zou on 2022/10/12.
//

#ifndef SKYMEDIAPLAYER_GLUTILS_H
#define SKYMEDIAPLAYER_GLUTILS_H

#include <GLES3/gl3.h>
#include "AndroidLog.h"

class GLUtils {
public:
    static GLuint LoadShader(GLenum type, const char* shader_source);

    static GLuint CreateProgram(const char* vertex_shader_source, const char* fragment_shader_source);

    static void DeleteProgram(GLuint program);
};


#endif //SKYMEDIAPLAYER_GLUTILS_H
