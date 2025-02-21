//
// Created by Chillstep on 25-2-13.
//

#include "Graphics/CEVulkanImageView.h"
#include "Graphics/CEVulkanLogicDevice.h"


namespace CE{

	CEVulkanImageView::CEVulkanImageView(CEVulkanLogicDevice* Device_,
		VkImage Image_,
		VkFormat Format_,
		VkImageAspectFlags AspectFlags_): Device(Device_)
	{
		VkImageViewCreateInfo ImageViewCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.image = Image_,
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = Format_,
			.components = {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY},
			.subresourceRange = {
				.aspectMask = AspectFlags_,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
		};

		CALL_VK(vkCreateImageView(Device->GetHandle(), &ImageViewCreateInfo, nullptr, &Handle));
	}

	CEVulkanImageView::~CEVulkanImageView()
	{
		if(Handle != VK_NULL_HANDLE)
		{
			vkDestroyImageView(Device->GetHandle(), Handle, nullptr);
		}
	}
}