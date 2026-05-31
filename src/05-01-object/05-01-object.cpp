#include "05-01-object.h"

#include <array>
#include <cmath>
#include <cstdio>

#include "PrimitiveMesh.h"

namespace
{
constexpr size_t kSceneCubeCount = 6;
constexpr size_t kScenePointLightCount = 3;

struct SceneCube
{
    vmath::vec3 position;
    vmath::vec3 rotationEulerSpeed;
    vmath::vec3 scale;
    vmath::vec3 albedo;
};

struct PointLightMotion
{
    float radius;
    float heightOffset;
    float heightAmplitude;
    float angleOffsetDegrees;
    float orbitSpeedScale;
    float bobSpeedScale;
};

const std::array<SceneCube, kSceneCubeCount> kSceneCubes = {{
    { vmath::vec3(0.0f, 0.60f, 0.0f),  vmath::vec3(11.0f, 20.0f, 7.0f),  vmath::vec3(1.20f, 1.20f, 1.20f), vmath::vec3(0.90f, 0.55f, 0.42f) },
    { vmath::vec3(2.8f, 0.45f, 1.8f),  vmath::vec3(28.0f, 18.0f, 9.0f),  vmath::vec3(0.90f, 0.90f, 0.90f), vmath::vec3(0.32f, 0.72f, 0.96f) },
    { vmath::vec3(-3.2f, 0.70f, 1.4f), vmath::vec3(15.0f, 24.0f, 18.0f), vmath::vec3(1.00f, 1.40f, 1.00f), vmath::vec3(0.40f, 0.82f, 0.52f) },
    { vmath::vec3(2.3f, 0.80f, -2.8f), vmath::vec3(32.0f, 12.0f, 22.0f), vmath::vec3(1.10f, 1.10f, 1.10f), vmath::vec3(0.98f, 0.76f, 0.24f) },
    { vmath::vec3(-2.0f, 0.50f, -3.4f),vmath::vec3(7.0f, 18.0f, 14.0f),  vmath::vec3(0.80f, 1.00f, 0.80f), vmath::vec3(0.80f, 0.48f, 0.92f) },
    { vmath::vec3(0.0f, 1.55f, -5.6f), vmath::vec3(10.0f, 16.0f, 10.0f), vmath::vec3(1.60f, 1.60f, 1.60f), vmath::vec3(0.92f, 0.92f, 0.95f) }
}};

const std::array<PointLightMotion, kScenePointLightCount> kPointLightMotions = {{
    { 5.2f, 2.2f, 0.7f,   0.0f, 1.00f, 1.7f },
    { 4.4f, 1.6f, 0.5f, 120.0f, 0.75f, 2.1f },
    { 6.1f, 2.8f, 0.6f, 240.0f, 1.20f, 1.3f }
}};

constexpr float kCameraMoveSpeed = 6.0f;
constexpr float kCameraBoostMultiplier = 2.5f;
constexpr float kKeyboardTurnSpeed = 90.0f;
constexpr float kMouseSensitivity = 0.12f;
constexpr float kPointLightMarkerScale = 0.14f;
constexpr float kPi = 3.1415926535f;

vmath::vec3 ExtractTranslation(const vmath::mat4& matrix)
{
    return vmath::vec3(matrix[3][0], matrix[3][1], matrix[3][2]);
}
} // namespace

