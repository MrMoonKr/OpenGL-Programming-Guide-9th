#ifndef __11_OIT_H__
#define __11_OIT_H__

#include "vapp.h"
#include "vbm.h"

class OITDemo : public VermilionApplication
{
private:
    // Member variables
    float aspect;

    // Head pointer image and PBO for clearing it
    GLuint  head_pointer_texture;

    GLuint  head_pointer_clear_buffer;

    // Atomic counter buffer
    GLuint  atomic_counter_buffer;

    // Linked list buffer
    GLuint  linked_list_buffer;

    GLuint  linked_list_texture;

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

#endif /* __11_OIT_H__ */
