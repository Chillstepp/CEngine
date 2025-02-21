//
// Created by Chillstep on 25-2-13.
//

#ifndef CENGINE_PLATFORM_PUBLIC_GRAPHICS_CEVULKANIMAGEVIEW_H_
#define CENGINE_PLATFORM_PUBLIC_GRAPHICS_CEVULKANIMAGEVIEW_H_

#include "Graphics/CEVulkanCommon.h"

namespace CE{
	class CEVulkanLogicDevice;

	class CEVulkanImageView{
	 public:
		CEVulkanImageView(CEVulkanLogicDevice *Device_, VkImage Image_, VkFormat Format_, VkImageAspectFlags AspectFlags_);
		~CEVulkanImageView();

		VkImageView GetHandle() const { return Handle; }
	 private:
		VkImageView Handle = VK_NULL_HANDLE;

		CEVulkanLogicDevice *Device;
	};
}

#endif //CENGINE_PLATFORM_PUBLIC_GRAPHICS_CEVULKANIMAGEVIEW_H_
