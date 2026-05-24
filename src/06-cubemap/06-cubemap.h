#ifndef __06_CUBEMAP_H__
#define __06_CUBEMAP_H__

#include "vapp.h"
#include "vbm.h"

class CubeMapExample : public VermilionApplication
{
private:
    // Member variables
    float aspect;

    GLuint skybox_prog;

    GLuint object_prog;

    GLuint vao;

    GLuint cube_vbo;

    GLuint cube_element_buffer;

    GLuint tex;

    GLint skybox_rotate_loc;

    GLint object_mat_mvp_loc;

    GLint object_mat_mv_loc;

    VBObject object;
public:
    // Override functions from base class
    bool OnInitialize(const AppConfig& config) override;

    void OnDisplay() override;

    void OnShutdown() override;

    void OnResize(int width, int height) override;
};

#endif /* __06_CUBEMAP_H__ */
