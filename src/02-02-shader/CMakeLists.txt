get_filename_component(example_name "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
add_vermilion_example("${example_name}")
target_sources("${example_name}" PRIVATE
    Camera.cpp
    Camera.h
    Shader.cpp
    Shader.h
    ShaderProgram.cpp
    ShaderProgram.h
)
