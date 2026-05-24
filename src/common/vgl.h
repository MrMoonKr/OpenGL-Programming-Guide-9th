#ifndef __VGL_H__
#define __VGL_H__

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

#define GLFW_INCLUDE_NONE
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#define BUFFER_OFFSET(a) ((void*)(a))

#endif /* __VGL_H__ */
