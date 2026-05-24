#ifndef __10_VIEWPORT_ARRAY_H__
#define __10_VIEWPORT_ARRAY_H__

#include "vapp.h"
#include "vbm.h"

class ViewportArrayApplication : public VermilionApplication
{
private:
    // Member variables
    float aspect;

    GLuint prog;

    GLuint vao;

    GLuint vbo;

    VBObject object;

    GLint model_matrix_pos;

    GLint projection_matrix_pos;
public:
    // Override functions from base class
    bool OnInitialize(const AppConfig& config) override;

    void OnDisplay() override;

    void OnShutdown() override;

    void OnResize(int width, int height) override;
};

#endif /* __10_VIEWPORT_ARRAY_H__ */
