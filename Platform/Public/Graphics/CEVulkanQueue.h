//
// Created by Chillstep on 2025/2/7.
//

#ifndef CENGINE_PLATFORM_PUBLIC_GRAPHICS_CEVULKANQUEUE_H_
#define CENGINE_PLATFORM_PUBLIC_GRAPHICS_CEVULKANQUEUE_H_

#include "CEVulkanCommon.h"

namespace CE{
	class CEVulkanQueue{
	 public:
		CEVulkanQueue(uint32_t FamilyIndex, uint32_t Index, VkQueue Queue, bool bCanPresent);
		~CEVulkanQueue() = default;

		inline VkQueue GetHandle() const {return Handle;}
		void WaitIdle() const;
		//Submit CommandBuffer to queue
		void Submit(std::vector<VkCommandBuffer> cmdBuffers, const std::vector<VkSemaphore> &waitSemaphores = {}, const std::vector<VkSemaphore> &signalSemaphores = {}, VkFence fence = VK_NULL_HANDLE);
	 private:

		uint32_t FamilyIndex;
		uint32_t Index;
		VkQueue Handle;
		bool CanPresent;
	};
}

#endif //CENGINE_PLATFORM_PUBLIC_GRAPHICS_CEVULKANQUEUE_H_
