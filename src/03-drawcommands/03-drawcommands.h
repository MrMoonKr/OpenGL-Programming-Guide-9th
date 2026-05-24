#ifndef __03_DRAWCOMMANDS_H__
#define __03_DRAWCOMMANDS_H__

#include "vapp.h"

class DrawCommandExample : public VermilionApplication
{
private:
    // Member variables
    float aspect;

    GLuint render_prog;

    GLuint vao[1];

    GLuint vbo[1];

    GLuint ebo[1];

    GLint render_model_matrix_loc;

    GLint render_projection_matrix_loc;
public:
    // Override functions from base class
    bool OnInitialize(const AppConfig& config) override;

    void OnDisplay() override;

    void OnShutdown() override;

    void OnResize(int width, int height) override;
};

#endif /* __03_DRAWCOMMANDS_H__ */
