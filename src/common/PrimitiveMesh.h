#ifndef __PRIMITIVE_MESH_H__
#define __PRIMITIVE_MESH_H__

#include <cstdint>
#include <vector>

#include "MeshData.h"
#include "vmath.h"

struct PrimitiveVertex
{
    GLfloat position[3] { 0.0f, 0.0f, 0.0f };
    GLfloat normal[3] { 0.0f, 1.0f, 0.0f };
    GLfloat texCoord[2] { 0.0f, 0.0f };
};

std::vector<VertexAttribute> GetPrimitiveVertexAttributes();

namespace PrimitiveMesh
{
MeshData MakeBox(float width = 1.0f, float height = 1.0f, float depth = 1.0f);
MeshData MakeCircle(float radius = 0.5f, std::uint32_t segments = 32, bool filled = true);
MeshData MakeCone(float radius = 0.5f, float height = 1.0f, std::uint32_t slices = 32, bool capBottom = true);
MeshData MakeCurve(const std::vector<vmath::vec3>& controlPoints, std::uint32_t segmentsPerSpan = 16);
MeshData MakeCylinder(float radius = 0.5f,
                      float height = 1.0f,
                      std::uint32_t slices = 32,
                      bool capTop = true,
                      bool capBottom = true);
MeshData MakeLine(const vmath::vec3& start, const vmath::vec3& end);
MeshData MakeSphere(float radius = 0.5f, std::uint32_t slices = 32, std::uint32_t stacks = 16);
} // namespace PrimitiveMesh

#endif /* __PRIMITIVE_MESH_H__ */
