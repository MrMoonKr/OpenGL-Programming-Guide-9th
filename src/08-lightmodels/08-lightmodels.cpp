/* $URL$
   $Rev$
   $Author$
   $Date$
   $Id$
 */

// #define USE_GL3W
#include <vermilion.h>

#include "08-lightmodels.h"
#include "vutils.h"
#include "vbm.h"

#include "vmath.h"

#include <stdio.h>

bool LightingExample::OnInitialize(const AppConfig& config)
{
    if (!VermilionApplication::OnInitialize(config))
    {
        return false;
    }
    // Now create a simple program to visualize the result
    render_prog = glCreateProgram();

    vglAttachShaderFile(render_prog, GL_VERTEX_SHADER, "media/shaders/lightmodels/lightmodels.vs.glsl");
    vglAttachShaderFile(render_prog, GL_FRAGMENT_SHADER, "media/shaders/lightmodels/lightmodels.fs.glsl");

    glLinkProgram(render_prog);

    mv_mat_loc = glGetUniformLocation(render_prog, "model_matrix");
    prj_mat_loc = glGetUniformLocation(render_prog, "proj_matrix");
    col_amb_loc = glGetUniformLocation(render_prog, "color_ambient");
    col_diff_loc = glGetUniformLocation(render_prog, "color_diffuse");
    col_spec_loc = glGetUniformLocation(render_prog, "color_specular");

    object.LoadFromVBM("media/torus.vbm", 0, 1, 2);

    return true;
}

void LightingExample::OnDisplay()
{
    float time = float(GetAppTime() & 0xFFFF) / float(0xFFFF);

    vmath::mat4 mv_matrix = vmath::translate(0.0f, 0.0f, -60.0f) *
                            vmath::rotate(987.0f * time * 3.14159f, vmath::vec3(0.0f, 0.0f, 1.0f)) *
                            vmath::rotate(1234.0f * time * 3.14159f, vmath::vec3(1.0f, 0.0f, 0.0f));
    vmath::mat4 prj_matrix = vmath::perspective(60.0f, 1.333f, 0.1f, 1000.0f);

    glUseProgram(render_prog);

    glUniformMatrix4fv(mv_mat_loc, 1, GL_FALSE, mv_matrix);
    glUniformMatrix4fv(prj_mat_loc, 1, GL_FALSE, prj_matrix);

    // Clear, select the rendering program and draw a full screen quad
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    object.Render();

    VermilionApplication::OnDisplay();
}

void LightingExample::OnShutdown()
{
    glUseProgram(0);
    glDeleteProgram(render_prog);
    glDeleteTextures(1, &output_image);
}

void LightingExample::OnResize(int width, int height)
{
    glViewport(0, 0, width, height);
}

int main(int argc, char** argv)
{
    LightingExample app;
    AppConfig config{};
    config.title = "Lighting Example";
    return app.Run(config);
}

