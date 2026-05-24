#ifndef __04_SHADOWMAP_H__
#define __04_SHADOWMAP_H__

#include "vapp.h"
#include "vbm.h"

class ShadowMapExample : public VermilionApplication
{
private:
    // Member variables
    float aspect;

    // Program to render from the light's position
    GLuint render_light_prog;

    struct
    {
    GLint model_view_projection_matrix;
    } render_light_uniforms;

    // FBO to render depth with
    GLuint  depth_fbo;

    GLuint  depth_texture;

    // Program to render the scene from the viewer's position
    GLuint render_scene_prog;

    struct
    {
    GLint model_matrix;
    GLint view_matrix;
    GLint projection_matrix;
    GLint shadow_matrix;
    GLint light_position;
    GLint material_ambient;
    GLint material_diffuse;
    GLint material_specular;
    GLint material_specular_power;
    } render_scene_uniforms;

    // Ground plane
    GLuint  ground_vbo;

    GLuint  ground_vao;

    VBObject object;

    GLint current_width;

    GLint current_height;
public:
    // Override functions from base class
    bool OnInitialize(const AppConfig& config) override;

    void OnDisplay() override;

    void OnShutdown() override;

    void OnResize(int width, int height) override;
private:
    void DrawScene(bool depth_only = false);
};

#endif /* __04_SHADOWMAP_H__ */
