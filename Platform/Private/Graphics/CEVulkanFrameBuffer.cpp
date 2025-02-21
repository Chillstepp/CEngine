//
// Created by Chillstep on 25-2-13.
//

#include "Graphics/CEVulkanFrameBuffer.h"
#include "Graphics/CEVulkanLogicDevice.h"
#include "Graphics/CEVulkanImageView.h"
#include "Graphics/CEVulkanRenderPass.h"
#include "Graphics/CEVulkanImage.h"

namespace CE{

	CEVulkanFrameBuffer::CEVulkanFrameBuffer(CEVulkanLogicDevice* Device_,
		CEVulkanRenderPass* RenderPass_,
		const std::vector<std::shared_ptr<CEVulkanImage>> &Images_,
		uint32_t Width_,
		uint32_t Height_): LogicDevice(Device_), RenderPass(RenderPass_), Width(Width_), Height(Height_), mImages(Images_)
	{
		ReCreate(Images_, Width, Height);
	}


	bool CEVulkanFrameBuffer::ReCreate(const std::vector<std::shared_ptr<CEVulkanImage>> &Images_, uint32_t Width_, uint32_t Height_)
	{
		VkResult Ret;
		//destroy old framebuffer
		if(Handle != VK_NULL_HANDLE) vkDestroyFramebuffer(LogicDevice->GetHandle(), Handle, nullptr);

		Width = Width_;
		Height = Height_;
		ImageViews.clear();

		std::vector<VkImageView> Attachments(Images_.size());
		for(int i = 0; i < Images_.size(); i++)
		{
			bool isDepthFormat = IsDepthOnlyFormat(Images_[i]->GetFormat()); // FIXME when format is stencil format
			ImageViews.push_back(std::make_shared<CEVulkanImageView>(LogicDevice, Images_[i]->GetHandle(), Images_[i]->GetFormat(), isDepthFormat ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT));
			Attachments[i] = ImageViews[i]->GetHandle();
		}

		VkFramebufferCreateInfo frameBufferInfo = {
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.renderPass = RenderPass->GetHandle(),
			.attachmentCount = static_cast<uint32_t>(ImageViews.size()),
			.pAttachments = Attachments.data(),
			.width = Width,
			.height = Height,
			.layers = 1
		};
		Ret = vkCreateFramebuffer(LogicDevice->GetHandle(), &frameBufferInfo, nullptr, &Handle);
		LOG_T("FrameBuffer {0}, new: {1}, width: {2}, height: {3}, view count: {4}", __FUNCTION__, (void*)Handle, Width, Height, ImageViews.size());
		return Ret == VK_SUCCESS;

	}

	CEVulkanFrameBuffer::~CEVulkanFrameBuffer()
	{
		if(Handle != nullptr) vkDestroyFramebuffer(LogicDevice->GetHandle(), Handle, nullptr);
	}

}