#ifndef __03_INSTANCING2_H__
#define __03_INSTANCING2_H__

#include "vapp.h"
#include "vbm.h"

class InstancingExample : public VermilionApplication
{
private:
    // Member variables
    float aspect;

    GLuint color_buffer;

    GLuint model_matrix_buffer;

    GLuint render_prog;

    GLint model_matrix_loc;

    GLint view_matrix_loc;

    GLint projection_matrix_loc;

    VBObject object;
public:
    // Override functions from base class
    bool OnInitialize(const AppConfig& config) override;

    void OnDisplay() override;

    void OnShutdown() override;

    void OnResize(int width, int height) override;
};

#endif /* __03_INSTANCING2_H__ */
