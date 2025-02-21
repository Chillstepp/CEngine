//
// Created by Chillstep on 25-2-11.
//
#include "Graphics/CEVulkanSwapchain.h"
#include "Graphics/CEVulkanLogicDevice.h"
#include "Graphics/CEVulkanGraphicContext.h"
#include "Graphics/CEVulkanQueue.h"

namespace  CE{

	CESwapchain::CESwapchain(CEVulkanGraphicContext* Context, CEVulkanLogicDevice* Device) : Context(Context), Device(Device)
	{
		Recreate();
	}

	CESwapchain::~CESwapchain()
	{
		if(SwapchainHandle != VK_NULL_HANDLE) vkDestroySwapchainKHR(Device->GetHandle() ,SwapchainHandle, nullptr);

	}

	bool CESwapchain::Recreate()
	{
		LOG_D("-----------------------------");
		SetupSurfaceCapabilities();
		LOG_D("currentExtent : {0} x {1}", SurfaceInfo.Capabilities.currentExtent.width, SurfaceInfo.Capabilities.currentExtent.height);
		LOG_D("surfaceFormat : {0}", vk_format_string(SurfaceInfo.SurfaceFormat.format));
		LOG_D("presentMode   : {0}", vk_present_mode_string(SurfaceInfo.PresentMode));
		LOG_D("-----------------------------");

		uint32_t imageCount = Device->GetSettings().SwapchainImageCount;
		if(imageCount < SurfaceInfo.Capabilities.minImageCount && SurfaceInfo.Capabilities.minImageCount > 0){
			imageCount = SurfaceInfo.Capabilities.minImageCount;
		}
		if(imageCount > SurfaceInfo.Capabilities.maxImageCount && SurfaceInfo.Capabilities.maxImageCount > 0){
			imageCount = SurfaceInfo.Capabilities.maxImageCount;
		}


		//VK_SHARING_MODE_EXCLUSIVE指出对该对象的任一range或图像子资源的访问，在同一时间对一个queue family(队列族)是独占的。
		//VK_SHARING_MODE_CONCURRENT指出从多个queue families对该对象的任一range或图像子资源的并发访问将得到支持。
		VkSharingMode imageSharingMode;
		uint32_t QueueFamilyIndexCount;
		uint32_t QueueFamilyIndices[2] = { 0, 0 };
		if(Context->IsSameGraphicPresentQueueFamily()){
			imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			QueueFamilyIndexCount = 0;
		} else {
			imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			QueueFamilyIndexCount = 2;
			QueueFamilyIndices[0] = Context->GetGraphicQueueFamilyInfo().QueueFamilyIndex;
			QueueFamilyIndices[1] = Context->GetPresentQueueFamilyInfo().QueueFamilyIndex;
		}

		VkSwapchainKHR OldSwapchain = SwapchainHandle;

		VkSwapchainCreateInfoKHR SwapchainCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.pNext = nullptr,
			.flags = 0,
			.surface = Context->GetSurface(),
			.minImageCount = imageCount,
			.imageFormat = SurfaceInfo.SurfaceFormat.format,
			.imageColorSpace = SurfaceInfo.SurfaceFormat.colorSpace,
			.imageExtent = SurfaceInfo.Capabilities.currentExtent,
			.imageArrayLayers = 1,
			.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			.imageSharingMode = imageSharingMode,
			.queueFamilyIndexCount = QueueFamilyIndexCount,
			.pQueueFamilyIndices = QueueFamilyIndices,
			.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
			.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			.presentMode = SurfaceInfo.PresentMode,
			.clipped = VK_FALSE,
			.oldSwapchain = OldSwapchain
		};
		VkResult ret = vkCreateSwapchainKHR(Device->GetHandle(), &SwapchainCreateInfo, nullptr, &SwapchainHandle);
		//destroy old swapchain
		if(SwapchainHandle != VK_NULL_HANDLE) vkDestroySwapchainKHR(Device->GetHandle() ,OldSwapchain, nullptr);

		if(ret != VK_SUCCESS){
			LOG_E("{0} : {1}", __FUNCTION__, vk_result_string(ret));
			return false;
		}
		LOG_T("Swapchain {0} : old: {1}, new: {2}, image count: {3}, format: {4}, present mode : {5}", __FUNCTION__, (void*)OldSwapchain, (void*)SwapchainHandle, imageCount,
			vk_format_string(SurfaceInfo.SurfaceFormat.format), vk_present_mode_string(SurfaceInfo.PresentMode));

