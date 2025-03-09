//
// Created by Chillstep on 25-2-19.
//

#include "Render/CERenderTarget.h"
#include "CEApplication.h"
#include "Graphics/CEVulkanRenderPass.h"
#include "Graphics/CEVulkanImage.h"

namespace CE{
	CERenderTarget::CERenderTarget(CEVulkanRenderPass* renderPass)
	{
		CERenderContext* renderContext = CEApplication::GetAppContext()->RenderContext;
		CESwapchain* swapchain = renderContext->GetSwapchain();

		mRenderPass = renderPass;
		mBufferCount = swapchain->GetImages().size();
		mExtent = {swapchain->GetWidth(), swapchain->GetHeight()};
		bSwapchainTarget = true;

		Init();
		ReCreate();
	}

	CERenderTarget::CERenderTarget(CEVulkanRenderPass* renderPass, uint32_t bufferCount, VkExtent2D extent):
		mRenderPass(renderPass), mBufferCount(bufferCount), mExtent(extent), bSwapchainTarget(false)
	{
		Init();
		ReCreate();
	}

	CERenderTarget::~CERenderTarget()
	{
		for(const auto& materialSystemItem: mMaterialSystemList)
		{
			materialSystemItem->OnDestroy();
		}
		mMaterialSystemList.clear();
	}

	void CERenderTarget::Begin(VkCommandBuffer cmdBuffer)
	{
		assert(!bBeginTarget && "You should not called Begin() again.");

		if(bShouldUpdate){
			ReCreate();
			bShouldUpdate = false;
		}

		if(bSwapchainTarget){
			CERenderContext *renderCxt = CEApplication::GetAppContext()->RenderContext;
			CESwapchain *swapchain = renderCxt->GetSwapchain();
			mCurrentBufferIdx = swapchain->GetCurrentImageIndex();
		} else {
			mCurrentBufferIdx = (mCurrentBufferIdx + 1) % mBufferCount;
		}

		mRenderPass->Begin(cmdBuffer, GetFrameBuffer(), mClearValues);
		bBeginTarget = true;
	}

	void CERenderTarget::End(VkCommandBuffer cmdBuffer)
	{
		if(bBeginTarget){
			mRenderPass->End(cmdBuffer);
			bBeginTarget = false;
		}
	}

	void CERenderTarget::SetExtent(const VkExtent2D& extent)
	{
		mExtent = extent;
		bShouldUpdate = true;
	}

	void CERenderTarget::SetBufferCount(uint32_t bufferCount)
	{
		mBufferCount = bufferCount;
		bShouldUpdate = true;
	}

	void CERenderTarget::SetColorClearValue(VkClearColorValue colorClearValue)
	{
		std::vector<Attachment> renderPassAttachments = mRenderPass->GetAttachments();
		for(int i = 0; i < renderPassAttachments.size(); i++){
			if(!IsDepthStencilFormat(renderPassAttachments[i].format) && renderPassAttachments[i].loadOp == VK_ATTACHMENT_LOAD_OP_CLEAR){
				mClearValues[i].color = colorClearValue;
			}
		}
	}

	void CERenderTarget::SetColorClearValue(uint32_t attachmentIndex, VkClearColorValue colorClearValue)
	{
		std::vector<Attachment> renderPassAttachments = mRenderPass->GetAttachments();
		if(attachmentIndex <= renderPassAttachments.size() - 1){
			if(!IsDepthStencilFormat(renderPassAttachments[attachmentIndex].format) && renderPassAttachments[attachmentIndex].loadOp == VK_ATTACHMENT_LOAD_OP_CLEAR){
				mClearValues[attachmentIndex].color = colorClearValue;
			}
		}
	}

	void CERenderTarget::SetDepthStencilClearValue(VkClearDepthStencilValue depthStencilValue)
	{
		std::vector<Attachment> renderPassAttachments = mRenderPass->GetAttachments();
		for(int i = 0; i < renderPassAttachments.size(); i++){
			if(IsDepthStencilFormat(renderPassAttachments[i].format) && renderPassAttachments[i].loadOp == VK_ATTACHMENT_LOAD_OP_CLEAR){
				mClearValues[i].depthStencil = depthStencilValue;
			}
		}
	}

	void CERenderTarget::SetDepthStencilClearValue(uint32_t attachmentIndex, VkClearDepthStencilValue depthStencilValue)
	{
		std::vector<Attachment> renderPassAttachments = mRenderPass->GetAttachments();
		if(attachmentIndex <= renderPassAttachments.size() - 1){
			if(IsDepthStencilFormat(renderPassAttachments[attachmentIndex].format) && renderPassAttachments[attachmentIndex].loadOp == VK_ATTACHMENT_LOAD_OP_CLEAR){
				mClearValues[attachmentIndex].depthStencil = depthStencilValue;
			}
		}
	}

	void CERenderTarget::Init()
	{
		mClearValues.resize(mRenderPass->GetAttachments().size());
		SetColorClearValue({ 0.f, 0.f, 0.f, 1.f });
		SetDepthStencilClearValue({ 1.f, 0 });
	}

	void CERenderTarget::ReCreate()
	{
		if(mExtent.width == 0 || mExtent.height == 0){
			return;
		}
		mFrameBuffers.clear();
		mFrameBuffers.resize(mBufferCount);

		CERenderContext *renderCxt = CEApplication::GetAppContext()->RenderContext;
		CEVulkanLogicDevice *device = renderCxt->GetDevice();
		CESwapchain *swapchain = renderCxt->GetSwapchain();

		std::vector<Attachment> attachments = mRenderPass->GetAttachments();
		if(attachments.empty()){
			return;
		}

		std::vector<VkImage> swapchainImages = swapchain->GetImages();

		for(int i = 0; i < mBufferCount; i++){
			std::vector<std::shared_ptr<CEVulkanImage>> images;
			for(int j = 0; j < attachments.size(); j++){
				Attachment attachment = attachments[j];
				if(bSwapchainTarget && attachment.finalLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR && attachment.samples == VK_SAMPLE_COUNT_1_BIT)
				{
					images.push_back(std::make_shared<CEVulkanImage>(device, swapchainImages[i], VkExtent3D{ mExtent.width, mExtent.height, 1 }, attachment.format, attachment.usage));
				}
				else
				{
					images.push_back(std::make_shared<CEVulkanImage>(device, VkExtent3D{ mExtent.width, mExtent.height, 1 }, attachment.format, attachment.usage, attachment.samples));
				}
			}
			mFrameBuffers[i] = std::make_shared<CEVulkanFrameBuffer>(device, mRenderPass, images, mExtent.width, mExtent.height);
			images.clear();
		}
	}
}