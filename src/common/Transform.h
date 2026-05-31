#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include "vmath.h"

class Transform
{
public:
    Transform() = default;

    void Reset();

    void SetPosition(const vmath::vec3& position);
    void SetPosition(float x, float y, float z);
    void Translate(const vmath::vec3& delta);

    void SetRotationEuler(const vmath::vec3& rotationEulerDegrees);
    void SetRotationEuler(float pitchDegrees, float yawDegrees, float rollDegrees);
    void RotateEuler(const vmath::vec3& deltaDegrees);

    void SetScale(const vmath::vec3& scale);
    void SetScale(float x, float y, float z);
    void SetUniformScale(float scale);

    const vmath::vec3& GetPosition() const { return m_position; }
    const vmath::vec3& GetRotationEuler() const { return m_rotationEuler; }
    const vmath::vec3& GetScale() const { return m_scale; }

    const vmath::mat4& GetLocalMatrix() const;

private:
    vmath::vec3 m_position { 0.0f, 0.0f, 0.0f };
    vmath::vec3 m_rotationEuler { 0.0f, 0.0f, 0.0f };
    vmath::vec3 m_scale { 1.0f, 1.0f, 1.0f };

    mutable bool m_dirty = true;
    mutable vmath::mat4 m_localMatrix = vmath::mat4::identity();

    void MarkDirty();
    void UpdateLocalMatrix() const;
};

#endif /* __TRANSFORM_H__ */
