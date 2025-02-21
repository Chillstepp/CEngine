//
// Created by Chillstep on 2025/2/7.
//

#include "Graphics/CEVulkanLogicDevice.h"
#include "Graphics/CEVulkanGraphicContext.h"
#include "Graphics/CEVulkanQueue.h"
#include "Graphics/CEVulkanCommandPool.h"

namespace CE{

	const DeviceFeature RequestedExtensions[] = {
		{VK_KHR_SWAPCHAIN_EXTENSION_NAME, true},
#ifdef CE_ENGINE_PLATFORM_WIN32
#elif CE_ENGINE_PLATFORM_MACOS
		{ "VK_KHR_portability_subset", true },
#elif CE_ENGINE_PLATFORM_LINUX
#endif
	};

	CEVulkanLogicDevice::CEVulkanLogicDevice(CEVulkanGraphicContext* Context,
		uint32_t GraphicQueueCount,
		uint32_t PresentQueueCount,
		const CEVkSettings& Settings): Settings(Settings), mContext(Context)
	{
		if(!Context)
		{
			LOG_E("Must create a vulkan graphic context before create device.");
			return;
		}
		QueueFamilyInfo GraphicQueueFamilyInfo = Context->GetGraphicQueueFamilyInfo();
		QueueFamilyInfo PresentQueueFamilyInfo = Context->GetPresentQueueFamilyInfo();

		//Queue request check
		if(GraphicQueueCount > GraphicQueueFamilyInfo.QueueCount){
			LOG_E("This Graphic queue family has {0} queue, but requested {1}", GraphicQueueFamilyInfo.QueueCount, GraphicQueueCount);
			return;
		}
		if(PresentQueueCount > PresentQueueFamilyInfo.QueueCount){
			LOG_E("this Present queue family has {0} queue, but request {1}", PresentQueueFamilyInfo.QueueCount, PresentQueueCount);
			return;
		}
		std::vector<float> GraphicQueuePriorities(GraphicQueueCount, 0.f);
		std::vector<float> PresentQueuePriorities(PresentQueueCount, 1.f);
		uint32_t SameQueueCount = GraphicQueueCount;
		bool bIsSameQueueFamilyIndex = Context->IsSameGraphicPresentQueueFamily();
		if(bIsSameQueueFamilyIndex)
		{
			//if same queue family, which means we need more queue in this queue family.
			SameQueueCount += PresentQueueCount;
			if(SameQueueCount > GraphicQueueFamilyInfo.QueueCount) //if current queue is not enough, make it max
			{
				SameQueueCount = GraphicQueueFamilyInfo.QueueCount;
			}
			GraphicQueuePriorities.insert(GraphicQueuePriorities.end(), PresentQueuePriorities.begin(), PresentQueuePriorities.end());
		}

		VkDeviceQueueCreateInfo QueueCreateInfo[2] =
			{
				{
					.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
					.pNext = nullptr,
					.flags = 0,
					.queueFamilyIndex = static_cast<uint32_t>(GraphicQueueFamilyInfo.QueueFamilyIndex),
					.queueCount = SameQueueCount,
					.pQueuePriorities = GraphicQueuePriorities.data()
				}
			};

		if(!bIsSameQueueFamilyIndex)
		{
			QueueCreateInfo[1] = {
				.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.queueFamilyIndex = static_cast<uint32_t>(PresentQueueFamilyInfo.QueueFamilyIndex),
				.queueCount = PresentQueueCount,
				.pQueuePriorities = PresentQueuePriorities.data()
			};
		}

		uint32_t AvailableExtensionCount;
		CALL_VK(vkEnumerateDeviceExtensionProperties(Context->GetPhyDevice(), "", &AvailableExtensionCount, nullptr));
		std::vector<VkExtensionProperties> AvailableExtensions(AvailableExtensionCount);
		CALL_VK(vkEnumerateDeviceExtensionProperties(Context->GetPhyDevice(), "", &AvailableExtensionCount, AvailableExtensions.data()));
		uint32_t EnableExtensionCount;
		const char *EnableExtensions[32];
		if(!checkDeviceFeatures("Device Extension", true, AvailableExtensionCount, AvailableExtensions.data(),
			ARRAY_LENGTH(RequestedExtensions), RequestedExtensions, &EnableExtensionCount, EnableExtensions)){
			return;
		}


		VkDeviceCreateInfo DeviceInfo = {
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.queueCreateInfoCount = static_cast<uint32_t>(bIsSameQueueFamilyIndex ? 1 : 2),
			.pQueueCreateInfos = QueueCreateInfo,
			.enabledLayerCount = 0,
			.ppEnabledLayerNames = 0,
			.enabledExtensionCount = EnableExtensionCount,
			.ppEnabledExtensionNames = EnableExtensionCount > 0 ? EnableExtensions : nullptr,
			.pEnabledFeatures = nullptr
		};

		CALL_VK(vkCreateDevice(Context->GetPhyDevice(), &DeviceInfo, nullptr, &LogicDevice));
		LOG_T("Vk logic Device: {0}", (void*)LogicDevice);

		for(int i = 0; i < GraphicQueueCount; i++)
		{
			VkQueue queue;
			vkGetDeviceQueue(LogicDevice, GraphicQueueFamilyInfo.QueueFamilyIndex, i, &queue);
			GraphicQueues.push_back(std::make_shared<CEVulkanQueue>(GraphicQueueFamilyInfo.QueueFamilyIndex, i, queue, false));
		}

		for(int i = 0; i < PresentQueueCount; i++)
		{
			VkQueue queue;
			vkGetDeviceQueue(LogicDevice, PresentQueueFamilyInfo.QueueFamilyIndex, i, &queue);
			PresentQueues.push_back(std::make_shared<CEVulkanQueue>(PresentQueueFamilyInfo.QueueFamilyIndex, i, queue, true));
		}

		// // create a pipeline cache
		CreatePipelineCache();

		//create default command pool
		CreateDefaultCmdPool();
	}

