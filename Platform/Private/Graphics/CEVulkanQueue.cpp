//
// Created by Chillstep on 2025/2/7.
//

#include "Graphics/CEVulkanQueue.h"

namespace CE{

	CEVulkanQueue::CEVulkanQueue(uint32_t FamilyIndex, uint32_t Index, VkQueue Queue, bool bCanPresent):
		FamilyIndex(FamilyIndex), Index(Index), Handle(Queue), CanPresent(bCanPresent)
	{
		LOG_T("Create a new Queue: {0} - {1} - {2}, present: {3}", FamilyIndex, Index, (void*) Queue, CanPresent);
	}

	void CEVulkanQueue::WaitIdle() const
	{
		CALL_VK(vkQueueWaitIdle(Handle));
	}

	void CEVulkanQueue::Submit(std::vector<VkCommandBuffer> cmdBuffers, const std::vector<VkSemaphore> &waitSemaphores, const std::vector<VkSemaphore> &signalSemaphores, VkFence fence)
	{
		VkPipelineStageFlags waitDstStageMask[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		VkSubmitInfo submitInfo = {
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.pNext = nullptr,
			.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size()),
			.pWaitSemaphores = waitSemaphores.data(),
			.pWaitDstStageMask = waitDstStageMask,
			.commandBufferCount = static_cast<uint32_t>(cmdBuffers.size()),
			.pCommandBuffers = cmdBuffers.data(),
			.signalSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size()),
			.pSignalSemaphores = signalSemaphores.data()
		};
		CALL_VK(vkQueueSubmit(Handle, 1, &submitInfo, fence));//1次提交std::vector<VkCommandBuffer> cmdBuffers
	}
}