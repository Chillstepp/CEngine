//
// Created by wanghaoyu25 on 25-3-7.
//

#ifndef CEBASEMATERIALSYSTEM_H
#define CEBASEMATERIALSYSTEM_H

#include "ECS/CESystem.h"
#include "ECS/Component/CEBaseMaterialComponent.h"
#include "ECS/Component/CEMeshComponent.h"
#include "ECS/Component/CETransformComponent.h"

namespace CE {
    class CEVulkanPipelineLayout;
    class CEVulkanPipeline;

    struct PushConstants {
        glm::mat4 matrix{1.f};
        uint32_t colorType;
    };

    class CEBaseMaterialSystem : public CEMaterialSystem {
    public:
        void OnInit(CEVulkanRenderPass *renderPass) override;
        void OnRender(VkCommandBuffer cmdBuffer, CERenderTarget *renderTarget) override;
        void OnDestroy() override;
    private:
        std::shared_ptr<CEVulkanPipelineLayout> mPipelineLayout;
        std::shared_ptr<CEVulkanPipeline> mPipeline;
    };
}

#endif //CEBASEMATERIALSYSTEM_H
