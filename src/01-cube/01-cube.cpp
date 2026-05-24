#include "01-cube.h"

#include <array>
#include <cstdio>

#include "vmath.h"
#include "vutils.h"

namespace
{
struct Vertex
{
    GLfloat position[3];
    GLfloat color[3];
};

constexpr std::array<Vertex, 8> kVertices = {{
    {{ -0.5f, -0.5f, -0.5f }, { 0.05f, 0.10f, 0.20f }},
    {{  0.5f, -0.5f, -0.5f }, { 0.95f, 0.20f, 0.15f }},
    {{  0.5f,  0.5f, -0.5f }, { 0.95f, 0.85f, 0.20f }},
    {{ -0.5f,  0.5f, -0.5f }, { 0.20f, 0.80f, 0.25f }},
    {{ -0.5f, -0.5f,  0.5f }, { 0.20f, 0.35f, 0.95f }},
    {{  0.5f, -0.5f,  0.5f }, { 0.90f, 0.25f, 0.90f }},
    {{  0.5f,  0.5f,  0.5f }, { 0.95f, 0.95f, 0.95f }},
    {{ -0.5f,  0.5f,  0.5f }, { 0.20f, 0.90f, 0.95f }}
}};

constexpr std::array<GLushort, 36> kIndices = {{
    0, 1, 2, 2, 3, 0,
    4, 7, 6, 6, 5, 4,
    0, 3, 7, 7, 4, 0,
    1, 5, 6, 6, 2, 1,
    3, 2, 6, 6, 7, 3,
    0, 4, 5, 5, 1, 0
}};

constexpr const char* kVertexShaderSource = R"(#version 460 core
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_color;

uniform mat4 u_mvp;

out vec3 v_color;

void main()
{
    gl_Position = u_mvp * vec4(a_position, 1.0);
    v_color = a_color;
}
)";

constexpr const char* kFragmentShaderSource = R"(#version 460 core
in vec3 v_color;

out vec4 fragColor;

void main()
{
    fragColor = vec4(v_color, 1.0);
}
)";

bool LinkProgram(GLuint program)
{
    glLinkProgram(program);

    GLint linked = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (linked == GL_FALSE)
    {
        GLint logLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

        std::string log(static_cast<size_t>(logLength > 0 ? logLength : 1), '\0');
        glGetProgramInfoLog(program,
                            static_cast<GLsizei>(log.size()),
                            nullptr,
                            log.data());

        std::fprintf(stderr, "Program link failed: %s\n", log.c_str());
        return false;
    }

    return true;
}
} // namespace

bool CubeExample::OnInitialize(const AppConfig& config)
{
    if (!VermilionApplication::OnInitialize(config))
    {
        return false;
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    m_program = glCreateProgram();
    if (!vglAttachShaderSource(m_program, GL_VERTEX_SHADER, kVertexShaderSource) ||
        !vglAttachShaderSource(m_program, GL_FRAGMENT_SHADER, kFragmentShaderSource) ||
        !LinkProgram(m_program))
    {
        return false;
    }

    m_mvpLocation = glGetUniformLocation(m_program, "u_mvp");

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(sizeof(kVertices)),
                 kVertices.data(),
                 GL_STATIC_DRAW);

    glGenBuffers(1, &m_indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(sizeof(kIndices)),
                 kIndices.data(),
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(Vertex),
                          BUFFER_OFFSET(offsetof(Vertex, position)));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(Vertex),
                          BUFFER_OFFSET(offsetof(Vertex, color)));
    glEnableVertexAttribArray(1);

    return true;
}

void CubeExample::OnUpdate(float deltaTime)
{
    m_rotationTime += deltaTime;
}

void CubeExample::OnDisplay()
{
    // v_clip = P * V * M * v_local

    const vmath::mat4 model =
        vmath::translate(0.0f, 0.0f, -5.0f) *
        vmath::rotate(m_rotationTime * 35.0f, 1.0f, 0.0f, 0.0f) *
        vmath::rotate(m_rotationTime * 50.0f, 0.0f, 1.0f, 0.0f);

    const vmath::mat4 projection = vmath::perspective(45.0f, m_aspect, 0.1f, 100000.0f);
    const vmath::mat4 mvp = projection * model;

    glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_program);
    glUniformMatrix4fv(m_mvpLocation, 1, GL_FALSE, mvp);

    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES,
                   static_cast<GLsizei>(kIndices.size()),
                   GL_UNSIGNED_SHORT,
                   nullptr);

    VermilionApplication::OnDisplay();
}

void CubeExample::OnShutdown()
{
    glDeleteBuffers(1, &m_indexBuffer);
    glDeleteBuffers(1, &m_vertexBuffer);
    glDeleteVertexArrays(1, &m_vao);
    glDeleteProgram(m_program);

    m_indexBuffer = 0;
    m_vertexBuffer = 0;
    m_vao = 0;
    m_program = 0;
    m_mvpLocation = -1;
}

void CubeExample::OnResize(int width, int height)
{
    glViewport(0, 0, width, height);
    m_aspect = (height > 0) ? static_cast<float>(width) / static_cast<float>(height) : 1.0f;
}

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    CubeExample app;
    AppConfig config{};
    config.title = "01 Cube";
    return app.Run(config);
}
