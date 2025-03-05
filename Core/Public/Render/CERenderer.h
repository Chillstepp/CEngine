//
// Created by Chillstep on 25-3-5.
//

#ifndef CENGINE_CORE_PUBLIC_RENDER_CERENDERER_H_
#define CENGINE_CORE_PUBLIC_RENDER_CERENDERER_H_

#include "CERenderContext.h"

#define RENDERER_NUM_BUFFER 2

namespace CE{
	class CERenderer
	{
	 public:
		CERenderer();
		~CERenderer();

		//VK_SUBOPTIMAL_KHR is a return code in Vulkan, typically associated with swapchain operations (such as vkAcquireNextImageKHR and vkQueuePresentKHR).
		// It indicates that the swapchain operation was successful, but the state of the swapchain is suboptimal, usually due to changes in window size or display mode.
		//@Return: Is changed Extent of swapchain's Image(Window)
		bool Begin(int32_t *outPutIndex);
		//@Return: Is changed Extent of swapchain's Image(Window)
		bool End(int32_t imageIndex, const std::vector<VkCommandBuffer>& commandBuffers);
	 private:

		constexpr static uint32_t sNumBuffer = RENDERER_NUM_BUFFER;
		uint32_t mCurrentBuffer = 0;
		std::vector<VkSemaphore> mImageAvailableSemaphores;
		std::vector<VkSemaphore> mSubmitedSemaphores;
		std::vector<VkFence> mFrameFences;
	};
}

#endif //CENGINE_CORE_PUBLIC_RENDER_CERENDERER_H_
