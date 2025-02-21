//
// Created by Chillstep on 25-2-21.
//

#ifndef CENGINE_CORE_PUBLIC_RENDER_CETEXTURE_H_
#define CENGINE_CORE_PUBLIC_RENDER_CETEXTURE_H_

#include "Graphics/CEVulkanCommon.h"

namespace CE{
	class CEVulkanImage;
	class CEVulkanImageView;
	//class CEVulkanBuffer;

	class CETexture{
	 public:
		CETexture(const std::string &filePath);
		~CETexture();

		uint32_t GetWidth() const { return mWidth; }
		uint32_t GetHeight() const { return mHeight; }
		CEVulkanImage *GetImage() const { return mImage.get(); }
		CEVulkanImageView *GetImageView() const { return mImageView.get(); }
		VkSampler GetSampler() const { return mSampler; }

	 private:
		uint32_t mWidth;
		uint32_t mHeight;
		VkFormat mFormat;
		std::shared_ptr<CEVulkanImage> mImage;
		std::shared_ptr<CEVulkanImageView> mImageView;
		VkSampler mSampler;
	};
}

#endif //CENGINE_CORE_PUBLIC_RENDER_CETEXTURE_H_
