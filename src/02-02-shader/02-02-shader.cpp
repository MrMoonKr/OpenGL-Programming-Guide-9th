#include "02-02-shader.h"

#include <array>
#include <cstddef>
#include <cstdio>
#include <vector>

namespace
{
struct Vertex
{
    GLfloat position[3];
    GLfloat color[3];
};

constexpr std::array<Vertex, 8> kCubeVertices = {{
    {{ -0.5f, -0.5f, -0.5f }, { 0.92f, 0.36f, 0.32f }},
    {{  0.5f, -0.5f, -0.5f }, { 0.97f, 0.64f, 0.20f }},
    {{  0.5f,  0.5f, -0.5f }, { 0.99f, 0.91f, 0.36f }},
    {{ -0.5f,  0.5f, -0.5f }, { 0.42f, 0.80f, 0.34f }},
    {{ -0.5f, -0.5f,  0.5f }, { 0.21f, 0.53f, 0.95f }},
    {{  0.5f, -0.5f,  0.5f }, { 0.73f, 0.34f, 0.92f }},
    {{  0.5f,  0.5f,  0.5f }, { 0.93f, 0.94f, 0.96f }},
    {{ -0.5f,  0.5f,  0.5f }, { 0.27f, 0.88f, 0.90f }}
}};

constexpr std::array<GLushort, 36> kCubeIndices = {{
    0, 1, 2, 2, 3, 0,
    4, 7, 6, 6, 5, 4,
    0, 3, 7, 7, 4, 0,
    1, 5, 6, 6, 2, 1,
    3, 2, 6, 6, 7, 3,
    0, 4, 5, 5, 1, 0
}};

const std::array<vmath::vec3, 8> kCubePositions = {{
    vmath::vec3(0.0f, 1.0f, 0.0f),
    vmath::vec3(3.0f, 0.5f, 2.0f),
    vmath::vec3(-3.5f, 0.75f, 1.5f),
    vmath::vec3(2.5f, 1.25f, -3.5f),
    vmath::vec3(-2.0f, 0.5f, -4.0f),
    vmath::vec3(5.0f, 0.5f, -1.5f),
    vmath::vec3(-5.5f, 1.5f, 3.5f),
    vmath::vec3(0.0f, 2.0f, -6.0f)
}};

constexpr const char* kVertexShaderSource = R"(
#version 460 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_color;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

out vec3 v_color;

void main()
{
    gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.0);
    v_color = a_color;
}
)";

constexpr const char* kFragmentShaderSource = R"(
#version 460 core

in vec3 v_color;

out vec4 fragColor;

void main()
{
    fragColor = vec4(v_color, 1.0);
}
)";

constexpr float kCameraMoveSpeed = 6.0f;
constexpr float kCameraBoostMultiplier = 2.5f;
constexpr float kKeyboardTurnSpeed = 90.0f;
constexpr float kMouseSensitivity = 0.12f;

std::vector<Vertex> BuildGridVertices()
{
    constexpr int kGridHalfExtent = 12;
    std::vector<Vertex> vertices;
    vertices.reserve(static_cast<size_t>((kGridHalfExtent * 2 + 1) * 4));

    for (int i = -kGridHalfExtent; i <= kGridHalfExtent; ++i)
    {
        const bool isCenterLine = (i == 0);
        const GLfloat line = static_cast<GLfloat>(i);

        const Vertex xzLineStart = {
            { static_cast<GLfloat>(-kGridHalfExtent), 0.0f, line },
            { isCenterLine ? 0.15f : 0.28f, isCenterLine ? 0.45f : 0.28f, isCenterLine ? 0.95f : 0.28f }
        };
        const Vertex xzLineEnd = {
            { static_cast<GLfloat>(kGridHalfExtent), 0.0f, line },
            { isCenterLine ? 0.15f : 0.28f, isCenterLine ? 0.45f : 0.28f, isCenterLine ? 0.95f : 0.28f }
        };
        const Vertex zxLineStart = {
            { line, 0.0f, static_cast<GLfloat>(-kGridHalfExtent) },
            { isCenterLine ? 0.95f : 0.28f, isCenterLine ? 0.25f : 0.28f, isCenterLine ? 0.20f : 0.28f }
        };
        const Vertex zxLineEnd = {
            { line, 0.0f, static_cast<GLfloat>(kGridHalfExtent) },
            { isCenterLine ? 0.95f : 0.28f, isCenterLine ? 0.25f : 0.28f, isCenterLine ? 0.20f : 0.28f }
        };

        vertices.push_back(xzLineStart);
        vertices.push_back(xzLineEnd);
        vertices.push_back(zxLineStart);
        vertices.push_back(zxLineEnd);
    }

    return vertices;
}
} // namespace

