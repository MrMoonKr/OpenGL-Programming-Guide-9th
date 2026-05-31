add_library(
    vermilion STATIC
    Camera.h
    CheckError.h
    Geometry.h
    Light.h
    LoadShaders.h
    MeshData.h
    PrimitiveMesh.h
    SceneObject.h
    Shader.h
    ShaderProgram.h
    Texture.h
    Transform.h
    VertexAttribute.h
    mat.h
    vapp.h
    vbm.h
    vec.h
    vermilion.h
    vgl.h
    vmath.h
    vutils.h
    Shapes/Cube.h
    Shapes/Teapot.h
    Camera.cpp
    Geometry.cpp
    Light.cpp
    LoadShaders.cpp
    PrimitiveMesh.cpp
    SceneObject.cpp
    Shader.cpp
    ShaderProgram.cpp
    Texture.cpp
    Transform.cpp
    loadtexture.cpp
    targa.cpp
    vbm.cpp
    vdds.cpp
    vutils.cpp
    vermilion.cpp
)

target_include_directories(vermilion PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(vermilion PUBLIC glad glfw PRIVATE stb::stb_image)

if(MSVC)
    target_compile_options(vermilion PRIVATE /utf-8)
endif()
