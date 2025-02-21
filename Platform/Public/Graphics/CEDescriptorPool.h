//
// Created by Chillstep on 25-2-21.
//

#ifndef CENGINE_PLATFORM_PUBLIC_GRAPHICS_CEDESCRIPTORPOOL_H_
#define CENGINE_PLATFORM_PUBLIC_GRAPHICS_CEDESCRIPTORPOOL_H_

#include "Graphics/CEVulkanCommon.h"

namespace CE{
	class CEVulkanLogicDevice;

	class CEDescriptorSetLayout{
	 public:
		CEDescriptorSetLayout(CEVulkanLogicDevice *device, const std::vector<VkDescriptorSetLayoutBinding> &bindings);
		~CEDescriptorSetLayout();

		VkDescriptorSetLayout GetHandle() const { return mHandle; }
	 private:
		VkDescriptorSetLayout mHandle = VK_NULL_HANDLE;

		CEVulkanLogicDevice *mDevice;
	};


	class CEDescriptorPool{
	 public:
		CEDescriptorPool(CEVulkanLogicDevice *device, uint32_t maxSets, const std::vector<VkDescriptorPoolSize> &poolSizes);
		~CEDescriptorPool();

		std::vector<VkDescriptorSet> AllocateDescriptorSet(CEDescriptorSetLayout *setLayout, uint32_t count);
	 private:
		VkDescriptorPool mHandle = VK_NULL_HANDLE;

		CEVulkanLogicDevice *mDevice;
	};




}

#endif //CENGINE_PLATFORM_PUBLIC_GRAPHICS_CEDESCRIPTORPOOL_H_
