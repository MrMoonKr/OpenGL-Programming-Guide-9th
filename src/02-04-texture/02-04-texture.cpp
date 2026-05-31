#include "02-04-texture.h"

#include <array>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <string>

#include "vmath.h"
#include "vutils.h"

namespace
{
struct Vertex
{
    GLfloat position[3];
    GLfloat normal[3];
    GLfloat texCoord[2];
};

struct SceneCube
{
    vmath::vec3 position;
    vmath::vec3 rotationAxis;
    float rotationSpeed;
    vmath::vec3 scale;
    vmath::vec3 albedo;
    bool textured;
    vmath::vec2 uvScale;
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

constexpr std::array<Vertex, 24> kCubeVertices = {{
    {{ -0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, { 0.0f, 0.0f }},
    {{  0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, { 1.0f, 0.0f }},
    {{  0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, { 1.0f, 1.0f }},
    {{ -0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, { 0.0f, 1.0f }},

    {{  0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, { 0.0f, 0.0f }},
    {{ -0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 0.0f }},
    {{ -0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 1.0f }},
    {{  0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, { 0.0f, 1.0f }},

    {{ -0.5f, -0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f }, { 0.0f, 0.0f }},
    {{ -0.5f, -0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f }, { 1.0f, 0.0f }},
    {{ -0.5f,  0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f }, { 1.0f, 1.0f }},
    {{ -0.5f,  0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f }, { 0.0f, 1.0f }},

    {{  0.5f, -0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f }, { 0.0f, 0.0f }},
    {{  0.5f, -0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f }, { 1.0f, 0.0f }},
    {{  0.5f,  0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f }, { 1.0f, 1.0f }},
    {{  0.5f,  0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f }, { 0.0f, 1.0f }},

    {{ -0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f }, { 0.0f, 0.0f }},
    {{  0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f }, { 1.0f, 0.0f }},
    {{  0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f }, { 1.0f, 1.0f }},
    {{ -0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f }, { 0.0f, 1.0f }},

    {{ -0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f }, { 0.0f, 0.0f }},
    {{  0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f }, { 1.0f, 0.0f }},
    {{  0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f }, { 1.0f, 1.0f }},
    {{ -0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f }, { 0.0f, 1.0f }}
}};

constexpr std::array<GLushort, 36> kCubeIndices = {{
    0, 1, 2, 2, 3, 0,
    4, 5, 6, 6, 7, 4,
    8, 9, 10, 10, 11, 8,
    12, 13, 14, 14, 15, 12,
    16, 17, 18, 18, 19, 16,
    20, 21, 22, 22, 23, 20
}};

const std::array<SceneCube, 7> kSceneCubes = {{
    { vmath::vec3(0.0f, 0.60f, 0.0f),   vmath::vec3(0.0f, 1.0f, 0.0f), 20.0f, vmath::vec3(1.20f, 1.20f, 1.20f), vmath::vec3(0.90f, 0.55f, 0.42f), false, vmath::vec2(1.0f, 1.0f) },
    { vmath::vec3(2.8f, 0.45f, 1.8f),   vmath::vec3(1.0f, 1.0f, 0.0f), 28.0f, vmath::vec3(0.90f, 0.90f, 0.90f), vmath::vec3(0.32f, 0.72f, 0.96f), false, vmath::vec2(1.0f, 1.0f) },
    { vmath::vec3(-3.2f, 0.70f, 1.4f),  vmath::vec3(0.0f, 1.0f, 1.0f), 24.0f, vmath::vec3(1.00f, 1.40f, 1.00f), vmath::vec3(0.40f, 0.82f, 0.52f), false, vmath::vec2(1.0f, 1.0f) },
    { vmath::vec3(2.3f, 0.80f, -2.8f),  vmath::vec3(1.0f, 0.0f, 1.0f), 32.0f, vmath::vec3(1.10f, 1.10f, 1.10f), vmath::vec3(1.00f, 1.00f, 1.00f), true,  vmath::vec2(1.0f, 1.0f) },
    { vmath::vec3(-2.0f, 0.50f, -3.4f), vmath::vec3(0.4f, 1.0f, 0.2f), 18.0f, vmath::vec3(0.80f, 1.00f, 0.80f), vmath::vec3(1.00f, 1.00f, 1.00f), true,  vmath::vec2(1.0f, 1.0f) },
    { vmath::vec3(0.0f, 1.55f, -5.6f),  vmath::vec3(1.0f, 1.0f, 1.0f), 16.0f, vmath::vec3(1.60f, 1.60f, 1.60f), vmath::vec3(1.00f, 1.00f, 1.00f), true,  vmath::vec2(1.0f, 1.0f) },
    { vmath::vec3(4.8f, 0.70f, -0.8f),  vmath::vec3(0.2f, 1.0f, 0.6f), 22.0f, vmath::vec3(1.00f, 1.00f, 1.00f), vmath::vec3(1.00f, 1.00f, 1.00f), true,  vmath::vec2(1.0f, 1.0f) }
}};

constexpr float kCameraMoveSpeed = 6.0f;
constexpr float kCameraBoostMultiplier = 2.5f;
constexpr float kKeyboardTurnSpeed = 90.0f;
constexpr float kMouseSensitivity = 0.12f;
constexpr float kPi = 3.1415926535f;

const std::array<PointLightMotion, 3> kPointLightMotions = {{
    { 5.2f, 2.2f, 0.7f,   0.0f, 1.00f, 1.7f },
    { 4.4f, 1.6f, 0.5f, 120.0f, 0.75f, 2.1f },
    { 6.1f, 2.8f, 0.6f, 240.0f, 1.20f, 1.3f }
}};

constexpr const char* kLitVertexShaderSource = R"(
#version 460 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texCoord;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform vec2 u_uvScale;

out vec3 v_worldPos;
out vec3 v_normal;
out vec2 v_texCoord;

uniform mat3 u_normalMatrix;

void main()
{
    vec4 worldPos = u_model * vec4(a_position, 1.0);
    v_worldPos = worldPos.xyz;
    v_normal = u_normalMatrix * a_normal;
    v_texCoord = a_texCoord * u_uvScale;
    gl_Position = u_projection * u_view * worldPos;
}
)";

constexpr const char* kLitFragmentShaderSource = R"(
#version 460 core

struct SunLight
{
    int enabled;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    int enabled;
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

in vec3 v_worldPos;
in vec3 v_normal;
in vec2 v_texCoord;

layout (location = 0) out vec4 fragColor;

uniform SunLight u_sunLight;
uniform PointLight u_pointLights[3];
uniform vec3 u_viewPos;
uniform vec3 u_albedo;
uniform sampler2D u_texture;
uniform int u_useTexture;
uniform float u_shininess;
uniform float u_specularStrength;

vec3 ResolveBaseColor()
{
    if (u_useTexture != 0)
    {
        return texture(u_texture, v_texCoord).rgb;
    }

    return u_albedo;
}

vec3 ApplySunLight(SunLight light, vec3 normal, vec3 viewDir, vec3 baseColor)
{
    if (light.enabled == 0)
    {
        return vec3(0.0);
    }

    vec3 lightDir = normalize(-light.direction);
    float diffuseFactor = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), u_shininess);

    vec3 ambient = light.ambient * baseColor;
    vec3 diffuse = light.diffuse * diffuseFactor * baseColor;
    vec3 specular = light.specular * specularFactor * u_specularStrength;
    return ambient + diffuse + specular;
}

vec3 ApplyPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 baseColor)
{
    if (light.enabled == 0)
    {
        return vec3(0.0);
    }

    vec3 toLight = light.position - fragPos;
    float distanceToLight = length(toLight);
    vec3 lightDir = normalize(toLight);
    float diffuseFactor = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), u_shininess);
    float attenuation = 1.0 / (light.constant +
                               light.linear * distanceToLight +
                               light.quadratic * distanceToLight * distanceToLight);

    vec3 ambient = light.ambient * baseColor;
    vec3 diffuse = light.diffuse * diffuseFactor * baseColor;
    vec3 specular = light.specular * specularFactor * u_specularStrength;
    return (ambient + diffuse + specular) * attenuation;
}

void main()
{
    vec3 baseColor = ResolveBaseColor();
    vec3 normal = normalize(v_normal);
    vec3 viewDir = normalize(u_viewPos - v_worldPos);

    vec3 litColor = ApplySunLight(u_sunLight, normal, viewDir, baseColor);
    for (int i = 0; i < 3; ++i)
    {
        litColor += ApplyPointLight(u_pointLights[i], normal, v_worldPos, viewDir, baseColor);
    }

    fragColor = vec4(litColor, 1.0);
}
)";

constexpr const char* kUnlitVertexShaderSource = R"(
#version 460 core

layout (location = 0) in vec3 a_position;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
    gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.0);
}
)";

