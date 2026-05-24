#ifndef __06_MULTITEXTURE_H__
#define __06_MULTITEXTURE_H__

#include "vapp.h"

class MultiTextureExample : public VermilionApplication
{
private:
    // Member variables
    float aspect;

    GLuint base_prog;

    GLuint vao;

    GLuint quad_vbo;

    GLuint tex1;

    GLuint tex2;

    GLint  time_loc;
public:
    // Override functions from base class
    bool OnInitialize(const AppConfig& config) override;

    void OnDisplay() override;

    void OnShutdown() override;

    void OnResize(int width, int height) override;
};

#endif /* __06_MULTITEXTURE_H__ */
