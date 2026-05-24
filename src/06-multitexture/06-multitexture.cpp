/* $URL$
   $Rev$
   $Author$
   $Date$
   $Id$
 */

#define USE_GL3W
#include <vermilion.h>

#include "06-multitexture.h"
#include "vutils.h"
#include "vbm.h"

#include "vmath.h"

#include <stdio.h>

bool MultiTextureExample::OnInitialize(const AppConfig& config)
{
    if (!VermilionApplication::OnInitialize(config))
    {
        return false;
    }
    base_prog = glCreateProgram();

    vglAttachShaderFile(base_prog, GL_VERTEX_SHADER, "media/shaders/multitexture/multitexture.vs.glsl");
    vglAttachShaderFile(base_prog, GL_FRAGMENT_SHADER, "media/shaders/multitexture/multitexture.fs.glsl");

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

    glUseProgram(base_prog);

    time_loc = glGetUniformLocation(base_prog, "time");
    glUniform1i(glGetUniformLocation(base_prog, "tex1"), 0);
    glUniform1i(glGetUniformLocation(base_prog, "tex2"), 1);

    vglImageData image;

    tex1 = vglLoadTexture("media/test.dds", 0, &image);

    glBindTexture(image.target, tex1);
    glTexParameteri(image.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    vglUnloadImage(&image);

    tex2 = vglLoadTexture("media/test3.dds", 0, &image);

    vglUnloadImage(&image);

    return true;
}

void MultiTextureExample::OnDisplay()
{
    float t = float(GetAppTime() & 0x3FFF) / float(0x3FFF);
    static const vmath::vec3 X(1.0f, 0.0f, 0.0f);
    static const vmath::vec3 Y(0.0f, 1.0f, 0.0f);
    static const vmath::vec3 Z(0.0f, 0.0f, 1.0f);

    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_CULL_FACE);
    glUseProgram(base_prog);

    glUniform1f(time_loc, t);

    glBindVertexArray(vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex2);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    VermilionApplication::OnDisplay();
}

void MultiTextureExample::OnShutdown()
{
    glUseProgram(0);
    glDeleteProgram(base_prog);
    glDeleteTextures(1, &tex1);
    glDeleteTextures(2, &tex2);
    glDeleteVertexArrays(1, &vao);
}

void MultiTextureExample::OnResize(int width, int height)
{
    glViewport(0, 0 , width, height);

    aspect = float(height) / float(width);
}

int main(int argc, char** argv)
{
    MultiTextureExample app;
    AppConfig config{};
    config.title = "Simple Multi-Texture Example";
    return app.Run(config);
}

