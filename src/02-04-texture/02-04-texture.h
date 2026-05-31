#ifndef __02_04_TEXTURE_H__
#define __02_04_TEXTURE_H__

#include <array>

#include "Camera.h"
#include "Light.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "vapp.h"
#include "vbm.h"

class TextureExample : public VermilionApplication
{
private:
    static constexpr size_t kPointLightCount = 3;

    struct SunLightUniforms
    {
        GLint enabled = -1;
        GLint direction = -1;
        GLint ambient = -1;
        GLint diffuse = -1;
        GLint specular = -1;
    };

    struct PointLightUniforms
    {
        GLint enabled = -1;
        GLint position = -1;
        GLint ambient = -1;
        GLint diffuse = -1;
        GLint specular = -1;
        GLint constant = -1;
        GLint linear = -1;
        GLint quadratic = -1;
    };

    float m_aspect = 16.0f / 9.0f;
    float m_elapsedTime = 0.0f;
    float m_pointLightOrbitDegrees = 0.0f;

    ShaderProgram m_litProgram {};
    ShaderProgram m_unlitProgram {};

    GLuint m_cubeVao = 0;
    GLuint m_cubeVertexBuffer = 0;
    GLuint m_cubeIndexBuffer = 0;
    VBObject m_lightSphere {};

    GLint m_litModelLocation = -1;
    GLint m_litViewLocation = -1;
    GLint m_litProjectionLocation = -1;
    GLint m_litNormalMatrixLocation = -1;
    GLint m_litViewPosLocation = -1;
    GLint m_litAlbedoLocation = -1;
    GLint m_litTextureLocation = -1;
    GLint m_litUseTextureLocation = -1;
    GLint m_litShininessLocation = -1;
    GLint m_litSpecularStrengthLocation = -1;
    GLint m_litUvScaleLocation = -1;
    SunLightUniforms m_sunLightUniforms {};
    std::array<PointLightUniforms, kPointLightCount> m_pointLightUniforms {};

    GLint m_unlitModelLocation = -1;
    GLint m_unlitViewLocation = -1;
    GLint m_unlitProjectionLocation = -1;
    GLint m_unlitColorLocation = -1;

    Texture m_texture {};
    Camera m_camera {};
    SunLight m_sunLight {};
    std::array<PointLight, kPointLightCount> m_pointLights {};
    vmath::mat4 m_floorModel = vmath::mat4::identity();
    vmath::mat3 m_floorNormalMatrix = vmath::mat3::identity();

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

#endif /* __02_04_TEXTURE_H__ */
