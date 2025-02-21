//
// Created by Chillstep on 2025/2/5.
//

#ifndef CENGINE_PLATFORM_PUBLIC_GRAPHICS_CEVULKANGRAPHICCONTEXT_H_
#define CENGINE_PLATFORM_PUBLIC_GRAPHICS_CEVULKANGRAPHICCONTEXT_H_


#include "CEGraphicContext.h"
#include "CEVulkanCommon.h"



namespace CE{

	struct QueueFamilyInfo{
		int32_t QueueFamilyIndex = -1;
		uint32_t QueueCount;
	};

	class CEVulkanGraphicContext :public CEGraphicContext{
	 public:
		CEVulkanGraphicContext(const CEVulkanGraphicContext&) = delete;
		CEVulkanGraphicContext &operator=(const CEVulkanGraphicContext&) = delete;

		explicit CEVulkanGraphicContext(CEWindow* Window);

		bool bShouldValidate = true;//use vulakn validation or not

		//some get funcs .
		inline const QueueFamilyInfo &GetGraphicQueueFamilyInfo() const { return GraphicQueueFamily; }
		inline const QueueFamilyInfo &GetPresentQueueFamilyInfo() const { return PresentQueueFamily; }
		inline VkPhysicalDeviceMemoryProperties GetPhyDeviceMemProperties() const { return PhyDeviceMemProperties; }
		inline bool IsSameGraphicPresentQueueFamily() const { return GraphicQueueFamily.QueueFamilyIndex == PresentQueueFamily.QueueFamilyIndex; }
		inline VkPhysicalDevice GetPhyDevice() const { return VulkanPhyDevice; }
		inline VkSurfaceKHR GetSurface() const { return VulkanSurface; }

		virtual ~CEVulkanGraphicContext() override;
	 private:
		void CreateInstance();
		void CreateSurface(CEWindow* Window);
		void SelectPhyDevice();
		static void PrintPhyDeviceInfo(const VkPhysicalDeviceProperties &Props);
		static uint32_t DeviceScoreEvaluate(const VkPhysicalDeviceProperties &Props);

		VkInstance VulkanInstance;
		VkSurfaceKHR VulkanSurface;

		VkPhysicalDevice VulkanPhyDevice;
		QueueFamilyInfo GraphicQueueFamily;
		QueueFamilyInfo PresentQueueFamily;
		VkPhysicalDeviceMemoryProperties PhyDeviceMemProperties;

	 protected:
		CEVulkanGraphicContext() = default;
	};
}

#endif //CENGINE_PLATFORM_PUBLIC_GRAPHICS_CEVULKANGRAPHICCONTEXT_H_