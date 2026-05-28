#ifndef __02_03_LIGHT_H__
#define __02_03_LIGHT_H__

#include <array>

#include "Camera.h"
#include "Light.h"
#include "ShaderProgram.h"
#include "vapp.h"
#include "vbm.h"

class LightExample : public VermilionApplication
{
private:
    static constexpr size_t kPointLightCount = 3;

    float m_aspect = 16.0f / 9.0f;
    float m_elapsedTime = 0.0f;
    float m_pointLightOrbitDegrees = 0.0f;

    ShaderProgram m_litProgram {};
    ShaderProgram m_unlitProgram {};

    GLuint m_cubeVao = 0;
    GLuint m_cubeVertexBuffer = 0;
    GLuint m_cubeIndexBuffer = 0;
    VBObject m_lightSphere {};

    Camera m_camera {};
    SunLight m_sunLight {};
    std::array<PointLight, kPointLightCount> m_pointLights {};

    bool m_sunLightEnabled = true;
    bool m_pointLightsEnabled = true;
    bool m_pointLightOrbitEnabled = true;

    bool m_rightMouseDown = false;
    bool m_mouseLookPrimed = false;
    double m_lastCursorX = 0.0;
    double m_lastCursorY = 0.0;

    std::array<bool, GLFW_KEY_LAST + 1> m_keyDown {};

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
    void UpdatePointLightPositions();
};

#endif /* __02_03_LIGHT_H__ */
