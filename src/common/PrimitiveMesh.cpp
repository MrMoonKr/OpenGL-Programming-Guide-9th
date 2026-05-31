#include "PrimitiveMesh.h"

#include <algorithm>
#include <cstddef>
#include <cmath>

namespace
{
constexpr float kPi = 3.1415926535f;
constexpr float kTwoPi = 2.0f * kPi;

PrimitiveVertex MakeVertex(const vmath::vec3& position,
                           const vmath::vec3& normal,
                           const vmath::vec2& texCoord)
{
    PrimitiveVertex vertex;
    vertex.position[0] = position[0];
    vertex.position[1] = position[1];
    vertex.position[2] = position[2];
    vertex.normal[0] = normal[0];
    vertex.normal[1] = normal[1];
    vertex.normal[2] = normal[2];
    vertex.texCoord[0] = texCoord[0];
    vertex.texCoord[1] = texCoord[1];
    return vertex;
}

MeshData MakeMesh(std::vector<PrimitiveVertex>&& vertices,
                  GLenum primitiveMode,
                  std::vector<std::uint32_t>&& indices = {})
{
    MeshData mesh;
    mesh.attributes = GetPrimitiveVertexAttributes();
    mesh.primitiveMode = primitiveMode;
    mesh.SetVertices(vertices);

    if (!indices.empty())
    {
        mesh.SetIndices(indices);
    }

    return mesh;
}

vmath::vec3 CatmullRom(const vmath::vec3& p0,
                       const vmath::vec3& p1,
                       const vmath::vec3& p2,
                       const vmath::vec3& p3,
                       float t)
{
    const float t2 = t * t;
    const float t3 = t2 * t;

    return 0.5f * ((2.0f * p1) +
                   (-p0 + p2) * t +
                   (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
                   (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3);
}
} // namespace

std::vector<VertexAttribute> GetPrimitiveVertexAttributes()
{
    return {
        VertexAttribute { 0, 3, GL_FLOAT, GL_FALSE, sizeof(PrimitiveVertex), offsetof(PrimitiveVertex, position) },
        VertexAttribute { 1, 3, GL_FLOAT, GL_FALSE, sizeof(PrimitiveVertex), offsetof(PrimitiveVertex, normal) },
        VertexAttribute { 2, 2, GL_FLOAT, GL_FALSE, sizeof(PrimitiveVertex), offsetof(PrimitiveVertex, texCoord) }
    };
}

namespace PrimitiveMesh
{
MeshData MakeBox(float width, float height, float depth)
{
    const float halfWidth = width * 0.5f;
    const float halfHeight = height * 0.5f;
    const float halfDepth = depth * 0.5f;

    std::vector<PrimitiveVertex> vertices;
    std::vector<std::uint32_t> indices;
    vertices.reserve(24);
    indices.reserve(36);

    const auto appendQuad = [&vertices, &indices](const vmath::vec3& a,
                                                  const vmath::vec3& b,
                                                  const vmath::vec3& c,
                                                  const vmath::vec3& d,
                                                  const vmath::vec3& normal)
    {
        const std::uint32_t base = static_cast<std::uint32_t>(vertices.size());
        vertices.push_back(MakeVertex(a, normal, vmath::vec2(0.0f, 0.0f)));
        vertices.push_back(MakeVertex(b, normal, vmath::vec2(1.0f, 0.0f)));
        vertices.push_back(MakeVertex(c, normal, vmath::vec2(1.0f, 1.0f)));
        vertices.push_back(MakeVertex(d, normal, vmath::vec2(0.0f, 1.0f)));

        indices.push_back(base + 0);
        indices.push_back(base + 1);
        indices.push_back(base + 2);
        indices.push_back(base + 0);
        indices.push_back(base + 2);
        indices.push_back(base + 3);
    };

    appendQuad(vmath::vec3(-halfWidth, -halfHeight,  halfDepth),
               vmath::vec3( halfWidth, -halfHeight,  halfDepth),
               vmath::vec3( halfWidth,  halfHeight,  halfDepth),
               vmath::vec3(-halfWidth,  halfHeight,  halfDepth),
               vmath::vec3(0.0f, 0.0f, 1.0f));

    appendQuad(vmath::vec3( halfWidth, -halfHeight, -halfDepth),
               vmath::vec3(-halfWidth, -halfHeight, -halfDepth),
               vmath::vec3(-halfWidth,  halfHeight, -halfDepth),
               vmath::vec3( halfWidth,  halfHeight, -halfDepth),
               vmath::vec3(0.0f, 0.0f, -1.0f));

    appendQuad(vmath::vec3(-halfWidth, -halfHeight, -halfDepth),
               vmath::vec3(-halfWidth, -halfHeight,  halfDepth),
               vmath::vec3(-halfWidth,  halfHeight,  halfDepth),
               vmath::vec3(-halfWidth,  halfHeight, -halfDepth),
               vmath::vec3(-1.0f, 0.0f, 0.0f));

    appendQuad(vmath::vec3( halfWidth, -halfHeight,  halfDepth),
               vmath::vec3( halfWidth, -halfHeight, -halfDepth),
               vmath::vec3( halfWidth,  halfHeight, -halfDepth),
               vmath::vec3( halfWidth,  halfHeight,  halfDepth),
               vmath::vec3(1.0f, 0.0f, 0.0f));

    appendQuad(vmath::vec3(-halfWidth,  halfHeight,  halfDepth),
               vmath::vec3( halfWidth,  halfHeight,  halfDepth),
               vmath::vec3( halfWidth,  halfHeight, -halfDepth),
               vmath::vec3(-halfWidth,  halfHeight, -halfDepth),
               vmath::vec3(0.0f, 1.0f, 0.0f));

    appendQuad(vmath::vec3(-halfWidth, -halfHeight, -halfDepth),
               vmath::vec3( halfWidth, -halfHeight, -halfDepth),
               vmath::vec3( halfWidth, -halfHeight,  halfDepth),
               vmath::vec3(-halfWidth, -halfHeight,  halfDepth),
               vmath::vec3(0.0f, -1.0f, 0.0f));

    return MakeMesh(std::move(vertices), GL_TRIANGLES, std::move(indices));
}

MeshData MakeCircle(float radius, std::uint32_t segments, bool filled)
{
    segments = std::max<std::uint32_t>(segments, 3);
    const float uvScale = radius != 0.0f ? 1.0f / (2.0f * radius) : 0.0f;

    std::vector<PrimitiveVertex> vertices;
    std::vector<std::uint32_t> indices;

    if (filled)
    {
        vertices.reserve(static_cast<std::size_t>(segments) + 1);
        indices.reserve(static_cast<std::size_t>(segments) * 3);

        vertices.push_back(MakeVertex(vmath::vec3(0.0f, 0.0f, 0.0f),
                                      vmath::vec3(0.0f, 1.0f, 0.0f),
                                      vmath::vec2(0.5f, 0.5f)));

        for (std::uint32_t i = 0; i < segments; ++i)
        {
            const float angle = kTwoPi * static_cast<float>(i) / static_cast<float>(segments);
            const float x = std::cos(angle) * radius;
            const float z = std::sin(angle) * radius;
            vertices.push_back(MakeVertex(vmath::vec3(x, 0.0f, z),
                                          vmath::vec3(0.0f, 1.0f, 0.0f),
                                          vmath::vec2((x * uvScale) + 0.5f,
                                                      (z * uvScale) + 0.5f)));
        }

        for (std::uint32_t i = 0; i < segments; ++i)
        {
            indices.push_back(0);
            indices.push_back(1 + i);
            indices.push_back(1 + ((i + 1) % segments));
        }

        return MakeMesh(std::move(vertices), GL_TRIANGLES, std::move(indices));
    }

    vertices.reserve(segments);
    for (std::uint32_t i = 0; i < segments; ++i)
    {
        const float angle = kTwoPi * static_cast<float>(i) / static_cast<float>(segments);
        const float x = std::cos(angle) * radius;
        const float z = std::sin(angle) * radius;
        vertices.push_back(MakeVertex(vmath::vec3(x, 0.0f, z),
                                      vmath::vec3(0.0f, 1.0f, 0.0f),
                                      vmath::vec2((x * uvScale) + 0.5f,
                                                  (z * uvScale) + 0.5f)));
    }

    return MakeMesh(std::move(vertices), GL_LINE_LOOP);
}

MeshData MakeCone(float radius, float height, std::uint32_t slices, bool capBottom)
{
    slices = std::max<std::uint32_t>(slices, 3);

    const float halfHeight = height * 0.5f;
    const float slope = height != 0.0f ? radius / height : 0.0f;
    const float uvScale = radius != 0.0f ? 1.0f / (2.0f * radius) : 0.0f;

    std::vector<PrimitiveVertex> vertices;
    std::vector<std::uint32_t> indices;
    vertices.reserve(static_cast<std::size_t>(slices + 1) * 2 + (capBottom ? slices + 1 : 0));
    indices.reserve(static_cast<std::size_t>(slices) * (capBottom ? 6 : 3));

    for (std::uint32_t i = 0; i <= slices; ++i)
    {
        const float u = static_cast<float>(i) / static_cast<float>(slices);
        const float angle = kTwoPi * u;
        const float x = std::cos(angle);
        const float z = std::sin(angle);
        const vmath::vec3 normal = vmath::normalize(vmath::vec3(x, slope, z));

        vertices.push_back(MakeVertex(vmath::vec3(0.0f, halfHeight, 0.0f),
                                      normal,
                                      vmath::vec2(u, 1.0f)));
        vertices.push_back(MakeVertex(vmath::vec3(x * radius, -halfHeight, z * radius),
                                      normal,
                                      vmath::vec2(u, 0.0f)));
    }

    for (std::uint32_t i = 0; i < slices; ++i)
    {
        const std::uint32_t apexIndex = i * 2;
        const std::uint32_t baseIndex = apexIndex + 1;
        const std::uint32_t nextBaseIndex = baseIndex + 2;

        indices.push_back(apexIndex);
        indices.push_back(baseIndex);
        indices.push_back(nextBaseIndex);
    }

    if (capBottom)
    {
        const std::uint32_t centerIndex = static_cast<std::uint32_t>(vertices.size());
        vertices.push_back(MakeVertex(vmath::vec3(0.0f, -halfHeight, 0.0f),
                                      vmath::vec3(0.0f, -1.0f, 0.0f),
                                      vmath::vec2(0.5f, 0.5f)));

        for (std::uint32_t i = 0; i < slices; ++i)
        {
            const float angle = kTwoPi * static_cast<float>(i) / static_cast<float>(slices);
            const float x = std::cos(angle) * radius;
            const float z = std::sin(angle) * radius;
            vertices.push_back(MakeVertex(vmath::vec3(x, -halfHeight, z),
                                          vmath::vec3(0.0f, -1.0f, 0.0f),
                                          vmath::vec2((x * uvScale) + 0.5f,
                                                      (z * uvScale) + 0.5f)));
        }

        for (std::uint32_t i = 0; i < slices; ++i)
        {
            indices.push_back(centerIndex);
            indices.push_back(centerIndex + 1 + ((i + 1) % slices));
            indices.push_back(centerIndex + 1 + i);
        }
    }

    return MakeMesh(std::move(vertices), GL_TRIANGLES, std::move(indices));
}

MeshData MakeCurve(const std::vector<vmath::vec3>& controlPoints, std::uint32_t segmentsPerSpan)
{
    MeshData mesh;

    if (controlPoints.size() < 2)
    {
        return mesh;
    }

    segmentsPerSpan = std::max<std::uint32_t>(segmentsPerSpan, 1);

    std::vector<PrimitiveVertex> vertices;

    if (controlPoints.size() == 2)
    {
        vertices.push_back(MakeVertex(controlPoints[0], vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec2(0.0f, 0.0f)));
        vertices.push_back(MakeVertex(controlPoints[1], vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec2(1.0f, 0.0f)));
        return MakeMesh(std::move(vertices), GL_LINE_STRIP);
    }

    const std::size_t spanCount = controlPoints.size() - 1;
    vertices.reserve(spanCount * segmentsPerSpan + 1);

    for (std::size_t span = 0; span < spanCount; ++span)
    {
        const vmath::vec3& p0 = controlPoints[span > 0 ? span - 1 : span];
        const vmath::vec3& p1 = controlPoints[span];
        const vmath::vec3& p2 = controlPoints[span + 1];
        const vmath::vec3& p3 = controlPoints[(span + 2) < controlPoints.size() ? span + 2 : span + 1];

        for (std::uint32_t segment = 0; segment < segmentsPerSpan; ++segment)
        {
            const float t = static_cast<float>(segment) / static_cast<float>(segmentsPerSpan);
            const float u = (static_cast<float>(span) + t) / static_cast<float>(spanCount);
            vertices.push_back(MakeVertex(CatmullRom(p0, p1, p2, p3, t),
                                          vmath::vec3(0.0f, 0.0f, 0.0f),
                                          vmath::vec2(u, 0.0f)));
        }
    }

    vertices.push_back(MakeVertex(controlPoints.back(), vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec2(1.0f, 0.0f)));

    return MakeMesh(std::move(vertices), GL_LINE_STRIP);
}

MeshData MakeCylinder(float radius,
                      float height,
                      std::uint32_t slices,
                      bool capTop,
                      bool capBottom)
{
    slices = std::max<std::uint32_t>(slices, 3);
    const float halfHeight = height * 0.5f;
    const float uvScale = radius != 0.0f ? 1.0f / (2.0f * radius) : 0.0f;

    std::vector<PrimitiveVertex> vertices;
    std::vector<std::uint32_t> indices;

    vertices.reserve(static_cast<std::size_t>(slices + 1) * 2 +
                     (capTop ? slices + 1 : 0) +
                     (capBottom ? slices + 1 : 0));
    indices.reserve(static_cast<std::size_t>(slices) * 6 +
                    (capTop ? static_cast<std::size_t>(slices) * 3 : 0) +
                    (capBottom ? static_cast<std::size_t>(slices) * 3 : 0));

    for (std::uint32_t i = 0; i <= slices; ++i)
    {
        const float u = static_cast<float>(i) / static_cast<float>(slices);
        const float angle = kTwoPi * u;
        const float x = std::cos(angle);
        const float z = std::sin(angle);
        const vmath::vec3 normal = vmath::normalize(vmath::vec3(x, 0.0f, z));

        vertices.push_back(MakeVertex(vmath::vec3(x * radius, halfHeight, z * radius),
                                      normal,
                                      vmath::vec2(u, 1.0f)));
        vertices.push_back(MakeVertex(vmath::vec3(x * radius, -halfHeight, z * radius),
                                      normal,
                                      vmath::vec2(u, 0.0f)));
    }

    for (std::uint32_t i = 0; i < slices; ++i)
    {
        const std::uint32_t topIndex = i * 2;
        const std::uint32_t bottomIndex = topIndex + 1;
        const std::uint32_t nextTopIndex = topIndex + 2;
        const std::uint32_t nextBottomIndex = bottomIndex + 2;

        indices.push_back(topIndex);
        indices.push_back(bottomIndex);
        indices.push_back(nextTopIndex);

        indices.push_back(nextTopIndex);
        indices.push_back(bottomIndex);
        indices.push_back(nextBottomIndex);
    }

    if (capTop)
    {
        const std::uint32_t centerIndex = static_cast<std::uint32_t>(vertices.size());
        vertices.push_back(MakeVertex(vmath::vec3(0.0f, halfHeight, 0.0f),
                                      vmath::vec3(0.0f, 1.0f, 0.0f),
                                      vmath::vec2(0.5f, 0.5f)));

        for (std::uint32_t i = 0; i < slices; ++i)
        {
            const float angle = kTwoPi * static_cast<float>(i) / static_cast<float>(slices);
            const float x = std::cos(angle) * radius;
            const float z = std::sin(angle) * radius;
            vertices.push_back(MakeVertex(vmath::vec3(x, halfHeight, z),
                                          vmath::vec3(0.0f, 1.0f, 0.0f),
                                          vmath::vec2((x * uvScale) + 0.5f,
                                                      (z * uvScale) + 0.5f)));
        }

        for (std::uint32_t i = 0; i < slices; ++i)
        {
            indices.push_back(centerIndex);
            indices.push_back(centerIndex + 1 + i);
            indices.push_back(centerIndex + 1 + ((i + 1) % slices));
        }
    }

    if (capBottom)
    {
        const std::uint32_t centerIndex = static_cast<std::uint32_t>(vertices.size());
        vertices.push_back(MakeVertex(vmath::vec3(0.0f, -halfHeight, 0.0f),
                                      vmath::vec3(0.0f, -1.0f, 0.0f),
                                      vmath::vec2(0.5f, 0.5f)));

        for (std::uint32_t i = 0; i < slices; ++i)
        {
            const float angle = kTwoPi * static_cast<float>(i) / static_cast<float>(slices);
            const float x = std::cos(angle) * radius;
            const float z = std::sin(angle) * radius;
            vertices.push_back(MakeVertex(vmath::vec3(x, -halfHeight, z),
                                          vmath::vec3(0.0f, -1.0f, 0.0f),
                                          vmath::vec2((x * uvScale) + 0.5f,
                                                      (z * uvScale) + 0.5f)));
        }

        for (std::uint32_t i = 0; i < slices; ++i)
        {
            indices.push_back(centerIndex);
            indices.push_back(centerIndex + 1 + ((i + 1) % slices));
            indices.push_back(centerIndex + 1 + i);
        }
    }

    return MakeMesh(std::move(vertices), GL_TRIANGLES, std::move(indices));
}

MeshData MakeLine(const vmath::vec3& start, const vmath::vec3& end)
{
    std::vector<PrimitiveVertex> vertices;
    vertices.reserve(2);
    vertices.push_back(MakeVertex(start, vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec2(0.0f, 0.0f)));
    vertices.push_back(MakeVertex(end, vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec2(1.0f, 0.0f)));
    return MakeMesh(std::move(vertices), GL_LINES);
}

MeshData MakeSphere(float radius, std::uint32_t slices, std::uint32_t stacks)
{
    slices = std::max<std::uint32_t>(slices, 3);
    stacks = std::max<std::uint32_t>(stacks, 2);

    std::vector<PrimitiveVertex> vertices;
    std::vector<std::uint32_t> indices;
    vertices.reserve(static_cast<std::size_t>(stacks + 1) * static_cast<std::size_t>(slices + 1));
    indices.reserve(static_cast<std::size_t>(stacks) * static_cast<std::size_t>(slices) * 6);

    for (std::uint32_t stack = 0; stack <= stacks; ++stack)
    {
        const float v = static_cast<float>(stack) / static_cast<float>(stacks);
        const float phi = v * kPi;
        const float y = std::cos(phi) * radius;
        const float ringRadius = std::sin(phi) * radius;

        for (std::uint32_t slice = 0; slice <= slices; ++slice)
        {
            const float u = static_cast<float>(slice) / static_cast<float>(slices);
            const float theta = u * kTwoPi;
            const float x = std::cos(theta) * ringRadius;
            const float z = std::sin(theta) * ringRadius;
            const vmath::vec3 position(x, y, z);
            const vmath::vec3 normal = radius != 0.0f ? vmath::normalize(position) : vmath::vec3(0.0f, 1.0f, 0.0f);
            vertices.push_back(MakeVertex(position, normal, vmath::vec2(u, 1.0f - v)));
        }
    }

    const std::uint32_t stride = slices + 1;
    for (std::uint32_t stack = 0; stack < stacks; ++stack)
    {
        for (std::uint32_t slice = 0; slice < slices; ++slice)
        {
            const std::uint32_t a = stack * stride + slice;
            const std::uint32_t b = a + stride;

            indices.push_back(a);
            indices.push_back(b);
            indices.push_back(a + 1);

            indices.push_back(a + 1);
            indices.push_back(b);
            indices.push_back(b + 1);
        }
    }

    return MakeMesh(std::move(vertices), GL_TRIANGLES, std::move(indices));
}
} // namespace PrimitiveMesh
