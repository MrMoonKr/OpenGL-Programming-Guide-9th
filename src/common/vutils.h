#ifndef __VUTILS_H__
#define __VUTILS_H__

#include "vgl.h"

bool vglAttachShaderSource(GLuint prog, GLenum type, const char* source);
bool vglAttachShaderFile(GLuint prog, GLenum type, const char* filename);

#endif /* __VUTILS_H__ */
