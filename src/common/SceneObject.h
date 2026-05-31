#ifndef __SCENE_OBJECT_H__
#define __SCENE_OBJECT_H__

#include <string>
#include <vector>

#include "Transform.h"

class Geometry;

class SceneObject
{
public:
    SceneObject() = default;
    explicit SceneObject(std::string name);
    virtual ~SceneObject();

    const std::string& GetName() const { return m_name; }
    void SetName(std::string name);

    bool IsVisible() const { return m_visible; }
    void SetVisible(bool visible);

    Transform& GetTransform() { return m_transform; }
    const Transform& GetTransform() const { return m_transform; }

    void SetGeometry(Geometry* geometry);
    Geometry* GetGeometry() { return m_geometry; }
    const Geometry* GetGeometry() const { return m_geometry; }

    void SetParent(SceneObject* parent);
    SceneObject* GetParent() const { return m_parent; }

    void AddChild(SceneObject& child);
    void RemoveChild(SceneObject& child);
    const std::vector<SceneObject*>& GetChildren() const { return m_children; }

    vmath::mat4 GetWorldMatrix() const;

private:
    std::string m_name {};
    bool m_visible = true;
    Transform m_transform {};
    Geometry* m_geometry = nullptr;
    SceneObject* m_parent = nullptr;
    std::vector<SceneObject*> m_children {};

    void DetachFromParent();
};

#endif /* __SCENE_OBJECT_H__ */
