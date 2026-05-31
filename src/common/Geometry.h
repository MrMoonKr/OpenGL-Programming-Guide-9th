#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <cstddef>

#include "MeshData.h"

class Geometry
{
public:
    Geometry() = default;
    ~Geometry();

    Geometry(const Geometry&) = delete;
    Geometry& operator=(const Geometry&) = delete;

    Geometry(Geometry&& other) noexcept;
    Geometry& operator=(Geometry&& other) noexcept;

    bool Create(const MeshData& mesh);
    void Destroy();

    void Bind() const;

    bool IsValid() const { return m_vao != 0; }
    bool IsIndexed() const { return m_indexCount > 0; }

    GLenum GetPrimitiveMode() const { return m_primitiveMode; }
    GLenum GetIndexType() const { return m_indexType; }
    GLsizei GetVertexCount() const { return m_vertexCount; }
    GLsizei GetIndexCount() const { return m_indexCount; }

    void Draw() const;
    void DrawVertices(GLint first, GLsizei count) const;
    void DrawIndices(GLsizei count = 0, std::size_t firstIndex = 0) const;
    void DrawElementsBaseVertex(GLint baseVertex, GLsizei count = 0, std::size_t firstIndex = 0) const;
    void DrawInstanced(GLsizei instanceCount) const;
    void DrawVerticesInstanced(GLint first, GLsizei count, GLsizei instanceCount) const;

private:
    GLuint m_vao = 0;
    GLuint m_vbo = 0;
    GLuint m_ebo = 0;

    GLenum m_primitiveMode = GL_TRIANGLES;
    GLenum m_indexType = GL_UNSIGNED_INT;

    GLsizei m_vertexCount = 0;
    GLsizei m_indexCount = 0;
};

#endif /* __GEOMETRY_H__ */