constexpr const char* kUnlitFragmentShaderSource = R"(
#version 460 core

layout (location = 0) out vec4 fragColor;

uniform vec3 u_color;

void main()
{
    fragColor = vec4(u_color, 1.0);
}
)";

vmath::vec3 NormalizeOrFallback(const vmath::vec3& value, const vmath::vec3& fallback)
{
    const float length = vmath::length(value);
    if (length <= 0.0001f)
    {
        return fallback;
    }

    return value / length;
}

vmath::mat4 BuildFloorModel()
{
    return vmath::translate(0.0f, -0.85f, 0.0f) * vmath::scale(14.0f, 0.30f, 14.0f);
}

vmath::mat3 ComputeNormalMatrix(const vmath::mat4& model)
{
    // Models in this sample are rotation * scale (+ translation), so the
    // inverse-transpose can be recovered from the scaled basis vectors.
    vmath::vec3 column0(model[0][0], model[0][1], model[0][2]);
    vmath::vec3 column1(model[1][0], model[1][1], model[1][2]);
    vmath::vec3 column2(model[2][0], model[2][1], model[2][2]);

    const float epsilon = 0.000001f;

    const float length0Squared = vmath::dot(column0, column0);
    const float length1Squared = vmath::dot(column1, column1);
    const float length2Squared = vmath::dot(column2, column2);

    if (length0Squared > epsilon)
    {
        column0 /= length0Squared;
    }
    if (length1Squared > epsilon)
    {
        column1 /= length1Squared;
    }
    if (length2Squared > epsilon)
    {
        column2 /= length2Squared;
    }

    return vmath::mat3(column0, column1, column2);
}