		uint32_t swapchainImageCount;
		ret = vkGetSwapchainImagesKHR(Device->GetHandle(), SwapchainHandle, &swapchainImageCount, nullptr);
		Images.resize(swapchainImageCount);
		ret = vkGetSwapchainImagesKHR(Device->GetHandle(), SwapchainHandle, &swapchainImageCount, Images.data());
		return ret == VK_SUCCESS;
	}
	void CESwapchain::SetupSurfaceCapabilities()
	{
		//capabilities
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Context->GetPhyDevice(), Context->GetSurface(), &SurfaceInfo.Capabilities);

		//format
		CEVkSettings settings = Device->GetSettings();
		uint32_t FormatCount;
		CALL_VK(vkGetPhysicalDeviceSurfaceFormatsKHR(Context->GetPhyDevice(), Context->GetSurface(), &FormatCount, nullptr));
		if(FormatCount == 0){
			LOG_E("{0} : num of surface format is 0", __FUNCTION__);
			return;
		}
		std::vector<VkSurfaceFormatKHR> Formats(FormatCount);
		CALL_VK(vkGetPhysicalDeviceSurfaceFormatsKHR(Context->GetPhyDevice(), Context->GetSurface(), &FormatCount, Formats.data()));
		int32_t FoundFormatIndex = -1;
		for(int i = 0; i < FormatCount; i++)
		{
			if(Formats[i].format == settings.SurfaceFormat && Formats[i].colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
			{
				FoundFormatIndex = i;
				break;
			}
		}
		if(FoundFormatIndex == -1) FoundFormatIndex = 0;
		SurfaceInfo.SurfaceFormat = Formats[FoundFormatIndex];

		//present mode
		uint32_t PresentModeCount;
		CALL_VK(vkGetPhysicalDeviceSurfacePresentModesKHR(Context->GetPhyDevice(), Context->GetSurface(), &PresentModeCount, nullptr));
		if(PresentModeCount == 0){
			LOG_E("{0} : num of surface present mode count is 0", __FUNCTION__);
			return;
		}
		VkPresentModeKHR PresentModes[PresentModeCount];
		CALL_VK(vkGetPhysicalDeviceSurfacePresentModesKHR(Context->GetPhyDevice(), Context->GetSurface(), &PresentModeCount, PresentModes));
		VkPresentModeKHR preferredPresentMode = Device->GetSettings().PresentMode;
		int32_t FoundPresentModeIndex = -1;
		for(int i = 0; i < PresentModeCount; i++){
			if(PresentModes[i] == preferredPresentMode){
				FoundPresentModeIndex = i;
				break;
			}
		}
		if(FoundPresentModeIndex >= 0){
			SurfaceInfo.PresentMode = PresentModes[FoundPresentModeIndex];
		}
		else{
			SurfaceInfo.PresentMode = PresentModes[0];
		}
	}

	VkResult CESwapchain::AcquireImage(int32_t* OutImageIndex, VkSemaphore Semaphore, VkFence Fence/* = VK_NULL_HANDLE*/)
	{
		uint32_t imageIndex;
		VkResult ret = vkAcquireNextImageKHR(Device->GetHandle(), SwapchainHandle, UINT64_MAX, Semaphore, Fence, &imageIndex);
		if(Fence != VK_NULL_HANDLE)
		{
			CALL_VK(vkWaitForFences(Device->GetHandle(), 1, &Fence, VK_FALSE, UINT64_MAX));
			CALL_VK(vkResetFences(Device->GetHandle(), 1, &Fence));
		}
		if(ret == VK_SUCCESS || ret == VK_SUBOPTIMAL_KHR){
			*OutImageIndex = imageIndex;
			mCurrentImageIndex = imageIndex;
		}
		return ret;
	}
	VkResult CESwapchain::Present(int32_t imageIndex, const std::vector<VkSemaphore>& semaphores) const
	{
		VkPresentInfoKHR presentInfo = {
			.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.pNext = nullptr,
			.waitSemaphoreCount = static_cast<uint32_t>(semaphores.size()),
			.pWaitSemaphores = semaphores.data(),
			.swapchainCount = 1,
			.pSwapchains = &SwapchainHandle,
			.pImageIndices = reinterpret_cast<const uint32_t *>(&imageIndex)
		};
		VkResult ret = vkQueuePresentKHR(Device->GetFirstPresentQueue()->GetHandle(), &presentInfo);
		Device->GetFirstPresentQueue()->WaitIdle();
		return  ret;
	}
}