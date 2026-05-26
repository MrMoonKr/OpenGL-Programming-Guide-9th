#ifndef __02_02_SHADER_H__
#define __02_02_SHADER_H__

#include <array>

#include "Camera.h"
#include "ShaderProgram.h"
#include "vapp.h"

class ShaderExample : public VermilionApplication
{
private:
    float m_aspect = 16.0f / 9.0f;
    float m_elapsedTime = 0.0f;

    ShaderProgram m_program{};

    GLuint m_cubeVao = 0;
    GLuint m_cubeVertexBuffer = 0;
    GLuint m_cubeIndexBuffer = 0;

    GLuint m_gridVao = 0;
    GLuint m_gridVertexBuffer = 0;
    GLsizei m_gridVertexCount = 0;

    Camera m_camera{};

    bool m_rightMouseDown = false;
    bool m_mouseLookPrimed = false;
    double m_lastCursorX = 0.0;
    double m_lastCursorY = 0.0;

    std::array<bool, GLFW_KEY_LAST + 1> m_keyDown{};

public:
    bool OnInitialize(const AppConfig& config) override;

    void OnUpdate(float deltaTime) override;

    void OnDisplay() override;

    void OnShutdown() override;

    void OnResize(int width, int height) override;

    void OnKey(int key, int scancode, int action, int mods) override;

    void OnMouseButton(int button, int action, int mods) override;

    void OnCursorMove(double x, double y) override;

private:
    void ResetCamera();
};

#endif /* __02_02_SHADER_H__ */