bool ObjectExample::OnInitialize(const AppConfig& config)
{
    if (!VermilionApplication::OnInitialize(config))
    {
        return false;
    }

    ResetCamera();

    m_pointLights[0].ambient = vmath::vec3(0.05f, 0.03f, 0.02f);
    m_pointLights[0].diffuse = vmath::vec3(1.00f, 0.72f, 0.38f);
    m_pointLights[0].specular = vmath::vec3(1.00f, 0.82f, 0.60f);

    m_pointLights[1].ambient = vmath::vec3(0.02f, 0.04f, 0.05f);
    m_pointLights[1].diffuse = vmath::vec3(0.34f, 0.76f, 1.00f);
    m_pointLights[1].specular = vmath::vec3(0.68f, 0.90f, 1.00f);

    m_pointLights[2].ambient = vmath::vec3(0.04f, 0.02f, 0.05f);
    m_pointLights[2].diffuse = vmath::vec3(0.88f, 0.44f, 1.00f);
    m_pointLights[2].specular = vmath::vec3(0.94f, 0.76f, 1.00f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    if (!m_litProgram.InitFromFiles("media/shaders/05-01-object/lit.vs.glsl",
                                    "media/shaders/05-01-object/lit.fs.glsl") ||
        !m_unlitProgram.InitFromFiles("media/shaders/05-01-object/unlit.vs.glsl",
                                      "media/shaders/05-01-object/unlit.fs.glsl") ||
        !InitializeScene())
    {
        return false;
    }

    std::fprintf(stderr,
                 "05-01-object controls\n"
                 "  RMB drag : look around\n"
                 "  W/A/S/D  : move forward/left/back/right\n"
                 "  Q/E      : move down/up\n"
                 "  Arrows   : keyboard look\n"
                 "  Shift    : speed boost\n"
                 "  1        : toggle SunLight\n"
                 "  2        : toggle 3 PointLights\n"
                 "  L        : toggle point-light orbits\n"
                 "  R        : reset camera\n"
                 "  Esc      : quit\n");

    return true;
}

void ObjectExample::OnUpdate(float deltaTime)
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

        m_camera.Move((movement / movementLength) * moveSpeed * deltaTime);
    }

    if (m_pointLightOrbitEnabled)
    {
        m_pointLightOrbitDegrees += deltaTime * 45.0f;
    }

    UpdateSceneObjects();
    UpdatePointLightPositions();
}

