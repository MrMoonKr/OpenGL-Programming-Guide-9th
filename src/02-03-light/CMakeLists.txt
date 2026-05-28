get_filename_component(example_name "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
add_vermilion_example("${example_name}")
target_sources("${example_name}" PRIVATE
    02-03-light.h
    Camera.cpp
    Camera.h
    Light.cpp
    Light.h
    Shader.cpp
    Shader.h
    ShaderProgram.cpp
    ShaderProgram.h
)
