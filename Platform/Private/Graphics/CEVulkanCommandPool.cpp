//
// Created by Chillstep on 25-2-14.
//

#include "Graphics/CEVulkanCommandPool.h"
#include "Graphics/CEVulkanLogicDevice.h"

namespace CE{
	CEVulkanCommandPool::CEVulkanCommandPool(CEVulkanLogicDevice* device, uint32_t queueFamilyIndex): mDevice(device)
	{
		VkCommandPoolCreateInfo commandPoolInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.pNext = nullptr,
			.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
			.queueFamilyIndex = queueFamilyIndex
		};
		CALL_VK(vkCreateCommandPool(mDevice->GetHandle(), &commandPoolInfo, nullptr, &mHandle));
		LOG_T("Create command pool : {0}", (void*)mHandle);
	}
	CEVulkanCommandPool::~CEVulkanCommandPool()
	{
		VK_D(CommandPool, mDevice->GetHandle(), mHandle);
	}
	void CEVulkanCommandPool::BeginCommandBuffer(VkCommandBuffer cmdBuffer)
	{
		CALL_VK(vkResetCommandBuffer(cmdBuffer, 0));
		VkCommandBufferBeginInfo beginInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.pNext = nullptr,
			.flags = 0,
			.pInheritanceInfo = nullptr
		};
		CALL_VK(vkBeginCommandBuffer(cmdBuffer, &beginInfo));
	}
	void CEVulkanCommandPool::EndCommandBuffer(VkCommandBuffer cmdBuffer)
	{
		CALL_VK(vkEndCommandBuffer(cmdBuffer));
	}
	std::vector<VkCommandBuffer> CEVulkanCommandPool::AllocateCommandBuffer(uint32_t count) const
	{
		std::vector<VkCommandBuffer> cmdBuffers(count);
		VkCommandBufferAllocateInfo allocateInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.pNext = nullptr,
			.commandPool = mHandle,
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = count
		};
		CALL_VK(vkAllocateCommandBuffers(mDevice->GetHandle(), &allocateInfo, cmdBuffers.data()));
		return cmdBuffers;
	}

	VkCommandBuffer CEVulkanCommandPool::AllocateOneCommandBuffer() const
	{
		std::vector<VkCommandBuffer> cmdBuffers = AllocateCommandBuffer(1);
		return cmdBuffers[0];
	}
}