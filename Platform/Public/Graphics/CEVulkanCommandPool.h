//
// Created by Chillstep on 25-2-14.
//

#ifndef CENGINE_PLATFORM_PUBLIC_GRAPHICS_CEVULKANCOMMANDPOOL_H_
#define CENGINE_PLATFORM_PUBLIC_GRAPHICS_CEVULKANCOMMANDPOOL_H_

#include "Graphics/CEVulkanCommon.h"

namespace CE{
	class CEVulkanLogicDevice;

	class CEVulkanCommandPool{
	 public:
		CEVulkanCommandPool(CEVulkanLogicDevice *device, uint32_t queueFamilyIndex);
		~CEVulkanCommandPool();

		static void BeginCommandBuffer(VkCommandBuffer cmdBuffer);
		static void EndCommandBuffer(VkCommandBuffer cmdBuffer);

		std::vector<VkCommandBuffer> AllocateCommandBuffer(uint32_t count) const;
		VkCommandBuffer AllocateOneCommandBuffer() const;
		inline VkCommandPool GetHandle() const { return mHandle; }
	 private:
		VkCommandPool mHandle;

		CEVulkanLogicDevice *mDevice;
	};
}

#endif //CENGINE_PLATFORM_PUBLIC_GRAPHICS_CEVULKANCOMMANDPOOL_H_
