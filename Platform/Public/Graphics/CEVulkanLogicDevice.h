//
// Created by Chillstep on 2025/2/7.
//

#ifndef CENGINE_PLATFORM_PUBLIC_GRAPHICS_CEVULKANLOGICDEVICE_H_
#define CENGINE_PLATFORM_PUBLIC_GRAPHICS_CEVULKANLOGICDEVICE_H_

#include "CEVulkanCommon.h"

namespace CE{
	class CEVulkanGraphicContext;
	class CEVulkanQueue;
	class CEVulkanCommandPool;

	struct CEVkSettings{
		VkFormat SurfaceFormat = VK_FORMAT_B8G8R8A8_UNORM;
		VkFormat DepthFormat = VK_FORMAT_D32_SFLOAT;
		VkPresentModeKHR PresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
		uint32_t SwapchainImageCount = 3;
	};

	class CEVulkanLogicDevice{
	 public:
		CEVulkanLogicDevice(CEVulkanGraphicContext* Context, uint32_t GraphicQueueCount, uint32_t PresentQueueCount, const CEVkSettings& Settings = {});
		~CEVulkanLogicDevice();

		inline VkDevice GetHandle() { return LogicDevice; }
		inline CEVkSettings GetSettings() { return Settings; }
		inline VkPipelineCache GetPipelineCache() const { return PipelineCache; }
		inline CEVulkanCommandPool* GetDefaultCommandPool() const {return mDefaultCmdPool.get();}

		CEVulkanQueue* GetGraphicQueue(uint32_t index) const { return GraphicQueues.size() < index + 1 ? nullptr : GraphicQueues[index].get(); };
		CEVulkanQueue* GetFirstGraphicQueue() const { return GraphicQueues.empty() ? nullptr : GraphicQueues[0].get(); };
		CEVulkanQueue* GetPresentQueue(uint32_t index) const { return PresentQueues.size() < index + 1 ? nullptr : PresentQueues[index].get(); };
		CEVulkanQueue* GetFirstPresentQueue() const { return PresentQueues.empty() ? nullptr : PresentQueues[0].get(); };

		int32_t GetMemoryIndex(VkMemoryPropertyFlags memProps, uint32_t memoryTypeBits) const;

		VkCommandBuffer CreateAndBeginOneCmdBuffer();
		void SubmitOneCmdBuffer(VkCommandBuffer cmdBuffer);

		VkResult CreateSimpleSampler(VkFilter filter, VkSamplerAddressMode addressMode, VkSampler *outSampler);


	 private:
		void CreatePipelineCache();
		void CreateDefaultCmdPool();

		VkDevice LogicDevice = VK_NULL_HANDLE;
		CEVulkanGraphicContext *mContext;

		std::vector<std::shared_ptr<CEVulkanQueue>> GraphicQueues;
		std::vector<std::shared_ptr<CEVulkanQueue>> PresentQueues;
		std::shared_ptr<CEVulkanCommandPool> mDefaultCmdPool;

		CEVkSettings Settings;

		VkPipelineCache PipelineCache = VK_NULL_HANDLE;
	};
}

#endif //CENGINE_PLATFORM_PUBLIC_GRAPHICS_CEVULKANLOGICDEVICE_H_
