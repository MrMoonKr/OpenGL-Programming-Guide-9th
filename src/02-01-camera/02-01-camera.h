#ifndef __02_01_CAMERA_H__
#define __02_01_CAMERA_H__

#include <array>

#include "camera.h"
#include "vapp.h"

class CameraExample : public VermilionApplication
{
private:
    float m_aspect = 16.0f / 9.0f;
    float m_elapsedTime = 0.0f;

    GLuint m_program = 0;

    GLuint m_cubeVao = 0;
    GLuint m_cubeVertexBuffer = 0;
    GLuint m_cubeIndexBuffer = 0;

    GLuint m_gridVao = 0;
    GLuint m_gridVertexBuffer = 0;
    GLsizei m_gridVertexCount = 0;

    GLint m_modelLocation = -1;
    GLint m_viewLocation = -1;
    GLint m_projectionLocation = -1;

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

#endif /* __02_01_CAMERA_H__ */
