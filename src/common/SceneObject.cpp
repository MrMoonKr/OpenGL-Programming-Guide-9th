#include "SceneObject.h"

#include <algorithm>
#include <utility>

#include "Geometry.h"

namespace
{
bool IsAncestorOf(const SceneObject* ancestor, const SceneObject* object)
{
    for (const SceneObject* current = object; current != nullptr; current = current->GetParent())
    {
        if (current == ancestor)
        {
            return true;
        }
    }

    return false;
}
} // namespace

SceneObject::SceneObject(std::string name)
    : m_name(std::move(name))
{
}

SceneObject::~SceneObject()
{
    DetachFromParent();

    for (SceneObject* child : m_children)
    {
        if (child != nullptr)
        {
            child->m_parent = nullptr;
        }
    }
}

void SceneObject::SetName(std::string name)
{
    m_name = std::move(name);
}

void SceneObject::SetVisible(bool visible)
{
    m_visible = visible;
}

void SceneObject::SetGeometry(Geometry* geometry)
{
    m_geometry = geometry;
}

void SceneObject::SetParent(SceneObject* parent)
{
    if (parent == this || parent == m_parent)
    {
        return;
    }

    if (parent != nullptr && IsAncestorOf(this, parent))
    {
        return;
    }

    DetachFromParent();

    m_parent = parent;
    if (m_parent == nullptr)
    {
        return;
    }

    const auto existing = std::find(m_parent->m_children.begin(), m_parent->m_children.end(), this);
    if (existing == m_parent->m_children.end())
    {
        m_parent->m_children.push_back(this);
    }
}

void SceneObject::AddChild(SceneObject& child)
{
    child.SetParent(this);
}

void SceneObject::RemoveChild(SceneObject& child)
{
    if (child.m_parent == this)
    {
        child.SetParent(nullptr);
    }
}

vmath::mat4 SceneObject::GetWorldMatrix() const
{
    if (m_parent == nullptr)
    {
        return m_transform.GetLocalMatrix();
    }

    return m_parent->GetWorldMatrix() * m_transform.GetLocalMatrix();
}

void SceneObject::DetachFromParent()
{
    if (m_parent == nullptr)
    {
        return;
    }

    auto& siblings = m_parent->m_children;
    siblings.erase(std::remove(siblings.begin(), siblings.end(), this), siblings.end());
    m_parent = nullptr;
}
