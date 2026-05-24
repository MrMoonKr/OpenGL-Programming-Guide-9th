//////////////////////////////////////////////////////////////////////////////
//
//  Triangles.cpp
//
//////////////////////////////////////////////////////////////////////////////

#include <vermilion.h>

#include "vgl.h"
#include "01-keypress.h"
#include "LoadShaders.h"

enum VAO_IDs { Triangles, NumVAOs };
enum Buffer_IDs { ArrayBuffer, NumBuffers };
enum Attrib_IDs { vPosition = 0 };

GLuint  VAOs[NumVAOs];
GLuint  Buffers[NumBuffers];

const GLuint  NumVertices = 6;

//----------------------------------------------------------------------------
//
// init
//

bool KeyPressExample::OnInitialize(const AppConfig& config)
{
    if (!VermilionApplication::OnInitialize(config))
    {
        return false;
    }
    glGenVertexArrays( NumVAOs, VAOs );
    glBindVertexArray( VAOs[Triangles] );

    GLfloat  vertices[NumVertices][2] = {
        { -0.90f, -0.90f }, {  0.85f, -0.90f }, { -0.90f,  0.85f },  // Triangle 1
        {  0.90f, -0.85f }, {  0.90f,  0.90f }, { -0.85f,  0.90f }   // Triangle 2
    };

    glGenBuffers( NumBuffers, Buffers );
    glBindBuffer( GL_ARRAY_BUFFER, Buffers[ArrayBuffer] );
    glBufferData( GL_ARRAY_BUFFER, sizeof(vertices),
                  vertices, GL_STATIC_DRAW );

    ShaderInfo  shaders[] = {
        { GL_VERTEX_SHADER, "media/shaders/keypress/keypress.vert" },
        { GL_FRAGMENT_SHADER, "media/shaders/keypress/keypress.frag" },
        { GL_NONE, NULL }
    };

    GLuint program = LoadShaders( shaders );
    glUseProgram( program );

    glVertexAttribPointer( vPosition, 2, GL_FLOAT,
                           GL_FALSE, 0, BUFFER_OFFSET(0) );
    glEnableVertexAttribArray( vPosition );

    return true;
}

void KeyPressExample::OnKey(int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        switch (key)
        {
            case GLFW_KEY_M:
                {
                    static GLenum  mode = GL_FILL;

                    mode = ( mode == GL_FILL ? GL_LINE : GL_FILL );
                    glPolygonMode( GL_FRONT_AND_BACK, mode );
                }
                return;
        }
    }

    VermilionApplication::OnKey(key, scancode, action, mods);
}

//----------------------------------------------------------------------------
//
// display
//

void KeyPressExample::OnDisplay()
{
    glClear( GL_COLOR_BUFFER_BIT );

    glBindVertexArray( VAOs[Triangles] );
    glDrawArrays( GL_TRIANGLES, 0, NumVertices );

    VermilionApplication::OnDisplay();
}

void KeyPressExample::OnResize(int width, int height)
{
    glViewport(0, 0, width, height);
}

void KeyPressExample::OnShutdown()
{

}

int main(int argc, char** argv)
{
    KeyPressExample app;
    AppConfig config{};
    config.title = "Key Press Example";
    return app.Run(config);
}
