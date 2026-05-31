#include "Camera.h"

#include <cmath>

namespace
{
constexpr float kPitchLimit = 89.0f;
constexpr float kPi = 3.1415926535f;

float DegreesToRadians(float degrees)
{
    return degrees * (kPi / 180.0f);
}
} // namespace

void Camera::Reset()
{
    m_position = vmath::vec3(0.0f, 2.5f, 8.0f);
    m_worldUp = vmath::vec3(0.0f, 1.0f, 0.0f);
    m_yaw = -90.0f;
    m_pitch = -15.0f;
    m_fovY = 60.0f;
    m_aspect = 16.0f / 9.0f;
    m_nearPlane = 0.1f;
    m_farPlane = 200.0f;
    UpdateBasisVectors();
    m_viewDirty = true;
    m_projectionDirty = true;
}

void Camera::SetPosition(const vmath::vec3& position)
{
    m_position = position;
    m_viewDirty = true;
}

void Camera::Move(const vmath::vec3& delta)
{
    m_position += delta;
    m_viewDirty = true;
}

void Camera::SetYawPitch(float yaw, float pitch)
{
    m_yaw = yaw;
    m_pitch = ClampPitch(pitch);
    UpdateBasisVectors();
    m_viewDirty = true;
}

void Camera::AddYawPitch(float yawDelta, float pitchDelta)
{
    SetYawPitch(m_yaw + yawDelta, m_pitch + pitchDelta);
}

void Camera::SetPerspective(float fovY, float aspect, float nearPlane, float farPlane)
{
    m_fovY = fovY;
    m_aspect = aspect;
    m_nearPlane = nearPlane;
    m_farPlane = farPlane;
    m_projectionDirty = true;
}

void Camera::SetAspect(float aspect)
{
    m_aspect = aspect;
    m_projectionDirty = true;
}

const vmath::mat4& Camera::GetViewMatrix()
{
    if (m_viewDirty)
    {
        UpdateViewMatrix();
    }

    return m_viewMatrix;
}

const vmath::mat4& Camera::GetProjectionMatrix()
{
    if (m_projectionDirty)
    {
        UpdateProjectionMatrix();
    }

    return m_projectionMatrix;
}

void Camera::UpdateBasisVectors()
{
    const float yawRadians = DegreesToRadians(m_yaw);
    const float pitchRadians = DegreesToRadians(m_pitch);

    m_forward = vmath::normalize(vmath::vec3(std::cos(yawRadians) * std::cos(pitchRadians),
                                             std::sin(pitchRadians),
                                             std::sin(yawRadians) * std::cos(pitchRadians)));
    m_right = vmath::normalize(vmath::cross(m_forward, m_worldUp));
    m_up = vmath::normalize(vmath::cross(m_right, m_forward));
}

void Camera::UpdateViewMatrix()
{
    m_viewMatrix = vmath::lookat(m_position, m_position + m_forward, m_up);
    m_viewDirty = false;
}

void Camera::UpdateProjectionMatrix()
{
    m_projectionMatrix = vmath::perspective(m_fovY, m_aspect, m_nearPlane, m_farPlane);
    m_projectionDirty = false;
}

float Camera::ClampPitch(float pitch)
{
    if (pitch > kPitchLimit)
    {
        return kPitchLimit;
    }

    if (pitch < -kPitchLimit)
    {
        return -kPitchLimit;
    }

    return pitch;
}
