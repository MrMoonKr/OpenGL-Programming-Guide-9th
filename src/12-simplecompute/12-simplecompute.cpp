/* $URL$
   $Rev$
   $Author$
   $Date$
   $Id$
 */

// #define USE_GL3W
#include <vermilion.h>

#include "12-simplecompute.h"
#include "vutils.h"
#include "vbm.h"

#include "vmath.h"

#include <stdio.h>

bool SimpleComputeShaderExample::OnInitialize(const AppConfig& config)
{
    if (!VermilionApplication::OnInitialize(config))
    {
        return false;
    }
    // Initialize our compute program
    compute_prog = glCreateProgram();

    vglAttachShaderFile(compute_prog, GL_COMPUTE_SHADER, "media/shaders/simplecompute/simplecompute.cs.glsl");

    glLinkProgram(compute_prog);

    // This is the texture that the compute program will write into
    glGenTextures(1, &output_image);
    glBindTexture(GL_TEXTURE_2D, output_image);
    glTexStorage2D(GL_TEXTURE_2D, 8, GL_RGBA32F, 256, 256);

    // Now create a simple program to visualize the result
    render_prog = glCreateProgram();

    vglAttachShaderFile(render_prog, GL_VERTEX_SHADER, "media/shaders/simplecompute/render.vs.glsl");
    vglAttachShaderFile(render_prog, GL_FRAGMENT_SHADER, "media/shaders/simplecompute/render.fs.glsl");

    glLinkProgram(render_prog);

    // This is the VAO containing the data to draw the quad (including its associated VBO)
    glGenVertexArrays(1, &render_vao);
    glBindVertexArray(render_vao);
    glEnableVertexAttribArray(0);
    glGenBuffers(1, &render_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, render_vbo);
    static const float verts[] =
    {
        -1.0f, -1.0f, 0.5f, 1.0f,
         1.0f, -1.0f, 0.5f, 1.0f,
         1.0f,  1.0f, 0.5f, 1.0f,
        -1.0f,  1.0f, 0.5f, 1.0f,
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);

    return true;
}

void SimpleComputeShaderExample::OnDisplay()
{
    // Activate the compute program and bind the output texture image
    glUseProgram(compute_prog);
    glBindImageTexture(0, output_image, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glDispatchCompute(8, 16, 1);

    // Now bind the texture for rendering _from_
    glBindTexture(GL_TEXTURE_2D, output_image);

    // Clear, select the rendering program and draw a full screen quad
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(render_prog);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    VermilionApplication::OnDisplay();
}

void SimpleComputeShaderExample::OnShutdown()
{
    glUseProgram(0);
    glDeleteProgram(compute_prog);
    glDeleteProgram(render_prog);
    glDeleteTextures(1, &output_image);
    glDeleteVertexArrays(1, &render_vao);
}

void SimpleComputeShaderExample::OnResize(int width, int height)
{
    glViewport(0, 0, width, height);
}

int main(int argc, char** argv)
{
    SimpleComputeShaderExample app;
    AppConfig config{};
    config.title = "Simple Compute Shader Example";
    return app.Run(config);
}

