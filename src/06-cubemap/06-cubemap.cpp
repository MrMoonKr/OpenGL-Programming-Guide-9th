/* $URL$
   $Rev$
   $Author$
   $Date$
   $Id$
 */

#include "vermilion.h"

#include "06-cubemap.h"
#include "vutils.h"
#include "vbm.h"

#include "vmath.h"

#include <stdio.h>

bool CubeMapExample::OnInitialize(const AppConfig& config)
{
    if (!VermilionApplication::OnInitialize(config))
    {
        return false;
    }
    skybox_prog = glCreateProgram();

    vglAttachShaderFile(skybox_prog, GL_VERTEX_SHADER, "media/shaders/cubemap/skybox.vs.glsl");
    vglAttachShaderFile(skybox_prog, GL_FRAGMENT_SHADER, "media/shaders/cubemap/skybox.fs.glsl");

    glLinkProgram(skybox_prog);

    object_prog = glCreateProgram();

    vglAttachShaderFile(object_prog, GL_VERTEX_SHADER, "media/shaders/cubemap/object.vs.glsl");
    vglAttachShaderFile(object_prog, GL_FRAGMENT_SHADER, "media/shaders/cubemap/object.fs.glsl");

    glLinkProgram(object_prog);

    glGenBuffers(1, &cube_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);

    static const GLfloat cube_vertices[] =
    {
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f
    };

    static const GLushort cube_indices[] =
    {
        0, 1, 2, 3, 6, 7, 4, 5,         // First strip
        2, 6, 0, 4, 1, 5, 3, 7          // Second strip
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &cube_element_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

    skybox_rotate_loc = glGetUniformLocation(skybox_prog, "tc_rotate");
    object_mat_mvp_loc = glGetUniformLocation(object_prog, "mat_mvp");
    object_mat_mv_loc = glGetUniformLocation(object_prog, "mat_mv");

    vglImageData image;

    tex = vglLoadTexture("media/TantolundenCube.dds", 0, &image);

    GLenum e;

    e = glGetError();

    vglUnloadImage(&image);

    object.LoadFromVBM("media/torus.vbm", 0, 1, 2);

    return true;
}

void CubeMapExample::OnDisplay()
{
    static const unsigned int start_time = GetAppTime();
    float t = float((GetAppTime() - start_time)) / float(0x3FFF);
    static const vmath::vec3 X(1.0f, 0.0f, 0.0f);
    static const vmath::vec3 Y(0.0f, 1.0f, 0.0f);
    static const vmath::vec3 Z(0.0f, 0.0f, 1.0f);

    vmath::mat4 tc_matrix(vmath::mat4::identity());

    glClearColor(0.0f, 0.25f, 0.3f, 1.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_CULL_FACE);
    glUseProgram(skybox_prog);

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    // tc_matrix = vmath::rotate(80.0f * 3.0f * t, Y);// * vmath::rotate(22.0f, Z);
    tc_matrix = vmath::perspective(35.0f, 1.0f / aspect, 0.1f, 500.0f) * tc_matrix;

    glUniformMatrix4fv(skybox_rotate_loc, 1, GL_FALSE, tc_matrix);
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_element_buffer);

    glDrawElements(GL_TRIANGLE_STRIP, 8, GL_UNSIGNED_SHORT, NULL);
    glDrawElements(GL_TRIANGLE_STRIP, 8, GL_UNSIGNED_SHORT, BUFFER_OFFSET(8 * sizeof(GLushort)));

    glUseProgram(object_prog);

    tc_matrix = vmath::translate(vmath::vec3(0.0f, 0.0f, -120.0f)) *
                vmath::rotate(80.0f * 3.0f * t, Y) * vmath::rotate(70.0f * 3.0f * t, Z);

    glUniformMatrix4fv(object_mat_mv_loc, 1, GL_FALSE, tc_matrix);
    tc_matrix = vmath::perspective(35.0f, 1.0f / aspect, 0.1f, 500.0f) * tc_matrix;
    glUniformMatrix4fv(object_mat_mvp_loc, 1, GL_FALSE, tc_matrix);

    glClear(GL_DEPTH_BUFFER_BIT);

    object.Render();

    VermilionApplication::OnDisplay();
}

void CubeMapExample::OnShutdown()
{
    glUseProgram(0);
    glDeleteProgram(skybox_prog);
    glDeleteProgram(object_prog);
    glDeleteTextures(1, &tex);
    glDeleteVertexArrays(1, &tex);
}

void CubeMapExample::OnResize(int width, int height)
{
    glViewport(0, 0 , width, height);

    aspect = float(height) / float(width);
}

int main(int argc, char** argv)
{
    CubeMapExample app;
    AppConfig config{};
    config.title = "Cube Map Example";
    return app.Run(config);
}

