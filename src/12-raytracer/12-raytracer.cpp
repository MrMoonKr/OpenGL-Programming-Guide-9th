/* $URL$
   $Rev$
   $Author$
   $Date$
   $Id$
 */

// #define USE_GL3W
#include <vermilion.h>

#include <vgl.h>
#include "12-raytracer.h"
#include "vutils.h"
#include "vbm.h"

#include "vmath.h"

#include <stdio.h>

#define OUTPUT_LODS         9
#define OUTPUT_SIZE_X       (1 << OUTPUT_LODS)
#define OUTPUT_SIZE_Y       (1 << OUTPUT_LODS)

bool RayTracingExample::OnInitialize(const AppConfig& config)
{
    if (!VermilionApplication::OnInitialize(config))
    {
        return false;
    }
    // Buffers
    glGenBuffers(2, ray_buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ray_buffer[0]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 64 * 1024 * 1024, NULL, GL_DYNAMIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ray_buffer[1]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 64 * 1024 * 1024, NULL, GL_DYNAMIC_COPY);

    static const GLuint zeros[] = { 0, 0 };

    glGenBuffers(1, &counter_buffer);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, counter_buffer);
    glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(zeros), zeros, GL_DYNAMIC_COPY);

    // Initialize our compute program
    initializer_prog = glCreateProgram();

    vglAttachShaderFile(initializer_prog, GL_COMPUTE_SHADER, "media/shaders/raytracer/init.cs.glsl");

    glLinkProgram(initializer_prog);

    
    
    // Initialize our compute program
    trace_prog = glCreateProgram();

    vglAttachShaderFile(trace_prog, GL_COMPUTE_SHADER, "media/shaders/raytracer/trace.cs.glsl");

    glLinkProgram(trace_prog);

    // This is the texture that the compute program will write into
    glGenTextures(1, &output_image);
    glBindTexture(GL_TEXTURE_2D, output_image);
    glTexStorage2D(GL_TEXTURE_2D, OUTPUT_LODS, GL_RGBA32F, OUTPUT_SIZE_X, OUTPUT_SIZE_Y);

    // Now create a simple program to visualize the result
    render_prog = glCreateProgram();

    vglAttachShaderFile(render_prog, GL_VERTEX_SHADER, "media/shaders/raytracer/render.vs.glsl");
    vglAttachShaderFile(render_prog, GL_FRAGMENT_SHADER, "media/shaders/raytracer/render.fs.glsl");

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

#pragma pack (push, 1)
struct RAY
{
    vmath::ivec4    screen_origin;
    vmath::vec4     world_origin;
    vmath::vec4     world_direction;
};
#pragma pack (pop)

void RayTracingExample::OnDisplay()
{
    // Activate the initialization compute program
    glUseProgram(initializer_prog);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ray_buffer[0]);
    glDispatchCompute(OUTPUT_SIZE_X / 16, OUTPUT_SIZE_Y / 16, 1);

    // Reset atomic counters
    static const GLuint zeros[] = { 0, 0 };

    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, counter_buffer);
    glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(zeros), zeros, GL_DYNAMIC_COPY);

    // 
    glBindImageTexture(0, output_image, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    // Bind ray buffers
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ray_buffer[0]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ray_buffer[1]);

    // Bind the trace program
    glUseProgram(trace_prog);
    glDispatchCompute(OUTPUT_SIZE_X / 16, OUTPUT_SIZE_Y / 16, 1);

    vmath::ivec2 * ptr = (vmath::ivec2 *)glMapBuffer(GL_ATOMIC_COUNTER_BUFFER, GL_READ_ONLY);

    glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);

    // Now bind the texture for rendering _from_
    glBindTexture(GL_TEXTURE_2D, output_image);

    // Clear, select the rendering program and draw a full screen quad
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(render_prog);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    VermilionApplication::OnDisplay();
}

void RayTracingExample::OnShutdown()
{
    glUseProgram(0);
    glDeleteProgram(initializer_prog);
    glDeleteProgram(render_prog);
    glDeleteTextures(1, &output_image);
    glDeleteVertexArrays(1, &render_vao);
}

void RayTracingExample::OnResize(int width, int height)
{
    glViewport(0, 0, width, height);
}

int main(int argc, char** argv)
{
    RayTracingExample app;
    AppConfig config{};
    config.title = "Compute Shader Raytracing Example";
    return app.Run(config);
}

