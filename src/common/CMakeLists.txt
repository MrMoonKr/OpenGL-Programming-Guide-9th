add_library(
    vermilion STATIC
    CheckError.h
    LoadShaders.h
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
    LoadShaders.cpp
    loadtexture.cpp
    targa.cpp
    vbm.cpp
    vdds.cpp
    vutils.cpp
    vermilion.cpp
)

target_include_directories(vermilion PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(vermilion PUBLIC glad glfw)

if(MSVC)
    target_compile_options(vermilion PRIVATE /utf-8)
endif()
