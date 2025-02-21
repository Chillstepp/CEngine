cmake_minimum_required(VERSION 3.23)

set(SHADERS
        00_hello_triangle.vert
        00_hello_triangle.frag
)

spirv_shaders(
        SHADER_HEADERS
        SPIRV_VERSION
        1.0 # Currently targeting Vulkan 1.0
        SOURCES
        ${SHADERS}
)

add_executable(RenderCube
        ${SHADER_HEADERS}
        main.cpp
)

target_link_libraries(RenderCube PRIVATE CECore)

