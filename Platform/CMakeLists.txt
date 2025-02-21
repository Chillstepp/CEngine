cmake_minimum_required(VERSION 3.23)
add_library(CEPlatform
        Private/CELog.cpp
        Private/CEGeometryUtil.cpp
        Private/Window/CEWindow.cpp
        Private/Window/CEGlfwWindow.cpp
        Private/Graphics/CEGraphicContext.cpp
        Private/Graphics/CEVulkanGraphicContext.cpp
        Private/Graphics/CEVulkanLogicDevice.cpp
        Private/Graphics/CEVulkanQueue.cpp
        Private/Graphics/CEVulkanSwapchain.cpp
        Private/Graphics/CEVulkanRenderPass.cpp
        Private/Graphics/CEVulkanFrameBuffer.cpp
        Private/Graphics/CEVulkanImageView.cpp
        Private/Graphics/CEVulkanPipeline.cpp
        Private/Graphics/CEVulkanCommandPool.cpp
        Private/Graphics/CEVulkanImage.cpp
        Private/Graphics/CEVulkanBuffer.cpp
        Private/Graphics/CEDescriptorPool.cpp
)


target_include_directories(CEPlatform PUBLIC External)

# glfw
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_INSTALL OFF)
add_subdirectory(External/glfw)
target_link_libraries(CEPlatform PRIVATE glfw)


add_subdirectory(External/glm)
target_link_libraries(CEPlatform PRIVATE glm)

find_package(Vulkan REQUIRED)
if (Vulkan_FOUND)
    message("Find vulkan success： ${Vulkan_INCLUDE_DIRS}")
    include_directories(${Vulkan_INCLUDE_DIRS})
else()
    message("Not find Vulkan.")
endif ()
target_include_directories(CEPlatform PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(CEPlatform PRIVATE ${Vulkan_LIBRARY})