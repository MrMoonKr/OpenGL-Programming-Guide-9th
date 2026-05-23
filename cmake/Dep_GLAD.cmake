include_guard(GLOBAL)

include(cmake/Dependencies.cmake)

if(TARGET glad)
    return()
endif()

dep_resolve_paths("glad" dep_source_dir dep_binary_dir)

FetchContent_Declare(
    glad-src
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG 658f48e72aee3c6582e80b05ac0f8787a64fe6bb
    SOURCE_DIR "${dep_source_dir}"
    BINARY_DIR "${dep_binary_dir}"
    SOURCE_SUBDIR cmake
)
FetchContent_MakeAvailable(glad-src)

glad_add_library(glad STATIC REPRODUCIBLE API gl:core=4.6)
add_library(glad::glad ALIAS glad)

if(MSVC)
    target_compile_options(glad PRIVATE /utf-8)
endif()
