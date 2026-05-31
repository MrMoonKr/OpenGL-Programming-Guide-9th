#ifndef __MESH_DATA_H__
#define __MESH_DATA_H__

#include <cstdint>
#include <cstring>
#include <type_traits>
#include <vector>

#include "VertexAttribute.h"

struct MeshData
{
    std::vector<std::uint8_t> vertexData {};
    std::vector<std::uint8_t> indexData {};
    std::vector<VertexAttribute> attributes {};

    GLenum primitiveMode = GL_TRIANGLES;
    GLenum usage = GL_STATIC_DRAW;
    GLenum indexType = GL_UNSIGNED_INT;

    GLsizei vertexCount = 0;
    GLsizei indexCount = 0;

    bool HasVertices() const
    {
        return vertexCount > 0 && !vertexData.empty();
    }

    bool HasIndices() const
    {
        return indexCount > 0 && !indexData.empty();
    }

    GLsizeiptr GetVertexDataSize() const
    {
        return static_cast<GLsizeiptr>(vertexData.size());
    }

    GLsizeiptr GetIndexDataSize() const
    {
        return static_cast<GLsizeiptr>(indexData.size());
    }

    void Clear()
    {
        vertexData.clear();
        indexData.clear();
        attributes.clear();
        primitiveMode = GL_TRIANGLES;
        usage = GL_STATIC_DRAW;
        indexType = GL_UNSIGNED_INT;
        vertexCount = 0;
        indexCount = 0;
    }

    template <typename TVertex>
    void SetVertices(const std::vector<TVertex>& vertices)
    {
        static_assert(std::is_trivially_copyable_v<TVertex>,
                      "MeshData::SetVertices requires trivially copyable vertex data.");

        vertexCount = static_cast<GLsizei>(vertices.size());
        vertexData.resize(sizeof(TVertex) * vertices.size());

        if (!vertices.empty())
        {
            std::memcpy(vertexData.data(), vertices.data(), vertexData.size());
        }
    }

    template <typename TIndex>
    void SetIndices(const std::vector<TIndex>& indices)
    {
        static_assert(std::is_trivially_copyable_v<TIndex>,
                      "MeshData::SetIndices requires trivially copyable index data.");

        indexType = ResolveIndexType<TIndex>();
        indexCount = static_cast<GLsizei>(indices.size());
        indexData.resize(sizeof(TIndex) * indices.size());

        if (!indices.empty())
        {
            std::memcpy(indexData.data(), indices.data(), indexData.size());
        }
    }

private:
    template <typename T>
    struct dependent_false : std::false_type
    {
    };

    template <typename TIndex>
    static constexpr GLenum ResolveIndexType()
    {
        if constexpr (std::is_same_v<TIndex, std::uint8_t>)
        {
            return GL_UNSIGNED_BYTE;
        }
        else if constexpr (std::is_same_v<TIndex, std::uint16_t>)
        {
            return GL_UNSIGNED_SHORT;
        }
        else if constexpr (std::is_same_v<TIndex, std::uint32_t>)
        {
            return GL_UNSIGNED_INT;
        }
        else
        {
            static_assert(dependent_false<TIndex>::value,
                          "MeshData::SetIndices only supports uint8_t, uint16_t, or uint32_t.");
            return GL_UNSIGNED_INT;
        }
    }
};

#endif /* __MESH_DATA_H__ */
