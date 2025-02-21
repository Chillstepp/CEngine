//
// Created by Chillstep on 25-2-21.
//

#include "Render/CETexture.h"
#include "Render/CERenderContext.h"
#include "CEApplication.h"
#include "Graphics/CEVulkanImage.h"
#include "Graphics/CEVulkanImageView.h"
#include "Graphics/CEVulkanBuffer.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"


namespace CE{
	CETexture::CETexture(const std::string& filePath)
	{
		int numChannel;
		uint8_t *data = stbi_load(filePath.c_str(), reinterpret_cast<int *>(&mWidth), reinterpret_cast<int *>(&mHeight), &numChannel, STBI_rgb_alpha);
		if(!data){
			LOG_E("Can not load this image: {0}", filePath);
			return;
		}

		mFormat = VK_FORMAT_R8G8B8A8_UNORM;
		CERenderContext *renderCxt = CEApplication::GetAppContext()->RenderContext;
		CEVulkanLogicDevice *device = renderCxt->GetDevice();
		//VK_IMAGE_USAGE_TRANSFER_DST_BIT used for
		mImage = std::make_shared<CEVulkanImage>(device, VkExtent3D{ mWidth, mHeight, 1}, mFormat, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_SAMPLE_COUNT_1_BIT);
		mImageView = std::make_shared<CEVulkanImageView>(device, mImage->GetHandle(), mFormat, VK_IMAGE_ASPECT_COLOR_BIT);
		CALL_VK(device->CreateSimpleSampler(VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, &mSampler));

		// copy data to buffer
		size_t size = sizeof(uint8_t) * 4 * mWidth * mHeight;
		std::shared_ptr<CEVulkanBuffer> stageBuffer = std::make_shared<CEVulkanBuffer>(device, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, size, data, true);

		// UNDEFINED  -->  TRANSFER_DST --> copy --> SHADER_READ_ONLY_OPTIMAL


		// copy buffer to image
		VkCommandBuffer cmdBuffer = device->CreateAndBeginOneCmdBuffer();
		CEVulkanImage::TransitionLayout(cmdBuffer, mImage->GetHandle(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		mImage->CopyFromBuffer(cmdBuffer, stageBuffer.get());
		CEVulkanImage::TransitionLayout(cmdBuffer, mImage->GetHandle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		device->SubmitOneCmdBuffer(cmdBuffer);

		stageBuffer.reset();
		stbi_image_free(data);

	}

	CETexture::~CETexture()
	{
		CERenderContext* renderContext = CEApplication::GetAppContext()->RenderContext;
		CEVulkanLogicDevice* device = renderContext->GetDevice();
		VK_D(Sampler, device->GetHandle(), mSampler);
		mImageView.reset();
		mImage.reset();
	}
}