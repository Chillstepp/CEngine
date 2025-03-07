//
// Created by wanghaoyu25 on 25-3-7.
//

#ifndef CESYSTEM_H
#define CESYSTEM_H

#include "Graphics/CEVulkanCommon.h"

namespace CE{
    class CEVulkanRenderPass;
    class CERenderTarget;

    class CESystem {
    public:
        virtual void OnUpdate(float DeltaTime){}

        std::string SystemName;
    };

    class CEMaterialSystem : public CESystem {
    public:
        virtual void OnInit(CEVulkanRenderPass* renderPass) = 0;
        virtual void OnRender(VkCommandBuffer cmdBuffer, CERenderTarget* renderTarget) = 0;
        virtual void OnDestroy() = 0;
    };
}

#endif //CESYSTEM_H
