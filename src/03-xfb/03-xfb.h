#ifndef __03_XFB_H__
#define __03_XFB_H__

#include "vapp.h"
#include "vbm.h"

class TransformFeedbackExample : public VermilionApplication
{
private:
    // Member variables
    float aspect;

    GLuint update_prog;

    GLuint vao[2];

    GLuint vbo[2];

    GLuint xfb;

    GLuint render_prog;

    GLuint geometry_vbo;

    GLuint render_vao;

    GLint render_model_matrix_loc;

    GLint render_projection_matrix_loc;

    GLuint geometry_tex;

    GLuint geometry_xfb;

    GLuint particle_xfb;

    GLint model_matrix_loc;

    GLint projection_matrix_loc;

    GLint triangle_count_loc;

    GLint time_step_loc;

    VBObject object;
public:
    // Override functions from base class
    bool OnInitialize(const AppConfig& config) override;

    void OnDisplay() override;

    void OnShutdown() override;

    void OnResize(int width, int height) override;
};

#endif /* __03_XFB_H__ */