void SetVec3Uniform(GLint location, const vmath::vec3& value)
{
    glUniform3f(location, value[0], value[1], value[2]);
}
} // namespace

bool TextureExample::OnInitialize(const AppConfig& config)
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

    UpdatePointLightPositions();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);

    if (!m_litProgram.Init(kLitVertexShaderSource, kLitFragmentShaderSource) ||
        !m_unlitProgram.Init(kUnlitVertexShaderSource, kUnlitFragmentShaderSource) ||
        !m_lightSphere.LoadFromVBM("media/unit_sphere.vbm", 0, 1, 2))
    {
        return false;
    }

    m_litModelLocation = m_litProgram.GetUniformLocation("u_model");
    m_litViewLocation = m_litProgram.GetUniformLocation("u_view");
    m_litProjectionLocation = m_litProgram.GetUniformLocation("u_projection");
    m_litNormalMatrixLocation = m_litProgram.GetUniformLocation("u_normalMatrix");
    m_litViewPosLocation = m_litProgram.GetUniformLocation("u_viewPos");
    m_litAlbedoLocation = m_litProgram.GetUniformLocation("u_albedo");
    m_litTextureLocation = m_litProgram.GetUniformLocation("u_texture");
    m_litUseTextureLocation = m_litProgram.GetUniformLocation("u_useTexture");
    m_litShininessLocation = m_litProgram.GetUniformLocation("u_shininess");
    m_litSpecularStrengthLocation = m_litProgram.GetUniformLocation("u_specularStrength");
    m_litUvScaleLocation = m_litProgram.GetUniformLocation("u_uvScale");

    m_sunLightUniforms.enabled = m_litProgram.GetUniformLocation("u_sunLight.enabled");
    m_sunLightUniforms.direction = m_litProgram.GetUniformLocation("u_sunLight.direction");
    m_sunLightUniforms.ambient = m_litProgram.GetUniformLocation("u_sunLight.ambient");
    m_sunLightUniforms.diffuse = m_litProgram.GetUniformLocation("u_sunLight.diffuse");
    m_sunLightUniforms.specular = m_litProgram.GetUniformLocation("u_sunLight.specular");

    for (size_t i = 0; i < m_pointLightUniforms.size(); ++i)
    {
        const std::string prefix = "u_pointLights[" + std::to_string(i) + "]";
        m_pointLightUniforms[i].enabled = m_litProgram.GetUniformLocation((prefix + ".enabled").c_str());
        m_pointLightUniforms[i].position = m_litProgram.GetUniformLocation((prefix + ".position").c_str());
        m_pointLightUniforms[i].ambient = m_litProgram.GetUniformLocation((prefix + ".ambient").c_str());
        m_pointLightUniforms[i].diffuse = m_litProgram.GetUniformLocation((prefix + ".diffuse").c_str());
        m_pointLightUniforms[i].specular = m_litProgram.GetUniformLocation((prefix + ".specular").c_str());
        m_pointLightUniforms[i].constant = m_litProgram.GetUniformLocation((prefix + ".constant").c_str());
        m_pointLightUniforms[i].linear = m_litProgram.GetUniformLocation((prefix + ".linear").c_str());
        m_pointLightUniforms[i].quadratic = m_litProgram.GetUniformLocation((prefix + ".quadratic").c_str());
    }

    m_unlitModelLocation = m_unlitProgram.GetUniformLocation("u_model");
    m_unlitViewLocation = m_unlitProgram.GetUniformLocation("u_view");
    m_unlitProjectionLocation = m_unlitProgram.GetUniformLocation("u_projection");
    m_unlitColorLocation = m_unlitProgram.GetUniformLocation("u_color");

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
                          BUFFER_OFFSET(offsetof(Vertex, normal)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(Vertex),
                          BUFFER_OFFSET(offsetof(Vertex, texCoord)));
    glEnableVertexAttribArray(2);

    if (!m_texture.LoadFromFile("media/grid.jpg"))
    {
        return false;
    }

    m_floorModel = BuildFloorModel();
    m_floorNormalMatrix = ComputeNormalMatrix(m_floorModel);

    std::fprintf(stderr,
                 "02-04-texture controls\n"
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

void TextureExample::OnUpdate(float deltaTime)
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

    if (m_pointLightOrbitEnabled)
    {
        m_pointLightOrbitDegrees += deltaTime * 45.0f;
    }

    UpdatePointLightPositions();
}

