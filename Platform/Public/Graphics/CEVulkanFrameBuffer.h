//
// Created by Chillstep on 25-2-13.
//

#ifndef CENGINE_PLATFORM_PUBLIC_GRAPHICS_CEVULKANFRAMEBUFFER_H_
#define CENGINE_PLATFORM_PUBLIC_GRAPHICS_CEVULKANFRAMEBUFFER_H_


#include "Graphics/CEVulkanCommon.h"

namespace CE{
	class CEVulkanLogicDevice;
	class CEVulkanRenderPass;
	class CEVulkanImageView;
	class CEVulkanImage;

	class CEVulkanFrameBuffer{
	 public:
		CEVulkanFrameBuffer(CEVulkanLogicDevice *Device_, CEVulkanRenderPass *RenderPass_, const std::vector<std::shared_ptr<CEVulkanImage>> &Images_, uint32_t Width_, uint32_t Height_);
		~CEVulkanFrameBuffer();

		bool ReCreate(const std::vector<std::shared_ptr<CEVulkanImage>> &Images_, uint32_t Width_, uint32_t Height_);

		inline VkFramebuffer GetHandle() const { return Handle; }
		inline uint32_t GetWidth() const { return Width; }
		inline uint32_t GetHeight() const { return Height; }

	 private:
		VkFramebuffer Handle = VK_NULL_HANDLE;
		CEVulkanLogicDevice* LogicDevice;
		CEVulkanRenderPass* RenderPass;
		uint32_t Width;
		uint32_t Height;
		std::vector<std::shared_ptr<CEVulkanImage>> mImages;
		std::vector<std::shared_ptr<CEVulkanImageView>> ImageViews;
	};
}

#endif //CENGINE_PLATFORM_PUBLIC_GRAPHICS_CEVULKANFRAMEBUFFER_H_
