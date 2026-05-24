#ifndef __12_RAYTRACER_H__
#define __12_RAYTRACER_H__

#include "vapp.h"

class RayTracingExample : public VermilionApplication
{
private:
    // Compute programs
    GLuint  initializer_prog;

    GLuint  trace_prog;

    // Texture for compute shader to write into
    GLuint  output_image;

    // Ray buffer
    GLuint  ray_buffer[2];

    // Append count buffer
    GLuint  counter_buffer;

    // Program, vao and vbo to render a quad
    GLuint  render_prog;

    GLuint  render_vao;

    GLuint  render_vbo;
public:
    // Override functions from base class
    bool OnInitialize(const AppConfig& config) override;

    void OnDisplay() override;

    void OnShutdown() override;

    void OnResize(int width, int height) override;
};

#endif /* __12_RAYTRACER_H__ */
