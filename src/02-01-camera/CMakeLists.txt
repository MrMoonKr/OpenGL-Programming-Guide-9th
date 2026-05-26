get_filename_component(example_name "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
add_vermilion_example("${example_name}")
target_sources("${example_name}" PRIVATE camera.cpp camera.h)
