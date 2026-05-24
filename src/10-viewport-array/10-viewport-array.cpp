/* $URL$
   $Rev$
   $Author$
   $Date$
   $Id$
 */

#include "10-viewport-array.h"
#include "vutils.h"
#include "vbm.h"

#include "vmath.h"

#include <stdio.h>

bool ViewportArrayApplication::OnInitialize(const AppConfig& config)
{
    if (!VermilionApplication::OnInitialize(config))
    {
        return false;
    }
    prog = glCreateProgram();

    vglAttachShaderFile(prog, GL_VERTEX_SHADER, "media/shaders/viewport-array/viewport-array.vs.glsl");
    vglAttachShaderFile(prog, GL_GEOMETRY_SHADER, "media/shaders/viewport-array/viewport-array.gs.glsl");
    vglAttachShaderFile(prog, GL_FRAGMENT_SHADER, "media/shaders/viewport-array/viewport-array.fs.glsl");

    glLinkProgram(prog);
    glUseProgram(prog);

    model_matrix_pos = glGetUniformLocation(prog, "model_matrix");
    projection_matrix_pos = glGetUniformLocation(prog, "projection_matrix");

    object.LoadFromVBM("media/ninja.vbm", 0, 1, 2);

    return true;
}

void ViewportArrayApplication::OnDisplay()
{
    float t = float(GetAppTime() & 0x3FFF) / float(0x3FFF);
    static const vmath::vec3 X(1.0f, 0.0f, 0.0f);
    static const vmath::vec3 Y(0.0f, 1.0f, 0.0f);
    static const vmath::vec3 Z(0.0f, 0.0f, 1.0f);

    glClearColor( 0.3f, 0.1f, 0.2f, 1.0f );
    glClearDepth( 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glUseProgram(prog);

    vmath::mat4 p(vmath::frustum(-1.0f, 1.0f, aspect, -aspect, 1.0f, 5000.0f));
    vmath::mat4 m[4];

    for (int i = 0; i < 4; i++)
    {
        m[i] = vmath::mat4(vmath::translate(0.0f, 0.0f, 100.0f * sinf(6.28318531f * t + i) - 230.0f) *
                           vmath::rotate(360.0f * t * float(i + 1), X) *
                           vmath::rotate(360.0f * t * float(i + 2), Y) *
                           vmath::rotate(360.0f * t * float(5 - i), Z) *
                           vmath::translate(0.0f, -80.0f, 0.0f));
    }

    glUniformMatrix4fv(model_matrix_pos, 4, GL_FALSE, m[0]);
    glUniformMatrix4fv(projection_matrix_pos, 1, GL_FALSE, p);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    object.Render();

    VermilionApplication::OnDisplay();
}

void ViewportArrayApplication::OnShutdown()
{
    glUseProgram(0);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(prog);
    glDeleteVertexArrays(1, &vao);
}

void ViewportArrayApplication::OnResize(int width, int height)
{
    const float wot = float(width) * 0.5f;
    const float hot = float(height) * 0.5f;

    glViewportIndexedf(0, 0.0f, 0.0f, wot, hot);
    glViewportIndexedf(1, wot, 0.0f, wot, hot);
    glViewportIndexedf(2, 0.0f, hot, wot, hot);
    glViewportIndexedf(3, wot, hot, wot, hot);

    aspect = hot / wot;
}

int main(int argc, char** argv)
{
    ViewportArrayApplication app;
    AppConfig config{};
    config.title = "Viewport Array";
    return app.Run(config);
}

