#ifndef __08_LIGHTMODELS_H__
#define __08_LIGHTMODELS_H__

#include "vapp.h"
#include "vbm.h"

class LightingExample : public VermilionApplication
{
private:
    // Texture for compute shader to write into
    GLuint  output_image;

    // Program, vao and vbo to render a full screen quad
    GLuint  render_prog;

    // Uniform locations
    GLint   mv_mat_loc;

    GLint   prj_mat_loc;

    GLint   col_amb_loc;

    GLint   col_diff_loc;

    GLint   col_spec_loc;

    // Object to render
    VBObject    object;
public:
    // Override functions from base class
    bool OnInitialize(const AppConfig& config) override;

    void OnDisplay() override;

    void OnShutdown() override;

    void OnResize(int width, int height) override;
};

#endif /* __08_LIGHTMODELS_H__ */
