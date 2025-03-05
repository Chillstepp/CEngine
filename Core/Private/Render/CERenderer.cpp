//
// Created by Chillstep on 25-3-5.
//
#include "Render/CERenderer.h"
#include "CEApplication.h"
#include "Graphics/CEVulkanQueue.h"

namespace CE
{
	CERenderer::CERenderer()
	{
		CE::CERenderContext* renderContext = CEApplication::GetAppContext()->RenderContext;
		CE::CEVulkanLogicDevice* device = renderContext->GetDevice();

		mImageAvailableSemaphores.resize(sNumBuffer);
		mSubmitedSemaphores.resize(sNumBuffer);
		mFrameFences.resize(sNumBuffer);

		VkSemaphoreCreateInfo semaphoreCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0
		};

		VkFenceCreateInfo fenceCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.pNext = nullptr,
			.flags = VK_FENCE_CREATE_SIGNALED_BIT //创建后立刻生效
		};
		for(int i = 0; i < sNumBuffer; i++)
		{
			CALL_VK(vkCreateSemaphore(device->GetHandle(), &semaphoreCreateInfo, nullptr, &mImageAvailableSemaphores[i]));
			CALL_VK(vkCreateSemaphore(device->GetHandle(), &semaphoreCreateInfo, nullptr, &mSubmitedSemaphores[i]));
			CALL_VK(vkCreateFence(device->GetHandle(), &fenceCreateInfo, nullptr, &mFrameFences[i]));
		}


	}

	CERenderer::~CERenderer()
	{
		CE::CERenderContext* renderContext = CEApplication::GetAppContext()->RenderContext;
		CE::CEVulkanLogicDevice* device = renderContext->GetDevice();
		for(int i = 0; i < sNumBuffer; i++) {
			VK_D(Semaphore, device->GetHandle(), mImageAvailableSemaphores[i]);
		}
		for(int i = 0; i < sNumBuffer; i++) {
			VK_D(Semaphore, device->GetHandle(), mSubmitedSemaphores[i]);
		}
		for(int i = 0; i < sNumBuffer; i++) {
			VK_D(Fence, device->GetHandle(), mFrameFences[i]);
		}
	}

	bool CERenderer::Begin(int32_t* outPutIndex)
	{
		CE::CERenderContext *renderCxt = CEApplication::GetAppContext()->RenderContext;
		CE::CEVulkanLogicDevice *device = renderCxt->GetDevice();
		CE::CESwapchain *swapchain = renderCxt->GetSwapchain();

		CALL_VK(vkWaitForFences(device->GetHandle(), 1, &mFrameFences[mCurrentBuffer], VK_TRUE, UINT64_MAX));
		CALL_VK(vkResetFences(device->GetHandle(), 1, &mFrameFences[mCurrentBuffer]));

		bool bShouldUpdateTarget = false;

		VkResult ret = swapchain->AcquireImage(outPutIndex, mImageAvailableSemaphores[mCurrentBuffer]);
		if(ret == VK_ERROR_OUT_OF_DATE_KHR){
			CALL_VK(vkDeviceWaitIdle(device->GetHandle())); // Wait render finish to recreate
			VkExtent2D originExtent = { swapchain->GetWidth(), swapchain->GetHeight() };
			bool bSuc = swapchain->Recreate();

			VkExtent2D newExtent = { swapchain->GetWidth(), swapchain->GetHeight() };
			if(bSuc && (originExtent.width != newExtent.width || originExtent.height != newExtent.height)){
				bShouldUpdateTarget = true;
			}
			ret = swapchain->AcquireImage(outPutIndex, mImageAvailableSemaphores[mCurrentBuffer]);
			if(ret != VK_SUCCESS && ret != VK_SUBOPTIMAL_KHR){
				LOG_E("Recreate swapchain error: {0}", vk_result_string(ret));
			}
		}
		return bShouldUpdateTarget;
	}

	bool CERenderer::End(int32_t imageIndex, const std::vector<VkCommandBuffer>& commandBuffers)
	{
		CE::CERenderContext *renderCxt = CEApplication::GetAppContext()->RenderContext;
		CE::CEVulkanLogicDevice *device = renderCxt->GetDevice();
		CE::CESwapchain *swapchain = renderCxt->GetSwapchain();

		bool bShouldUpdateTarget = false;

		device->GetFirstGraphicQueue()->Submit({ commandBuffers }, { mImageAvailableSemaphores[mCurrentBuffer] }, { mSubmitedSemaphores[mCurrentBuffer] }, mFrameFences[mCurrentBuffer]);
		VkResult ret = swapchain->Present(imageIndex, { mSubmitedSemaphores[mCurrentBuffer] });
		if(ret == VK_SUBOPTIMAL_KHR){
			CALL_VK(vkDeviceWaitIdle(device->GetHandle()));
			VkExtent2D originExtent = { swapchain->GetWidth(), swapchain->GetHeight() };
			bool bSuc = swapchain->Recreate();

			VkExtent2D newExtent = { swapchain->GetWidth(), swapchain->GetHeight() };
			if(bSuc && (originExtent.width != newExtent.width || originExtent.height != newExtent.height)){
				bShouldUpdateTarget = true;
			}
		}

		mCurrentBuffer = (mCurrentBuffer + 1) % sNumBuffer;
		return bShouldUpdateTarget;
	}
}
