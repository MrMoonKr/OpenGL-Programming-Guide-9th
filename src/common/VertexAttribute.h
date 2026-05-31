#ifndef __VERTEX_ATTRIBUTE_H__
#define __VERTEX_ATTRIBUTE_H__

#include <cstddef>

#include "vgl.h"

struct VertexAttribute
{
    GLuint index = 0;
    GLint components = 0;
    GLenum type = GL_FLOAT;
    GLboolean normalized = GL_FALSE;
    GLsizei stride = 0;
    std::size_t offset = 0;
    GLuint divisor = 0;
    bool enabled = true;
    bool integer = false;
};

#endif /* __VERTEX_ATTRIBUTE_H__ */
