cmake_minimum_required(VERSION 3.23)


add_library(CECore STATIC
        Private/CEApplication.cpp
        Private/Render/CERenderContext.cpp
        Private/Render/CEMesh.cpp
        Private/Render/CERenderTarget.cpp
        Private/Render/CETexture.cpp
        Private/Render/CERenderer.cpp
        Private/ECS/CEUUID.cpp
)

target_link_libraries(CECore PUBLIC CEPlatform)