	CEVulkanLogicDevice::~CEVulkanLogicDevice()
	{
		vkDeviceWaitIdle(LogicDevice);
		//tips: 这里比较奇怪, 如果不手动destroy其实是比较符合raii写法，但是vulkan要求：all child objects created on device must have been destroyed prior to
		// destroying device。 因此这与析构函数逻辑是相反的，析构函数先析构device再去析构自身，所以这里绝对有问题。有几个可有解决的方法：
		// 1. 手动置空提前 = nullptr or .reset()  （目前选择）
		// 2. 裸指针手动管理
		// 3. 不存在于device上
		mDefaultCmdPool.reset();
		VK_D(PipelineCache, LogicDevice, PipelineCache);
		vkDestroyDevice(LogicDevice, nullptr);
	}

	void CEVulkanLogicDevice::CreatePipelineCache()
	{
		VkPipelineCacheCreateInfo pipelineCacheInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0
		};
		CALL_VK(vkCreatePipelineCache(LogicDevice, &pipelineCacheInfo, nullptr, &PipelineCache));
	}

	int32_t CEVulkanLogicDevice::GetMemoryIndex(VkMemoryPropertyFlags memProps, uint32_t memoryTypeBits) const
	{
		VkPhysicalDeviceMemoryProperties phyDeviceMemProps = mContext->GetPhyDeviceMemProperties();
		if(phyDeviceMemProps.memoryTypeCount == 0){
			LOG_E("Physical device memory type count is 0");
			return -1;
		}
		for(int i = 0; i < phyDeviceMemProps.memoryTypeCount; i++)
		{
			if(memoryTypeBits & (1 << i) && (phyDeviceMemProps.memoryTypes[i].propertyFlags & memProps) == memProps)
			{
				return i;
			}
		}
		LOG_E("Can not find memory type index: type bit: {0}", memoryTypeBits);
		return 0;
	}

	VkCommandBuffer CEVulkanLogicDevice::CreateAndBeginOneCmdBuffer()
	{
		VkCommandBuffer cmdBuffer = mDefaultCmdPool->AllocateOneCommandBuffer();
		mDefaultCmdPool->BeginCommandBuffer(cmdBuffer);
		return cmdBuffer;
	}

	void CEVulkanLogicDevice::SubmitOneCmdBuffer(VkCommandBuffer cmdBuffer)
	{
		mDefaultCmdPool->EndCommandBuffer(cmdBuffer);
		CEVulkanQueue *queue = GetFirstGraphicQueue();
		queue->Submit({ cmdBuffer });
		queue->WaitIdle();
	}

	void CEVulkanLogicDevice::CreateDefaultCmdPool()
	{
		mDefaultCmdPool = std::make_shared<CE::CEVulkanCommandPool>(this, mContext->GetGraphicQueueFamilyInfo().QueueFamilyIndex);
	}

	VkResult CEVulkanLogicDevice::CreateSimpleSampler(VkFilter filter,
		VkSamplerAddressMode addressMode,
		VkSampler* outSampler)
	{
		VkSamplerCreateInfo samplerInfo = {
			.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.magFilter = filter,
			.minFilter = filter,
			.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
			.addressModeU = addressMode,
			.addressModeV = addressMode,
			.addressModeW = addressMode,
			.mipLodBias = 0,
			.anisotropyEnable = VK_FALSE,
			.maxAnisotropy = 0,
			.compareEnable = VK_FALSE,
			.compareOp = VK_COMPARE_OP_NEVER,
			.minLod = 0,
			.maxLod = 1,
			.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
			.unnormalizedCoordinates = VK_FALSE
		};
		return vkCreateSampler(LogicDevice, &samplerInfo, nullptr, outSampler);
	}
}