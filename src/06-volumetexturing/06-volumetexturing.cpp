/* $URL$
   $Rev$
   $Author$
   $Date$
   $Id$
 */

#define USE_GL3W
#include <vermilion.h>

#include "06-volumetexturing.h"
#include "vutils.h"
#include "vbm.h"

#include "vmath.h"

#include <stdio.h>

bool VolumeTextureExample::OnInitialize(const AppConfig& config)
{
    if (!VermilionApplication::OnInitialize(config))
    {
        return false;
    }
    base_prog = glCreateProgram();

    vglAttachShaderFile(base_prog, GL_VERTEX_SHADER, "media/shaders/volumetexturing/volumetexturing.vs.glsl");
    vglAttachShaderFile(base_prog, GL_FRAGMENT_SHADER, "media/shaders/volumetexturing/volumetexturing.fs.glsl");

    glGenBuffers(1, &quad_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);

    static const GLfloat quad_data[] =
    {
         1.0f, -1.0f,
        -1.0f, -1.0f,
        -1.0f, 1.0f,
         1.0f, 1.0f,

         0.0f, 0.0f,
         1.0f, 0.0f,
         1.0f, 1.0f,
         0.0f, 1.0f
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_data), quad_data, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(8 * sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glLinkProgram(base_prog);

    char buf[1024];
    glGetProgramInfoLog(base_prog, 1024, NULL, buf);

    tc_rotate_loc = glGetUniformLocation(base_prog, "tc_rotate");

    vglImageData image;

    tex = vglLoadTexture("media/cloud.dds", 0, &image);

    glTexParameteri(image.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    vglUnloadImage(&image);

    return true;
}

void VolumeTextureExample::OnDisplay()
{
    float t = float(GetAppTime()) / float(0x3FFF);
    static const vmath::vec3 X(1.0f, 0.0f, 0.0f);
    static const vmath::vec3 Y(0.0f, 1.0f, 0.0f);
    static const vmath::vec3 Z(0.0f, 0.0f, 1.0f);

    vmath::mat4 tc_matrix(vmath::mat4::identity());

    glClearColor(0.6f, 0.7f, 0.9f, 1.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_CULL_FACE);
    glUseProgram(base_prog);

    tc_matrix = vmath::rotate(t * 170.0f, X) *
                vmath::rotate(t * 137.0f, Y) *
                vmath::rotate(t * 93.0f, Z);

    glUniformMatrix4fv(tc_rotate_loc, 1, GL_FALSE, tc_matrix);
    glBindVertexArray(vao);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisable(GL_BLEND);

    VermilionApplication::OnDisplay();
}

void VolumeTextureExample::OnShutdown()
{
    glUseProgram(0);
    glDeleteProgram(base_prog);
    glDeleteTextures(1, &tex);
    glDeleteVertexArrays(1, &tex);
}

void VolumeTextureExample::OnResize(int width, int height)
{
    glViewport(0, 0 , width, height);

    aspect = float(height) / float(width);
}

int main(int argc, char** argv)
{
    VolumeTextureExample app;
    AppConfig config{};
    config.title = "Volume Texture Example";
    return app.Run(config);
}

