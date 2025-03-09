//
// Created by Chillstep on 25-2-19.
//

#ifndef CENGINE_CORE_PUBLIC_RENDER_CERENDERTARGET_H_
#define CENGINE_CORE_PUBLIC_RENDER_CERENDERTARGET_H_

#include "Graphics/CEVulkanFrameBuffer.h"
#include "Render/CERenderContext.h"
#include "ECS/CESystem.h"

namespace CE{
	class CERenderTarget{
	 public:
		CERenderTarget(CEVulkanRenderPass *renderPass);
		CERenderTarget(CEVulkanRenderPass *renderPass, uint32_t bufferCount, VkExtent2D extent);
		~CERenderTarget();

		void Begin(VkCommandBuffer cmdBuffer);
		void End(VkCommandBuffer cmdBuffer);

		CEVulkanRenderPass *GetRenderPass() const { return mRenderPass; }
		CEVulkanFrameBuffer *GetFrameBuffer() const { return mFrameBuffers[mCurrentBufferIdx].get(); }

		void SetExtent(const VkExtent2D &extent);
		void SetBufferCount(uint32_t bufferCount);

		void SetColorClearValue(VkClearColorValue colorClearValue);
		void SetColorClearValue(uint32_t attachmentIndex, VkClearColorValue colorClearValue);
		void SetDepthStencilClearValue(VkClearDepthStencilValue depthStencilValue);
		void SetDepthStencilClearValue(uint32_t attachmentIndex, VkClearDepthStencilValue depthStencilValue);

		template<typename T, typename... Args>
		void AddMaterialSystem(Args&&... args) {
			std::shared_ptr<CEMaterialSystem> system = std::make_shared<T>(std::forward<Args>(args)...);
			system->OnInit(mRenderPass);
			mMaterialSystemList.push_back(system);
		}

		void RenderMaterialSystems(VkCommandBuffer cmdBuffer) {
			for (auto &item: mMaterialSystemList){
				item->OnRender(cmdBuffer, this);
			}
		}


	 private:
		void Init();
		void ReCreate();

		std::vector<std::shared_ptr<CEVulkanFrameBuffer>> mFrameBuffers;
		std::vector<std::shared_ptr<CEMaterialSystem>> mMaterialSystemList;

		CEVulkanRenderPass* mRenderPass;
		std::vector<VkClearValue> mClearValues;
		uint32_t mBufferCount;
		uint32_t mCurrentBufferIdx = 0;
		VkExtent2D mExtent;

		bool bSwapchainTarget = false; //用于swapchain present？
		bool bBeginTarget = false; //为了只begin一次，用作记录

		bool bShouldUpdate = false;//分辨率改变时进行重建
	};
}

#endif //CENGINE_CORE_PUBLIC_RENDER_CERENDERTARGET_H_