void TextureExample::OnDisplay()
{
    glClearColor(0.07f, 0.09f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(m_cubeVao);

    m_litProgram.Use();
    glUniformMatrix4fv(m_litViewLocation, 1, GL_FALSE, m_camera.GetViewMatrix());
    glUniformMatrix4fv(m_litProjectionLocation, 1, GL_FALSE, m_camera.GetProjectionMatrix());
    SetVec3Uniform(m_litViewPosLocation, m_camera.GetPosition());
    glUniform1f(m_litShininessLocation, 48.0f);
    glUniform1f(m_litSpecularStrengthLocation, 0.55f);
    glUniform1i(m_litTextureLocation, 0);

    m_sunLight.enabled = m_sunLightEnabled;
    glUniform1i(m_sunLightUniforms.enabled, m_sunLight.enabled ? 1 : 0);
    SetVec3Uniform(m_sunLightUniforms.direction, m_sunLight.direction);
    SetVec3Uniform(m_sunLightUniforms.ambient, m_sunLight.ambient);
    SetVec3Uniform(m_sunLightUniforms.diffuse, m_sunLight.diffuse);
    SetVec3Uniform(m_sunLightUniforms.specular, m_sunLight.specular);
    for (size_t i = 0; i < m_pointLights.size(); ++i)
    {
        m_pointLights[i].enabled = m_pointLightsEnabled;
        glUniform1i(m_pointLightUniforms[i].enabled, m_pointLights[i].enabled ? 1 : 0);
        SetVec3Uniform(m_pointLightUniforms[i].position, m_pointLights[i].position);
        SetVec3Uniform(m_pointLightUniforms[i].ambient, m_pointLights[i].ambient);
        SetVec3Uniform(m_pointLightUniforms[i].diffuse, m_pointLights[i].diffuse);
        SetVec3Uniform(m_pointLightUniforms[i].specular, m_pointLights[i].specular);
        glUniform1f(m_pointLightUniforms[i].constant, m_pointLights[i].constant);
        glUniform1f(m_pointLightUniforms[i].linear, m_pointLights[i].linear);
        glUniform1f(m_pointLightUniforms[i].quadratic, m_pointLights[i].quadratic);
    }

    m_texture.Bind(0);

    glUniformMatrix4fv(m_litModelLocation, 1, GL_FALSE, m_floorModel);
    glUniformMatrix3fv(m_litNormalMatrixLocation, 1, GL_FALSE, m_floorNormalMatrix);
    SetVec3Uniform(m_litAlbedoLocation, vmath::vec3(0.34f, 0.36f, 0.40f));
    glUniform1i(m_litUseTextureLocation, 0);
    glUniform2f(m_litUvScaleLocation, 1.0f, 1.0f);
    glDrawElements(GL_TRIANGLES,
                   static_cast<GLsizei>(kCubeIndices.size()),
                   GL_UNSIGNED_SHORT,
                   nullptr);

    for (const SceneCube& cube : kSceneCubes)
    {
        const float spin = m_elapsedTime * cube.rotationSpeed;
        const vmath::vec3 rotationAxis = NormalizeOrFallback(cube.rotationAxis, vmath::vec3(0.0f, 1.0f, 0.0f));
        const vmath::mat4 model =
            vmath::translate(cube.position) *
            vmath::rotate(spin, rotationAxis[0], rotationAxis[1], rotationAxis[2]) *
            vmath::rotate(spin * 0.37f, 0.0f, 1.0f, 0.0f) *
            vmath::scale(cube.scale);
        const vmath::mat3 normalMatrix = ComputeNormalMatrix(model);

        glUniformMatrix4fv(m_litModelLocation, 1, GL_FALSE, model);
        glUniformMatrix3fv(m_litNormalMatrixLocation, 1, GL_FALSE, normalMatrix);
        SetVec3Uniform(m_litAlbedoLocation, cube.albedo);
        glUniform1i(m_litUseTextureLocation, cube.textured ? 1 : 0);
        glUniform2f(m_litUvScaleLocation, cube.uvScale[0], cube.uvScale[1]);
        glDrawElements(GL_TRIANGLES,
                       static_cast<GLsizei>(kCubeIndices.size()),
                       GL_UNSIGNED_SHORT,
                       nullptr);
    }

    m_unlitProgram.Use();
    glUniformMatrix4fv(m_unlitViewLocation, 1, GL_FALSE, m_camera.GetViewMatrix());
    glUniformMatrix4fv(m_unlitProjectionLocation, 1, GL_FALSE, m_camera.GetProjectionMatrix());
    for (const PointLight& pointLight : m_pointLights)
    {
        const vmath::mat4 lightModel = vmath::translate(pointLight.position) * vmath::scale(0.14f);
        glUniformMatrix4fv(m_unlitModelLocation, 1, GL_FALSE, lightModel);
        SetVec3Uniform(m_unlitColorLocation, pointLight.diffuse);
        m_lightSphere.Render();
    }

    VermilionApplication::OnDisplay();
}

void TextureExample::OnShutdown()
{
    if (GetWindow() != nullptr)
    {
        glfwSetInputMode(GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    m_texture.Destroy();
    glDeleteBuffers(1, &m_cubeIndexBuffer);
    glDeleteBuffers(1, &m_cubeVertexBuffer);
    glDeleteVertexArrays(1, &m_cubeVao);
    m_lightSphere.Free();
    m_litProgram.Destroy();
    m_unlitProgram.Destroy();

    m_cubeIndexBuffer = 0;
    m_cubeVertexBuffer = 0;
    m_cubeVao = 0;
}

void TextureExample::OnResize(int width, int height)
{
    glViewport(0, 0, width, height);
    m_aspect = (height > 0) ? static_cast<float>(width) / static_cast<float>(height) : 1.0f;
    m_camera.SetAspect(m_aspect);
}

void TextureExample::OnKey(int key, int scancode, int action, int mods)
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

void TextureExample::OnMouseButton(int button, int action, int mods)
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

void TextureExample::OnCursorMove(double x, double y)
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

void TextureExample::ResetCamera()
{
    m_camera.Reset();
    m_camera.SetAspect(m_aspect);
    m_sunLightEnabled = true;
    m_pointLightsEnabled = true;
    m_pointLightOrbitEnabled = true;
    m_pointLightOrbitDegrees = 0.0f;
    UpdatePointLightPositions();
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

void TextureExample::UpdatePointLightPositions()
{
    for (size_t i = 0; i < m_pointLights.size(); ++i)
    {
        const PointLightMotion& motion = kPointLightMotions[i];
        const float angleDegrees = m_pointLightOrbitDegrees * motion.orbitSpeedScale + motion.angleOffsetDegrees;
        const float angleRadians = angleDegrees * (kPi / 180.0f);
        const float bobRadians = angleRadians * motion.bobSpeedScale;
        const float height = motion.heightOffset + std::sin(bobRadians) * motion.heightAmplitude;

        m_pointLights[i].position = vmath::vec3(std::cos(angleRadians) * motion.radius,
                                                height,
                                                std::sin(angleRadians) * motion.radius);
    }
}

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    TextureExample app;
    AppConfig config {};
    config.title = "02-04 Texture";
    return app.Run(config);
}