bool ShaderExample::OnInitialize(const AppConfig& config)
{
    if (!VermilionApplication::OnInitialize(config))
    {
        return false;
    }

    ResetCamera();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    if (!m_program.Init(kVertexShaderSource, kFragmentShaderSource))
    {
        return false;
    }

    glGenVertexArrays(1, &m_cubeVao);
    glBindVertexArray(m_cubeVao);

    glGenBuffers(1, &m_cubeVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_cubeVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(sizeof(kCubeVertices)),
                 kCubeVertices.data(),
                 GL_STATIC_DRAW);

    glGenBuffers(1, &m_cubeIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cubeIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(sizeof(kCubeIndices)),
                 kCubeIndices.data(),
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

    const std::vector<Vertex> gridVertices = BuildGridVertices();
    m_gridVertexCount = static_cast<GLsizei>(gridVertices.size());

    glGenVertexArrays(1, &m_gridVao);
    glBindVertexArray(m_gridVao);

    glGenBuffers(1, &m_gridVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_gridVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(gridVertices.size() * sizeof(Vertex)),
                 gridVertices.data(),
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

    std::fprintf(stderr,
                 "02-02-shader controls\n"
                 "  RMB drag : look around\n"
                 "  W/A/S/D  : move forward/left/back/right\n"
                 "  Q/E      : move down/up\n"
                 "  Arrows   : keyboard look\n"
                 "  Shift    : speed boost\n"
                 "  R        : reset camera\n"
                 "  Esc      : quit\n");

    return true;
}

void ShaderExample::OnUpdate(float deltaTime)
{
    m_elapsedTime += deltaTime;

    if (m_keyDown[GLFW_KEY_LEFT])
    {
        m_camera.AddYawPitch(-kKeyboardTurnSpeed * deltaTime, 0.0f);
    }
    if (m_keyDown[GLFW_KEY_RIGHT])
    {
        m_camera.AddYawPitch(kKeyboardTurnSpeed * deltaTime, 0.0f);
    }
    if (m_keyDown[GLFW_KEY_UP])
    {
        m_camera.AddYawPitch(0.0f, kKeyboardTurnSpeed * deltaTime);
    }
    if (m_keyDown[GLFW_KEY_DOWN])
    {
        m_camera.AddYawPitch(0.0f, -kKeyboardTurnSpeed * deltaTime);
    }

    vmath::vec3 movement(0.0f, 0.0f, 0.0f);
    const vmath::vec3 forward = m_camera.GetForward();
    const vmath::vec3 right = m_camera.GetRight();
    const vmath::vec3 worldUp(0.0f, 1.0f, 0.0f);

    if (m_keyDown[GLFW_KEY_W])
    {
        movement += forward;
    }
    if (m_keyDown[GLFW_KEY_S])
    {
        movement -= forward;
    }
    if (m_keyDown[GLFW_KEY_D])
    {
        movement += right;
    }
    if (m_keyDown[GLFW_KEY_A])
    {
        movement -= right;
    }
    if (m_keyDown[GLFW_KEY_E])
    {
        movement += worldUp;
    }
    if (m_keyDown[GLFW_KEY_Q])
    {
        movement -= worldUp;
    }

    const float movementLength = vmath::length(movement);
    if (movementLength > 0.0f)
    {
        float moveSpeed = kCameraMoveSpeed;
        if (m_keyDown[GLFW_KEY_LEFT_SHIFT] || m_keyDown[GLFW_KEY_RIGHT_SHIFT])
        {
            moveSpeed *= kCameraBoostMultiplier;
        }

        movement = movement / movementLength;
        m_camera.Move(movement * moveSpeed * deltaTime);
    }
}

void ShaderExample::OnDisplay()
{
    glClearColor(0.08f, 0.10f, 0.14f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_program.Use();
    m_program.SetMat4("u_view", m_camera.GetViewMatrix());
    m_program.SetMat4("u_projection", m_camera.GetProjectionMatrix());

    const vmath::mat4 identity = vmath::mat4::identity();
    m_program.SetMat4("u_model", identity);
    glBindVertexArray(m_gridVao);
    glDrawArrays(GL_LINES, 0, m_gridVertexCount);

    glBindVertexArray(m_cubeVao);
    for (size_t i = 0; i < kCubePositions.size(); ++i)
    {
        const float spin = m_elapsedTime * (20.0f + static_cast<float>(i) * 5.0f);
        const vmath::vec3& cubePosition = kCubePositions[i];
        const vmath::mat4 model =
            vmath::translate(cubePosition) *
            vmath::rotate(spin, 0.0f, 1.0f, 0.0f) *
            vmath::rotate(spin * 0.35f, 1.0f, 0.0f, 0.0f);

        m_program.SetMat4("u_model", model);
        glDrawElements(GL_TRIANGLES,
                       static_cast<GLsizei>(kCubeIndices.size()),
                       GL_UNSIGNED_SHORT,
                       nullptr);
    }

    VermilionApplication::OnDisplay();
}

void ShaderExample::OnShutdown()
{
    if (GetWindow() != nullptr)
    {
        glfwSetInputMode(GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    glDeleteBuffers(1, &m_gridVertexBuffer);
    glDeleteVertexArrays(1, &m_gridVao);
    glDeleteBuffers(1, &m_cubeIndexBuffer);
    glDeleteBuffers(1, &m_cubeVertexBuffer);
    glDeleteVertexArrays(1, &m_cubeVao);
    m_program.Destroy();

    m_gridVertexBuffer = 0;
    m_gridVao = 0;
    m_cubeIndexBuffer = 0;
    m_cubeVertexBuffer = 0;
    m_cubeVao = 0;
    m_gridVertexCount = 0;
}

void ShaderExample::OnResize(int width, int height)
{
    glViewport(0, 0, width, height);
    m_aspect = (height > 0) ? static_cast<float>(width) / static_cast<float>(height) : 1.0f;
    m_camera.SetAspect(m_aspect);
}

void ShaderExample::OnKey(int key, int scancode, int action, int mods)
{
    (void)scancode;
    (void)mods;

    if (key >= 0 && key <= GLFW_KEY_LAST)
    {
        if (action == GLFW_PRESS)
        {
            m_keyDown[static_cast<size_t>(key)] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            m_keyDown[static_cast<size_t>(key)] = false;
        }
    }

    if (action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_ESCAPE)
        {
            glfwSetWindowShouldClose(GetWindow(), GLFW_TRUE);
            return;
        }

        if (key == GLFW_KEY_R)
        {
            ResetCamera();
            return;
        }
    }

    VermilionApplication::OnKey(key, scancode, action, mods);
}

void ShaderExample::OnMouseButton(int button, int action, int mods)
{
    (void)mods;

    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        m_rightMouseDown = (action != GLFW_RELEASE);
        m_mouseLookPrimed = false;

        if (GetWindow() != nullptr)
        {
            glfwSetInputMode(GetWindow(),
                             GLFW_CURSOR,
                             m_rightMouseDown ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        }
    }

    VermilionApplication::OnMouseButton(button, action, mods);
}

void ShaderExample::OnCursorMove(double x, double y)
{
    if (!m_rightMouseDown)
    {
        return;
    }

    if (!m_mouseLookPrimed)
    {
        m_lastCursorX = x;
        m_lastCursorY = y;
        m_mouseLookPrimed = true;
        return;
    }

    const float deltaX = static_cast<float>(x - m_lastCursorX);
    const float deltaY = static_cast<float>(y - m_lastCursorY);

    m_lastCursorX = x;
    m_lastCursorY = y;

    m_camera.AddYawPitch(deltaX * kMouseSensitivity, -deltaY * kMouseSensitivity);
}

void ShaderExample::ResetCamera()
{
    m_camera.Reset();
    m_camera.SetAspect(m_aspect);
    m_rightMouseDown = false;
    m_mouseLookPrimed = false;
    m_lastCursorX = 0.0;
    m_lastCursorY = 0.0;
    m_keyDown.fill(false);

    if (GetWindow() != nullptr)
    {
        glfwSetInputMode(GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    ShaderExample app;
    AppConfig config{};
    config.title = "02-02 Shader";
    return app.Run(config);
}
