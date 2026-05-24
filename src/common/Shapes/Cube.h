//////////////////////////////////////////////////////////////////////////////
//
//  Cube.h - data for unit cube centered at the origin composed of
//             2 individual triangles per cube face
//
//////////////////////////////////////////////////////////////////////////////

#include <glad/gl.h>

const int NumCubeVertices = 8;
const int NumCubeFaces = 6;
const int NumCubeTriangles = 12;
const int NumCubeIndices = 3 * NumCubeTriangles;

//
//  CubeVertices - vertices for a unit cube centered at the origin
//

static const GLfloat CubeVertices[NumCubeVertices][3] = {
    { -0.5f, -0.5f, -0.5f },  // Vertex 0
    {  0.5f, -0.5f, -0.5f },  // Vertex 1
    {  0.5f,  0.5f, -0.5f },  // Vertex 2
    { -0.5f,  0.5f, -0.5f },  // Vertex 3
    { -0.5f, -0.5f,  0.5f },  // Vertex 4
    {  0.5f, -0.5f,  0.5f },  // Vertex 5
    {  0.5f,  0.5f,  0.5f },  // Vertex 6
    { -0.5f,  0.5f,  0.5f }   // Vertex 7
};

//
//  CubeIndices - indices for 12 individual triangles that make up a cube
//

static const GLushort CubeIndices[NumCubeTriangles][3] = {
    { 4, 5, 6 }, { 4, 6, 7 },  // Front
    { 0, 3, 2 }, { 0, 2, 1 },  // Back
    { 0, 4, 7 }, { 0, 7, 3 },  // Left
    { 1, 2, 6 }, { 1, 6, 5 },  // Right
    { 3, 7, 6 }, { 3, 6, 2 },  // Top
    { 0, 1, 5 }, { 0, 5, 4 }   // Bottom
};
