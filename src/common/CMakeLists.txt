add_library(
    vermilion STATIC
    LoadShaders.cpp
    loadtexture.cpp
    targa.cpp
    vbm.cpp
    vdds.cpp
    vermilion.cpp
)

target_include_directories(vermilion PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(vermilion PUBLIC glad glfw)

if(MSVC)
    target_compile_options(vermilion PRIVATE /utf-8)
endif()
