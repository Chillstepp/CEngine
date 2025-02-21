cmake_minimum_required(VERSION 3.23)

set(SHADERS
        02_descriptor_set.vert
        02_descriptor_set.frag
)

spirv_shaders(
        SHADER_HEADERS
        SPIRV_VERSION
        1.0 # Currently targeting Vulkan 1.0
        SOURCES
        ${SHADERS}
)

add_executable(SandBox
        ${SHADER_HEADERS}
        main.cpp
)

target_link_libraries(SandBox PRIVATE CECore)

