//
// Created by Chillstep on 25-2-13.
//

#ifndef CENGINE_PLATFORM_PUBLIC_GRAPHICS_CEVULKANRENDERPASS_H_
#define CENGINE_PLATFORM_PUBLIC_GRAPHICS_CEVULKANRENDERPASS_H_

#include "Graphics/CEVulkanCommon.h"

namespace CE{
	class CEVulkanLogicDevice;
	class CEVulkanFrameBuffer;

	struct Attachment{
		VkFormat                        format = VK_FORMAT_UNDEFINED;
		VkSampleCountFlagBits           samples = VK_SAMPLE_COUNT_1_BIT;
		VkAttachmentLoadOp              loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		VkAttachmentStoreOp             storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		VkAttachmentLoadOp              stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		VkAttachmentStoreOp             stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		VkImageLayout                   initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		VkImageLayout                   finalLayout = VK_IMAGE_LAYOUT_UNDEFINED; //todo: layout and imageuseage seems same?
		//Up same as vkattachmentdescription
		VkImageUsageFlags 				usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	};

	struct RenderSubPass{
		std::vector<uint32_t> InputAttachments; //index in attachments
		std::vector<uint32_t> ColorAttachments;
		std::vector<uint32_t> DepthStencilAttachments;
		VkSampleCountFlagBits SampleCount = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
	};

	class CEVulkanRenderPass{
	 public:
		CEVulkanRenderPass(CEVulkanLogicDevice* LogicDevice_, const std::vector<Attachment>& Attachments_ = {}, const std::vector<RenderSubPass>& SubPasses_ = {});
		~CEVulkanRenderPass();

		inline VkRenderPass GetHandle() const { return Handle; }
		void Begin(VkCommandBuffer cmdBuffer, CEVulkanFrameBuffer *frameBuffer, const std::vector<VkClearValue> &clearValues) const;
		void End(VkCommandBuffer cmdBuffer) const;

		const std::vector<Attachment>& GetAttachments() const {return mAttachments;}
		const std::vector<RenderSubPass>& GetSubPasses() const {return mSubPasses;}
	 private:
		VkRenderPass Handle = VK_NULL_HANDLE;
		CEVulkanLogicDevice* LogicDevice;

		std::vector<Attachment> mAttachments;
		std::vector<RenderSubPass> mSubPasses;
	};
}

#endif //CENGINE_PLATFORM_PUBLIC_GRAPHICS_CEVULKANRENDERPASS_H_
