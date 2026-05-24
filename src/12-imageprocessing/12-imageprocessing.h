#ifndef __12_IMAGEPROCESSING_H__
#define __12_IMAGEPROCESSING_H__

#include "vapp.h"

class ImageProcessingComputeExample : public VermilionApplication
{
private:
    // Member variables
    GLuint  compute_prog;

    GLuint  compute_shader;

    // Texture to process
    GLuint  input_image;

    // Texture for compute shader to write into
    GLuint  intermediate_image;

    GLuint  output_image;

    // Program, vao and vbo to render a full screen quad
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

#endif /* __12_IMAGEPROCESSING_H__ */
