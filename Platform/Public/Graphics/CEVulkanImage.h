//
// Created by Chillstep on 25-2-17.
//

#ifndef CENGINE_PLATFORM_PUBLIC_GRAPHICS_CEVULKANIMAGE_H_
#define CENGINE_PLATFORM_PUBLIC_GRAPHICS_CEVULKANIMAGE_H_

#include "CEVulkanCommon.h"

namespace CE{
	class CEVulkanLogicDevice;
	class CEVulkanBuffer;

	class CEVulkanImage{
	 public:
		//CreateImage, need allocate
		CEVulkanImage(CEVulkanLogicDevice *Device, VkExtent3D Extent, VkFormat Format, VkImageUsageFlags Usage, VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT);
		//Create Image by VkImage which has been allocated
		CEVulkanImage(CEVulkanLogicDevice *Device, VkImage Image, VkExtent3D Extent, VkFormat Format, VkImageUsageFlags Usage, VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT);
		~CEVulkanImage();

		inline VkFormat GetFormat() const { return mFormat; }
		inline VkImage GetHandle() const { return mHandle; }

		static bool TransitionLayout(VkCommandBuffer cmdBuffer, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);

		void CopyFromBuffer(VkCommandBuffer cmdBuffer, CEVulkanBuffer *buffer);

	 private:
		VkImage mHandle = VK_NULL_HANDLE;
		VkDeviceMemory mMemory = VK_NULL_HANDLE;

		bool bCreateImage = true;

		CEVulkanLogicDevice *mDevice;

		VkFormat mFormat;
		VkExtent3D mExtent;
		VkImageUsageFlags mUsage;
	};
}

#endif //CENGINE_PLATFORM_PUBLIC_GRAPHICS_CEVULKANIMAGE_H_