void ObjectExample::OnDisplay()
{
    glClearColor(0.07f, 0.09f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_litProgram.Use();
    m_litProgram.SetMat4("u_view", m_camera.GetViewMatrix());
    m_litProgram.SetMat4("u_projection", m_camera.GetProjectionMatrix());
    m_litProgram.SetVec3("u_viewPos", m_camera.GetPosition());
    m_litProgram.SetFloat("u_shininess", 48.0f);
    m_litProgram.SetFloat("u_specularStrength", 0.55f);

    m_sunLight.enabled = m_sunLightEnabled;
    m_sunLight.Apply(m_litProgram, "u_sunLight");

    for (size_t i = 0; i < m_pointLights.size(); ++i)
    {
        m_pointLights[i].enabled = m_pointLightsEnabled;

        const char* prefix = (i == 0) ? "u_pointLights[0]" :
                             (i == 1) ? "u_pointLights[1]" :
                                        "u_pointLights[2]";
        m_pointLights[i].Apply(m_litProgram, prefix);
    }

    if (m_floorObject.IsVisible())
    {
        m_litProgram.SetMat4("u_model", m_floorObject.GetWorldMatrix());
        m_litProgram.SetVec3("u_albedo", vmath::vec3(0.34f, 0.36f, 0.40f));
        m_boxGeometry.Draw();
    }

    for (size_t i = 0; i < m_cubeObjects.size(); ++i)
    {
        if (!m_cubeObjects[i].IsVisible())
        {
            continue;
        }

        m_litProgram.SetMat4("u_model", m_cubeObjects[i].GetWorldMatrix());
        m_litProgram.SetVec3("u_albedo", kSceneCubes[i].albedo);
        m_boxGeometry.Draw();
    }

    m_unlitProgram.Use();
    m_unlitProgram.SetMat4("u_view", m_camera.GetViewMatrix());
    m_unlitProgram.SetMat4("u_projection", m_camera.GetProjectionMatrix());

    for (size_t i = 0; i < m_pointLightObjects.size(); ++i)
    {
        if (!m_pointLightObjects[i].IsVisible())
        {
            continue;
        }

        m_unlitProgram.SetMat4("u_model", m_pointLightObjects[i].GetWorldMatrix());
        m_unlitProgram.SetVec3("u_color", m_pointLights[i].diffuse);
        m_sphereGeometry.Draw();
    }

    VermilionApplication::OnDisplay();
}

void ObjectExample::OnShutdown()
{
    if (GetWindow() != nullptr)
    {
        glfwSetInputMode(GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    m_sphereGeometry.Destroy();
    m_boxGeometry.Destroy();
    m_unlitProgram.Destroy();
    m_litProgram.Destroy();
}

void ObjectExample::OnResize(int width, int height)
{
    glViewport(0, 0, width, height);
    m_aspect = (height > 0) ? static_cast<float>(width) / static_cast<float>(height) : 1.0f;
    m_camera.SetAspect(m_aspect);
}

void ObjectExample::OnKey(int key, int scancode, int action, int mods)
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

        if (key == GLFW_KEY_1)
        {
            m_sunLightEnabled = !m_sunLightEnabled;
            return;
        }

        if (key == GLFW_KEY_2)
        {
            m_pointLightsEnabled = !m_pointLightsEnabled;
            return;
        }

        if (key == GLFW_KEY_L)
        {
            m_pointLightOrbitEnabled = !m_pointLightOrbitEnabled;
            return;
        }
    }

    VermilionApplication::OnKey(key, scancode, action, mods);
}

void ObjectExample::OnMouseButton(int button, int action, int mods)
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

void ObjectExample::OnCursorMove(double x, double y)
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

bool ObjectExample::InitializeScene()
{
    MeshData boxMesh = PrimitiveMesh::MakeBox();
    MeshData sphereMesh = PrimitiveMesh::MakeSphere(0.5f, 24, 12);

    if (!m_boxGeometry.Create(boxMesh) || !m_sphereGeometry.Create(sphereMesh))
    {
        return false;
    }

    m_floorObject.SetGeometry(&m_boxGeometry);
    m_sceneRoot.AddChild(m_floorObject);
    m_floorObject.GetTransform().SetPosition(0.0f, -0.85f, 0.0f);
    m_floorObject.GetTransform().SetScale(14.0f, 0.30f, 14.0f);

    for (size_t i = 0; i < m_cubeObjects.size(); ++i)
    {
        m_cubeObjects[i].SetGeometry(&m_boxGeometry);
        m_sceneRoot.AddChild(m_cubeObjects[i]);
        m_cubeObjects[i].GetTransform().SetPosition(kSceneCubes[i].position);
        m_cubeObjects[i].GetTransform().SetScale(kSceneCubes[i].scale);
    }

    for (size_t i = 0; i < m_pointLightOrbitRoots.size(); ++i)
    {
        m_sceneRoot.AddChild(m_pointLightOrbitRoots[i]);
        m_pointLightOrbitRoots[i].AddChild(m_pointLightObjects[i]);
        m_pointLightObjects[i].SetGeometry(&m_sphereGeometry);
        m_pointLightObjects[i].GetTransform().SetUniformScale(kPointLightMarkerScale);
    }

    UpdateSceneObjects();
    UpdatePointLightPositions();
    return true;
}

void ObjectExample::ResetCamera()
{
    m_camera.Reset();
    m_camera.SetAspect(m_aspect);

    m_sunLightEnabled = true;
    m_pointLightsEnabled = true;
    m_pointLightOrbitEnabled = true;
    m_pointLightOrbitDegrees = 0.0f;

    m_rightMouseDown = false;
    m_mouseLookPrimed = false;
    m_lastCursorX = 0.0;
    m_lastCursorY = 0.0;
    m_keyDown.fill(false);

    UpdateSceneObjects();
    UpdatePointLightPositions();

    if (GetWindow() != nullptr)
    {
        glfwSetInputMode(GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void ObjectExample::UpdateSceneObjects()
{
    for (size_t i = 0; i < m_cubeObjects.size(); ++i)
    {
        const SceneCube& cube = kSceneCubes[i];
        const float spin = m_elapsedTime;
        m_cubeObjects[i].GetTransform().SetRotationEuler(cube.rotationEulerSpeed * spin);
    }
}

void ObjectExample::UpdatePointLightPositions()
{
    for (size_t i = 0; i < m_pointLightObjects.size(); ++i)
    {
        const PointLightMotion& motion = kPointLightMotions[i];
        const float angleDegrees = m_pointLightOrbitDegrees * motion.orbitSpeedScale + motion.angleOffsetDegrees;
        const float angleRadians = angleDegrees * (kPi / 180.0f);
        const float bobRadians = angleRadians * motion.bobSpeedScale;
        const float height = motion.heightOffset + std::sin(bobRadians) * motion.heightAmplitude;

        m_pointLightOrbitRoots[i].GetTransform().SetRotationEuler(0.0f, angleDegrees, 0.0f);
        m_pointLightObjects[i].GetTransform().SetPosition(motion.radius, height, 0.0f);

        m_pointLights[i].position = ExtractTranslation(m_pointLightObjects[i].GetWorldMatrix());
    }
}

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    ObjectExample app;
    AppConfig config {};
    config.title = "05-01 Object";
    return app.Run(config);
}
