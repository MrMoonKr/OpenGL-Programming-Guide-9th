#include "Geometry.h"

#include <cstdint>
#include <utility>

namespace
{
std::size_t GetIndexTypeSize(GLenum indexType)
{
    switch (indexType)
    {
        case GL_UNSIGNED_BYTE:
            return sizeof(GLubyte);
        case GL_UNSIGNED_SHORT:
            return sizeof(GLushort);
        case GL_UNSIGNED_INT:
        default:
            return sizeof(GLuint);
    }
}
} // namespace

Geometry::~Geometry()
{
    Destroy();
}

Geometry::Geometry(Geometry&& other) noexcept
    : m_vao(std::exchange(other.m_vao, 0)),
      m_vbo(std::exchange(other.m_vbo, 0)),
      m_ebo(std::exchange(other.m_ebo, 0)),
      m_primitiveMode(other.m_primitiveMode),
      m_indexType(other.m_indexType),
      m_vertexCount(other.m_vertexCount),
      m_indexCount(other.m_indexCount)
{
    other.m_primitiveMode = GL_TRIANGLES;
    other.m_indexType = GL_UNSIGNED_INT;
    other.m_vertexCount = 0;
    other.m_indexCount = 0;
}

Geometry& Geometry::operator=(Geometry&& other) noexcept
{
    if (this != &other)
    {
        Destroy();

        m_vao = std::exchange(other.m_vao, 0);
        m_vbo = std::exchange(other.m_vbo, 0);
        m_ebo = std::exchange(other.m_ebo, 0);
        m_primitiveMode = other.m_primitiveMode;
        m_indexType = other.m_indexType;
        m_vertexCount = other.m_vertexCount;
        m_indexCount = other.m_indexCount;

        other.m_primitiveMode = GL_TRIANGLES;
        other.m_indexType = GL_UNSIGNED_INT;
        other.m_vertexCount = 0;
        other.m_indexCount = 0;
    }

    return *this;
}

bool Geometry::Create(const MeshData& mesh)
{
    if (!mesh.HasVertices() || mesh.attributes.empty())
    {
        return false;
    }

    Destroy();

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 mesh.GetVertexDataSize(),
                 mesh.vertexData.data(),
                 mesh.usage);

    if (mesh.HasIndices())
    {
        glGenBuffers(1, &m_ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     mesh.GetIndexDataSize(),
                     mesh.indexData.data(),
                     mesh.usage);
    }

    for (const VertexAttribute& attribute : mesh.attributes)
    {
        if (!attribute.enabled)
        {
            continue;
        }

        glEnableVertexAttribArray(attribute.index);

        if (attribute.integer)
        {
            glVertexAttribIPointer(attribute.index,
                                   attribute.components,
                                   attribute.type,
                                   attribute.stride,
                                   reinterpret_cast<const void*>(static_cast<std::uintptr_t>(attribute.offset)));
        }
        else
        {
            glVertexAttribPointer(attribute.index,
                                  attribute.components,
                                  attribute.type,
                                  attribute.normalized,
                                  attribute.stride,
                                  reinterpret_cast<const void*>(static_cast<std::uintptr_t>(attribute.offset)));
        }

        if (attribute.divisor != 0)
        {
            glVertexAttribDivisor(attribute.index, attribute.divisor);
        }
    }

    m_primitiveMode = mesh.primitiveMode;
    m_indexType = mesh.indexType;
    m_vertexCount = mesh.vertexCount;
    m_indexCount = mesh.indexCount;

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return m_vao != 0 && m_vbo != 0;
}

void Geometry::Destroy()
{
    glDeleteBuffers(1, &m_ebo);
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);

    m_vao = 0;
    m_vbo = 0;
    m_ebo = 0;
    m_primitiveMode = GL_TRIANGLES;
    m_indexType = GL_UNSIGNED_INT;
    m_vertexCount = 0;
    m_indexCount = 0;
}

void Geometry::Bind() const
{
    glBindVertexArray(m_vao);
}

void Geometry::Draw() const
{
    if (IsIndexed())
    {
        DrawIndices();
    }
    else
    {
        DrawVertices(0, m_vertexCount);
    }
}

void Geometry::DrawVertices(GLint first, GLsizei count) const
{
    if (m_vao == 0 || count <= 0)
    {
        return;
    }

    Bind();
    glDrawArrays(m_primitiveMode, first, count);
}

void Geometry::DrawIndices(GLsizei count, std::size_t firstIndex) const
{
    if (m_vao == 0 || !IsIndexed())
    {
        return;
    }

    const GLsizei drawCount = count > 0 ? count : m_indexCount;
    const std::size_t offsetBytes = firstIndex * GetIndexTypeSize(m_indexType);

    Bind();
    glDrawElements(m_primitiveMode,
                   drawCount,
                   m_indexType,
                   reinterpret_cast<const void*>(static_cast<std::uintptr_t>(offsetBytes)));
}

void Geometry::DrawElementsBaseVertex(GLint baseVertex, GLsizei count, std::size_t firstIndex) const
{
    if (m_vao == 0 || !IsIndexed())
    {
        return;
    }

    const GLsizei drawCount = count > 0 ? count : m_indexCount;
    const std::size_t offsetBytes = firstIndex * GetIndexTypeSize(m_indexType);

    Bind();
    glDrawElementsBaseVertex(m_primitiveMode,
                             drawCount,
                             m_indexType,
                             reinterpret_cast<const void*>(static_cast<std::uintptr_t>(offsetBytes)),
                             baseVertex);
}

void Geometry::DrawInstanced(GLsizei instanceCount) const
{
    if (m_vao == 0 || instanceCount <= 0)
    {
        return;
    }

    Bind();

    if (IsIndexed())
    {
        glDrawElementsInstanced(m_primitiveMode,
                                m_indexCount,
                                m_indexType,
                                nullptr,
                                instanceCount);
    }
    else
    {
        glDrawArraysInstanced(m_primitiveMode, 0, m_vertexCount, instanceCount);
    }
}

void Geometry::DrawVerticesInstanced(GLint first, GLsizei count, GLsizei instanceCount) const
{
    if (m_vao == 0 || count <= 0 || instanceCount <= 0)
    {
        return;
    }

    Bind();
    glDrawArraysInstanced(m_primitiveMode, first, count, instanceCount);
}
