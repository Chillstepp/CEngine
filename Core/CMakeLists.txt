cmake_minimum_required(VERSION 3.23)


add_library(CECore STATIC
        Private/CEApplication.cpp
        Private/Render/CERenderContext.cpp
        Private/Render/CEMesh.cpp
        Private/Render/CERenderTarget.cpp
        Private/Render/CETexture.cpp
        Private/Render/CERenderer.cpp

        Private/ECS/CEUUID.cpp
        Private/ECS/CESystem.cpp
        Private/ECS/CEComponent.cpp
        Private/ECS/CEEntity.cpp
        Private/ECS/CENode.cpp
        Private/ECS/CEScene.cpp
        Private/ECS/Component/CELookAtCameraComponent.cpp

        Private/ECS/System/CEBaseMaterialSystem.cpp
)

target_link_libraries(CECore PUBLIC CEPlatform)
