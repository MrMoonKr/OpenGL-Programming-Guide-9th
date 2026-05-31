include_guard(GLOBAL)

include(cmake/Dependencies.cmake)

if(TARGET stb_image)
    return()
endif()

dep_resolve_paths("stb" dep_source_dir dep_binary_dir)

dep_declare_fetchcontent(
    stb
    "https://github.com/nothings/stb.git"
    "31c1ad37456438565541f4919958214b6e762fb4"
    "${dep_source_dir}"
    "${dep_binary_dir}"
)
FetchContent_MakeAvailable(stb)

add_library(stb_image STATIC "${CMAKE_SOURCE_DIR}/src/common/stb_image.cpp")
add_library(stb::stb_image ALIAS stb_image)

target_include_directories(
    stb_image
    PUBLIC
        "${dep_source_dir}"
)

if(MSVC)
    target_compile_options(stb_image PRIVATE /utf-8)
endif()
