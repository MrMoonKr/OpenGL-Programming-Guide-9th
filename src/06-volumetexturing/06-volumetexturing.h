#ifndef __06_VOLUMETEXTURING_H__
#define __06_VOLUMETEXTURING_H__

#include "vapp.h"

class VolumeTextureExample : public VermilionApplication
{
private:
    // Member variables
    float aspect;

    GLuint base_prog;

    GLuint vao;

    GLuint quad_vbo;

    GLuint tex;

    GLint tc_rotate_loc;
public:
    // Override functions from base class
    bool OnInitialize(const AppConfig& config) override;

    void OnDisplay() override;

    void OnShutdown() override;

    void OnResize(int width, int height) override;
};

#endif /* __06_VOLUMETEXTURING_H__ */
