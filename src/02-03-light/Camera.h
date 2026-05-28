#ifndef __02_03_LIGHT_CAMERA_H__
#define __02_03_LIGHT_CAMERA_H__

#include "vmath.h"

class Camera
{
private:
    vmath::vec3 m_position { 0.0f, 2.5f, 8.0f };
    vmath::vec3 m_worldUp { 0.0f, 1.0f, 0.0f };

    float m_yaw = -90.0f;
    float m_pitch = -15.0f;

    float m_fovY = 60.0f;
    float m_aspect = 16.0f / 9.0f;
    float m_nearPlane = 0.1f;
    float m_farPlane = 200.0f;

    vmath::vec3 m_forward { 0.0f, 0.0f, -1.0f };
    vmath::vec3 m_right { 1.0f, 0.0f, 0.0f };
    vmath::vec3 m_up { 0.0f, 1.0f, 0.0f };

    bool m_viewDirty = true;
    bool m_projectionDirty = true;

    vmath::mat4 m_viewMatrix = vmath::mat4::identity();
    vmath::mat4 m_projectionMatrix = vmath::mat4::identity();

public:
    Camera()
    {
        Reset();
    }

    void Reset();

    void SetPosition(const vmath::vec3& position);
    void Move(const vmath::vec3& delta);

    void SetYawPitch(float yaw, float pitch);
    void AddYawPitch(float yawDelta, float pitchDelta);

    void SetPerspective(float fovY, float aspect, float nearPlane, float farPlane);
    void SetAspect(float aspect);

    const vmath::vec3& GetPosition() const { return m_position; }
    float GetYaw() const { return m_yaw; }
    float GetPitch() const { return m_pitch; }

    const vmath::vec3& GetForward() const { return m_forward; }
    const vmath::vec3& GetRight() const { return m_right; }
    const vmath::vec3& GetUp() const { return m_up; }

    const vmath::mat4& GetViewMatrix();
    const vmath::mat4& GetProjectionMatrix();

private:
    void UpdateBasisVectors();
    void UpdateViewMatrix();
    void UpdateProjectionMatrix();
    static float ClampPitch(float pitch);
};

#endif /* __02_03_LIGHT_CAMERA_H__ */
