#ifndef __10_FUR_H__
#define __10_FUR_H__

#include "vapp.h"
#include "vbm.h"

class FurApplication : public VermilionApplication
{
private:
    // Member variables
    float aspect;

    GLuint base_prog;

    GLuint fur_prog;

    GLuint fur_texture;

    VBObject object;

    GLint fur_model_matrix_pos;

    GLint fur_projection_matrix_pos;

    GLint base_model_matrix_pos;

    GLint base_projection_matrix_pos;
public:
    // Override functions from base class
    bool OnInitialize(const AppConfig& config) override;

    void OnDisplay() override;

    void OnShutdown() override;

    void OnResize(int width, int height) override;
};

#endif /* __10_FUR_H__ */
