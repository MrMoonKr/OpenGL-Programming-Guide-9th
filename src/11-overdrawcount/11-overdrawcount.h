#ifndef __11_OVERDRAWCOUNT_H__
#define __11_OVERDRAWCOUNT_H__

#include "vapp.h"
#include "vbm.h"

class OverdrawCountExample : public VermilionApplication
{
private:
    // Member variables
    float aspect;

    // Color palette buffer texture
    GLuint  image_palette_buffer;

    GLuint  image_palette_texture;

    // Output image and PBO for clearing it
    GLuint  overdraw_count_buffer;

    GLuint  overdraw_count_clear_buffer;

    // Program to render the scene
    GLuint render_scene_prog;

    struct
    {
    GLint aspect;
    GLint time;
    GLint model_matrix;
    GLint view_matrix;
    GLint projection_matrix;
    } render_scene_uniforms;

    // Program to resolve
    GLuint resolve_program;

    // Full Screen Quad
    GLuint  quad_vbo;

    GLuint  quad_vao;

    GLint current_width;

    GLint current_height;

    VBObject object;

    // Program to construct the linked list (renders the transparent objects)
    GLuint  list_build_program;
public:
    // Override functions from base class
    bool OnInitialize(const AppConfig& config) override;

    void OnDisplay() override;

    void OnShutdown() override;

    void OnResize(int width, int height) override;

    void OnKey(int key, int scancode, int action, int mods) override;
private:
    void DrawScene(void);

    void InitPrograms(void);
};

#endif /* __11_OVERDRAWCOUNT_H__ */
