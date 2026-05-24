#ifndef __06_MIPMAP_H__
#define __06_MIPMAP_H__

#include "vapp.h"
#include "vbm.h"

class MipmapExample : public VermilionApplication
{
private:
    // Member variables
    float aspect;

    GLuint mipmap_prog;

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
private:
    virtual void Reshape(int width, int height);
};

#endif /* __06_MIPMAP_H__ */
