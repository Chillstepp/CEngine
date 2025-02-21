//
// Created by Chillstep on 25-2-11.
//

#ifndef CENGINE_PLATFORM_PUBLIC_GRAPHICS_CEVULKANSWAPCHAIN_H_
#define CENGINE_PLATFORM_PUBLIC_GRAPHICS_CEVULKANSWAPCHAIN_H_
#include "CEVulkanCommon.h"

namespace CE{
	class CEVulkanGraphicContext;
	class CEVulkanLogicDevice;

	struct SurfaceInfo{
		VkSurfaceCapabilitiesKHR Capabilities;
		VkSurfaceFormatKHR SurfaceFormat;
		VkPresentModeKHR PresentMode;
	};

	class CESwapchain{
	 public:
		CESwapchain(CEVulkanGraphicContext* Context, CEVulkanLogicDevice* Device);
		~CESwapchain();

		//usually was used when window change size
		bool Recreate();

		const std::vector<VkImage> &GetImages() const { return Images; }
		uint32_t GetWidth() const { return SurfaceInfo.Capabilities.currentExtent.width; }
		uint32_t GetHeight() const { return SurfaceInfo.Capabilities.currentExtent.height; }
		int32_t GetCurrentImageIndex() const { return mCurrentImageIndex; }
		const SurfaceInfo &GetSurfaceInfo() const { return SurfaceInfo; }

		VkResult AcquireImage(int32_t* OutImageIndex, VkSemaphore Semaphore, VkFence Fence = VK_NULL_HANDLE);
		VkResult Present(int32_t imageIndex, const std::vector<VkSemaphore>& semaphores) const;
	 private:
		void SetupSurfaceCapabilities();

		VkSwapchainKHR SwapchainHandle = VK_NULL_HANDLE;
		CEVulkanLogicDevice* Device = nullptr;
		CEVulkanGraphicContext* Context = nullptr;

		SurfaceInfo SurfaceInfo;

		std::vector<VkImage> Images;

		int32_t mCurrentImageIndex = -1;

	};
}

#endif //CENGINE_PLATFORM_PUBLIC_GRAPHICS_CEVULKANSWAPCHAIN_H_
