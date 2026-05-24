/* $URL$
   $Rev$
   $Author$
   $Date$
   $Id$
 */

// #define USE_GL3W
#include <vermilion.h>

#include "vgl.h"
#include "12-particlesimulator.h"
#include "vutils.h"
#include "vbm.h"

#include "vmath.h"

#include <stdio.h>

enum
{
    PARTICLE_GROUP_SIZE     = 1024,
    PARTICLE_GROUP_COUNT    = 8192,
    PARTICLE_COUNT          = (PARTICLE_GROUP_SIZE * PARTICLE_GROUP_COUNT)
};

static inline float random_float()
{
    float res;
    unsigned int tmp;
    static unsigned int seed = 0xFFFF0C59;

    seed *= 16807;

    tmp = seed ^ (seed >> 4) ^ (seed << 15);

    *((unsigned int *) &res) = (tmp >> 9) | 0x3F800000;

    return (res - 1.0f);
}

static vmath::vec3 random_vector(float minmag = 0.0f, float maxmag = 1.0f)
{
    vmath::vec3 randomvec(random_float() * 2.0f - 1.0f, random_float() * 2.0f - 1.0f, random_float() * 2.0f - 1.0f);
    randomvec = normalize(randomvec);
    randomvec *= (random_float() * (maxmag - minmag) + minmag);

    return randomvec;
}

bool ComputeParticleSimulator::OnInitialize(const AppConfig& config)
{
    if (!VermilionApplication::OnInitialize(config))
    {
        return false;
    }
    int i;

    // Initialize our compute program
    compute_prog = glCreateProgram();

    vglAttachShaderFile(compute_prog, GL_COMPUTE_SHADER, "media/shaders/particlesimulator/particlesimulator.cs.glsl");

    glLinkProgram(compute_prog);

    dt_location = glGetUniformLocation(compute_prog, "dt");

    glGenVertexArrays(1, &render_vao);
    glBindVertexArray(render_vao);

    glGenBuffers(2, buffers);
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glBufferData(GL_ARRAY_BUFFER, PARTICLE_COUNT * sizeof(vmath::vec4), NULL, GL_DYNAMIC_COPY);

    vmath::vec4 * positions = (vmath::vec4 *)glMapBufferRange(GL_ARRAY_BUFFER,
                                                              0,
                                                              PARTICLE_COUNT * sizeof(vmath::vec4),
                                                              GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

    for (i = 0; i < PARTICLE_COUNT; i++)
    {
        positions[i] = vmath::vec4(random_vector(-10.0f, 10.0f), random_float());
    }

    glUnmapBuffer(GL_ARRAY_BUFFER);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, velocity_buffer);
    glBufferData(GL_ARRAY_BUFFER, PARTICLE_COUNT * sizeof(vmath::vec4), NULL, GL_DYNAMIC_COPY);

    vmath::vec4 * velocities = (vmath::vec4 *)glMapBufferRange(GL_ARRAY_BUFFER,
                                                               0,
                                                               PARTICLE_COUNT * sizeof(vmath::vec4),
                                                               GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

    for (i = 0; i < PARTICLE_COUNT; i++)
    {
        velocities[i] = vmath::vec4(random_vector(-0.1f, 0.1f), 0.0f);
    }

    glUnmapBuffer(GL_ARRAY_BUFFER);

    glGenTextures(2, tbos);

    for (i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_BUFFER, tbos[i]);
        glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, buffers[i]);
    }

    glGenBuffers(1, &attractor_buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, attractor_buffer);
    glBufferData(GL_UNIFORM_BUFFER, 32 * sizeof(vmath::vec4), NULL, GL_STATIC_DRAW);

    for (i = 0; i < MAX_ATTRACTORS; i++)
    {
        attractor_masses[i] = 0.5f + random_float() * 0.5f;
    }

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, attractor_buffer);

    // Now create a simple program to visualize the result
    render_prog = glCreateProgram();

    vglAttachShaderFile(render_prog, GL_VERTEX_SHADER, "media/shaders/particlesimulator/render.vs.glsl");
    vglAttachShaderFile(render_prog, GL_FRAGMENT_SHADER, "media/shaders/particlesimulator/render.fs.glsl");

    glLinkProgram(render_prog);

    return true;
}

void ComputeParticleSimulator::OnDisplay()
{
    static const GLuint start_ticks = GetAppTime() - 100000;
    GLuint current_ticks = GetAppTime();
    static GLuint last_ticks = current_ticks;
    float time = ((start_ticks - current_ticks) & 0xFFFFF) / float(0xFFFFF);
    float delta_time = (float)(current_ticks - last_ticks) * 0.075f;

    if (delta_time < 0.01f)
    {
        return;
    }

    vmath::vec4 * attractors = (vmath::vec4 *)glMapBufferRange(GL_UNIFORM_BUFFER,
                                                               0,
                                                               32 * sizeof(vmath::vec4),
                                                               GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

    int i;

    for (i = 0; i < 32; i++)
    {
        attractors[i] = vmath::vec4(sinf(time * (float)(i + 4) * 7.5f * 20.0f) * 50.0f,
                                    cosf(time * (float)(i + 7) * 3.9f * 20.0f) * 50.0f,
                                    sinf(time * (float)(i + 3) * 5.3f * 20.0f) * cosf(time * (float)(i + 5) * 9.1f) * 100.0f,
                                    attractor_masses[i]);
    }

    glUnmapBuffer(GL_UNIFORM_BUFFER);

    // If dt is too large, the system could explode, so cap it to
    // some maximum allowed value
    if (delta_time >= 2.0f)
    {
        delta_time = 2.0f;
    }

    // Activate the compute program and bind the position and velocity buffers
    glUseProgram(compute_prog);
    glBindImageTexture(0, velocity_tbo, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    glBindImageTexture(1, position_tbo, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    // Set delta time
    glUniform1f(dt_location, delta_time);
    // Dispatch
    glDispatchCompute(PARTICLE_GROUP_COUNT, 1, 1);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    vmath::mat4 mvp = vmath::perspective(45.0f, aspect_ratio, 0.1f, 1000.0f) *
                      vmath::translate(0.0f, 0.0f, -160.0f) *
                      vmath::rotate(time * 1000.0f, vmath::vec3(0.0f, 1.0f, 0.0f));

    // Clear, select the rendering program and draw a full screen quad
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glUseProgram(render_prog);
    glUniformMatrix4fv(0, 1, GL_FALSE, mvp);
    glBindVertexArray(render_vao);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    // glPointSize(2.0f);
    glDrawArrays(GL_POINTS, 0, PARTICLE_COUNT);

    last_ticks = current_ticks;

    VermilionApplication::OnDisplay();
}

void ComputeParticleSimulator::OnShutdown()
{
    glUseProgram(0);
    glDeleteProgram(compute_prog);
    glDeleteProgram(render_prog);
    glDeleteVertexArrays(1, &render_vao);
}

void ComputeParticleSimulator::OnResize(int width, int height)
{
    glViewport(0, 0, width, height);
    aspect_ratio = (float)width / (float)height;
}

int main(int argc, char** argv)
{
    ComputeParticleSimulator app;
    AppConfig config{};
    config.title = "Compute Shader Particle System";
    return app.Run(config);
}
