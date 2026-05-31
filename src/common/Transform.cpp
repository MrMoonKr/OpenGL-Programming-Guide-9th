#include "Transform.h"

void Transform::Reset()
{
    m_position = vmath::vec3(0.0f, 0.0f, 0.0f);
    m_rotationEuler = vmath::vec3(0.0f, 0.0f, 0.0f);
    m_scale = vmath::vec3(1.0f, 1.0f, 1.0f);
    m_localMatrix = vmath::mat4::identity();
    m_dirty = false;
}

void Transform::SetPosition(const vmath::vec3& position)
{
    m_position = position;
    MarkDirty();
}

void Transform::SetPosition(float x, float y, float z)
{
    SetPosition(vmath::vec3(x, y, z));
}

void Transform::Translate(const vmath::vec3& delta)
{
    m_position += delta;
    MarkDirty();
}

void Transform::SetRotationEuler(const vmath::vec3& rotationEulerDegrees)
{
    m_rotationEuler = rotationEulerDegrees;
    MarkDirty();
}

void Transform::SetRotationEuler(float pitchDegrees, float yawDegrees, float rollDegrees)
{
    SetRotationEuler(vmath::vec3(pitchDegrees, yawDegrees, rollDegrees));
}

void Transform::RotateEuler(const vmath::vec3& deltaDegrees)
{
    m_rotationEuler += deltaDegrees;
    MarkDirty();
}

void Transform::SetScale(const vmath::vec3& scale)
{
    m_scale = scale;
    MarkDirty();
}

void Transform::SetScale(float x, float y, float z)
{
    SetScale(vmath::vec3(x, y, z));
}

void Transform::SetUniformScale(float scale)
{
    SetScale(vmath::vec3(scale, scale, scale));
}

const vmath::mat4& Transform::GetLocalMatrix() const
{
    if (m_dirty)
    {
        UpdateLocalMatrix();
    }

    return m_localMatrix;
}

void Transform::MarkDirty()
{
    m_dirty = true;
}

void Transform::UpdateLocalMatrix() const
{
    m_localMatrix = vmath::translate(m_position) *
                    vmath::rotate(m_rotationEuler[0], m_rotationEuler[1], m_rotationEuler[2]) *
                    vmath::scale(m_scale);
    m_dirty = false;
}
