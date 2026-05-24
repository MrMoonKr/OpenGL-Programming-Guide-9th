/*
 * Copyright 2012-2015 Graham Sellers
 *
 * This code is part of the OpenGL Programming Guide, 9th Edition.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

#include <vermilion.h>

#include "vgl.h"
#include "09-simpletess.h"
#include "mat.h"
#include "CheckError.h"
#include "LoadShaders.h"

GLuint  PLoc;  // Projection matrix

const GLfloat  zNear = 1.0;
const GLfloat  zFar  = 3.0;

const int  NumVertices = 4;  // vertices in our patch

//----------------------------------------------------------------------------

bool SimpleTssellationExample::OnInitialize(const AppConfig& config)
{
    if (!VermilionApplication::OnInitialize(config))
    {
        return false;
    }
    // Create a vertex array object
    enum { Patch, NumVAOs };
    GLuint VAOs[NumVAOs];
    glGenVertexArrays( NumVAOs, VAOs );
    glBindVertexArray( VAOs[Patch] );

    // Create and initialize a buffer object
    enum { Array, NumBuffers };
    GLuint buffers[NumBuffers];
    glGenBuffers( NumBuffers, buffers );
    glBindBuffer( GL_ARRAY_BUFFER, buffers[Array] );

    GLfloat vertices[NumVertices][2] = {
	{ -0.5, -0.5 },
	{  0.5, -0.5 },
	{  0.5,  0.5 },
	{ -0.5,  0.5 }
    };
    glBufferData( GL_ARRAY_BUFFER, sizeof(vertices),
		  vertices, GL_STATIC_DRAW );

    // Load shaders and use the resulting shader program
    ShaderInfo  shaders[] =
    {
        { GL_VERTEX_SHADER,          "media/shaders/simpletess/simple.vert" },
        { GL_TESS_CONTROL_SHADER,    "media/shaders/simpletess/simple.cont" },
        { GL_TESS_EVALUATION_SHADER, "media/shaders/simpletess/simple.eval" },
        { GL_FRAGMENT_SHADER,        "media/shaders/simpletess/simple.frag" },
        { GL_NONE, NULL }
    };

    GLuint program = LoadShaders( shaders );
    glUseProgram( program );

    // set up vertex arrays
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 2, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET(0) );

    PLoc = glGetUniformLocation( program, "P" );

    mat4  modelview = Translate( 0.0f, 0.0f, -0.5f * (zNear + zFar) ) *
	RotateX( -50.0 );
    glUniformMatrix4fv( glGetUniformLocation( program, "MV" ),
			1, GL_TRUE, modelview );

    glPatchParameteri( GL_PATCH_VERTICES, 4 );

    glEnable( GL_DEPTH_TEST );

    glClearColor( 0.0, 0.0, 0.0, 1.0 );

    return true;
}

//----------------------------------------------------------------------------

void SimpleTssellationExample::OnDisplay()
{
    mat4  projection = Perspective( 60.0, aspect, zNear, zFar );
    glUniformMatrix4fv( PLoc, 1, GL_TRUE, projection );

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glDrawArrays( GL_PATCHES, 0, NumVertices );

    VermilionApplication::OnDisplay();
}

//----------------------------------------------------------------------------

void SimpleTssellationExample::OnResize(int width, int height)
{
    glViewport( 0, 0, width, height );

    aspect = GLfloat(width) / height;
}

//----------------------------------------------------------------------------

void SimpleTssellationExample::OnShutdown()
{
}

void SimpleTssellationExample::OnKey(int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        switch (key)
        {
            case GLFW_KEY_M:
            {
                static GLenum mode = GL_FILL;

                mode = (mode == GL_FILL ? GL_LINE : GL_FILL);
                glPolygonMode(GL_FRONT_AND_BACK, mode);
                return;
            }
        }
    }

    VermilionApplication::OnKey(key, scancode, action, mods);
}

int main(int argc, char** argv)
{
    SimpleTssellationExample app;
    AppConfig config{};
    config.title = "Tessellation Example";
    return app.Run(config);
}

