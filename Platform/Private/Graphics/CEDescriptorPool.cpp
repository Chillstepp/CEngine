//
// Created by Chillstep on 25-2-21.
//
#include "Graphics/CEDescriptorPool.h"
#include "Graphics/CEVulkanLogicDevice.h"

namespace CE{

	CEDescriptorSetLayout::CEDescriptorSetLayout(CEVulkanLogicDevice* device,
		const std::vector<VkDescriptorSetLayoutBinding>& bindings): mDevice(device)
	{
		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.bindingCount = static_cast<uint32_t>(bindings.size()),
			.pBindings = bindings.data()
		};
		CALL_VK(vkCreateDescriptorSetLayout(mDevice->GetHandle(), &descriptorSetLayoutInfo, nullptr, &mHandle));
	}

	CEDescriptorSetLayout::~CEDescriptorSetLayout()
	{
		VK_D(DescriptorSetLayout, mDevice->GetHandle(), mHandle);
	}


	CEDescriptorPool::CEDescriptorPool(CEVulkanLogicDevice* device, uint32_t maxSets, const std::vector<VkDescriptorPoolSize>& poolSizes) : mDevice(device)
	{
		VkDescriptorPoolCreateInfo descriptorPoolInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.maxSets = maxSets,
			.poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
			.pPoolSizes = poolSizes.data()
		};
		CALL_VK(vkCreateDescriptorPool(mDevice->GetHandle(), &descriptorPoolInfo, nullptr, &mHandle));
	}

	CEDescriptorPool::~CEDescriptorPool()
	{
		VK_D(DescriptorPool, mDevice->GetHandle(), mHandle);
	}

	std::vector<VkDescriptorSet> CEDescriptorPool::AllocateDescriptorSet(CEDescriptorSetLayout* setLayout, uint32_t count)
	{
		std::vector<VkDescriptorSet> descriptorSets(count);

		std::vector<VkDescriptorSetLayout> setLayouts(count);
		for(int i = 0; i < count; i++)
		{
			setLayouts[i] = setLayout->GetHandle();
		}

		VkDescriptorSetAllocateInfo allocateInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.pNext = nullptr,
			.descriptorPool = mHandle,
			.descriptorSetCount = count,
			.pSetLayouts = setLayouts.data()
		};
		VkResult ret = vkAllocateDescriptorSets(mDevice->GetHandle(), &allocateInfo, descriptorSets.data());

		CALL_VK(ret);
		if(ret != VK_SUCCESS)
		{
			descriptorSets.clear();
		}
		return descriptorSets;
	}

}
