//
// Created by Chillstep on 25-2-13.
//


#include "Graphics/CEVulkanRenderPass.h"
#include "Graphics/CEVulkanLogicDevice.h"
#include "Graphics/CEVulkanFrameBuffer.h"

namespace CE{

	CEVulkanRenderPass::CEVulkanRenderPass(CEVulkanLogicDevice* LogicDevice_,
		const std::vector<Attachment>& Attachments_,
		const std::vector<RenderSubPass>& SubPasses_): LogicDevice(LogicDevice_), mAttachments(Attachments_), mSubPasses(SubPasses_)
	{
		//1. default subpass and attachment
		if(mSubPasses.empty() && mAttachments.empty())
		{
			mAttachments = {{
				.format = LogicDevice->GetSettings().SurfaceFormat,
				.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
				.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
				.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
				.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
				.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
			}};
			mSubPasses = {{ .ColorAttachments = { 0 }, .SampleCount = VK_SAMPLE_COUNT_1_BIT }};
		}

		//2. subpasses
		std::vector<VkSubpassDescription> SubpassDescriptions(mSubPasses.size());
		std::vector<VkAttachmentReference> InputAttachmentRefs[mSubPasses.size()];
		std::vector<VkAttachmentReference> ColorAttachmentRefs[mSubPasses.size()];
		std::vector<VkAttachmentReference> DepthStencilAttachmentRefs[mSubPasses.size()];
		VkAttachmentReference ResolveAttachmentRefs[mSubPasses.size()];
		for(int i = 0; i < mSubPasses.size(); i++)
		{
			RenderSubPass SubPass = mSubPasses[i];
			for(const auto& inputAttachment: SubPass.InputAttachments)
			{
				InputAttachmentRefs[i].push_back({inputAttachment, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL});
			}
			for(const auto& colorAttachment: SubPass.ColorAttachments)
			{
				ColorAttachmentRefs[i].push_back({colorAttachment, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL});
				mAttachments[colorAttachment].samples = SubPass.SampleCount;
				if(SubPass.SampleCount > VK_SAMPLE_COUNT_1_BIT)
				{
					mAttachments[colorAttachment].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				}
			}
			for(const auto& depthStencilAttachment: SubPass.DepthStencilAttachments)
			{
				DepthStencilAttachmentRefs[i].push_back({depthStencilAttachment, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL});
				mAttachments[depthStencilAttachment].samples = SubPass.SampleCount;
			}

			//如果要多重采样，最后再push一个attachment
			if(SubPass.SampleCount > VK_SAMPLE_COUNT_1_BIT){
				mAttachments.push_back({
					.format = LogicDevice->GetSettings().SurfaceFormat,
					.samples = VK_SAMPLE_COUNT_1_BIT,
					.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
					.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
					.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
					.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
					.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				});
				ResolveAttachmentRefs[i] = { static_cast<uint32_t>(mAttachments.size() - 1), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
			}

			SubpassDescriptions[i].flags = 0;
			SubpassDescriptions[i].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			SubpassDescriptions[i].inputAttachmentCount = InputAttachmentRefs[i].size();
			SubpassDescriptions[i].pInputAttachments = InputAttachmentRefs[i].data();
			SubpassDescriptions[i].colorAttachmentCount = ColorAttachmentRefs[i].size();
			SubpassDescriptions[i].pColorAttachments = ColorAttachmentRefs[i].data();
			SubpassDescriptions[i].pResolveAttachments = SubPass.SampleCount > VK_SAMPLE_COUNT_1_BIT ? &ResolveAttachmentRefs[i] : nullptr;
			SubpassDescriptions[i].pDepthStencilAttachment = DepthStencilAttachmentRefs[i].data();
			SubpassDescriptions[i].preserveAttachmentCount = 0;
			SubpassDescriptions[i].pPreserveAttachments = nullptr;
		}

		std::vector<VkSubpassDependency> dependencies(mSubPasses.size() - 1);
		if(mSubPasses.size() > 1){
			for(int i = 0; i < dependencies.size(); i++){
				dependencies[i].srcSubpass      = i;
				dependencies[i].dstSubpass      = i + 1;
				dependencies[i].srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependencies[i].dstStageMask    = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				dependencies[i].srcAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				dependencies[i].dstAccessMask   = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
				dependencies[i].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
			}
		}

		//3. createinfo
		std::vector<VkAttachmentDescription> vkAttachments;
		vkAttachments.reserve(mAttachments.size());
		for (const auto &attachment: mAttachments){
			vkAttachments.push_back({
				.flags = 0,
				.format = attachment.format,
				.samples = attachment.samples,
				.loadOp = attachment.loadOp,
				.storeOp = attachment.storeOp,
				.stencilLoadOp = attachment.stencilLoadOp,
				.stencilStoreOp = attachment.stencilStoreOp,
				.initialLayout = attachment.initialLayout,
				.finalLayout = attachment.finalLayout
			});
		}
		VkRenderPassCreateInfo renderPassInfo = {
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.attachmentCount = static_cast<uint32_t>(vkAttachments.size()),
			.pAttachments = vkAttachments.data(),
			.subpassCount = static_cast<uint32_t>(mSubPasses.size()),
			.pSubpasses = SubpassDescriptions.data(),
			.dependencyCount = static_cast<uint32_t>(dependencies.size()),
			.pDependencies = dependencies.data()
		};
		CALL_VK(vkCreateRenderPass(LogicDevice->GetHandle(), &renderPassInfo, nullptr, &Handle));
		LOG_T("RenderPass {0} : {1}, attachment count: {2}, subpass count: {3}", __FUNCTION__, (void*)Handle, mAttachments.size(), mSubPasses.size());
	}

	CEVulkanRenderPass::~CEVulkanRenderPass()
	{
		if(Handle != VK_NULL_HANDLE) vkDestroyRenderPass(LogicDevice->GetHandle(), Handle, nullptr);
	}

	void CEVulkanRenderPass::Begin(VkCommandBuffer cmdBuffer,
		CEVulkanFrameBuffer* frameBuffer,
		const std::vector<VkClearValue>& clearValues) const
	{
		VkRect2D renderArea = {
			.offset = { 0, 0 },
			.extent = { frameBuffer->GetWidth(), frameBuffer->GetHeight() }
		};
		VkRenderPassBeginInfo beginInfo = {
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
			.pNext = nullptr,
			.renderPass = Handle,
			.framebuffer = frameBuffer->GetHandle(),
			.renderArea = renderArea,
			.clearValueCount = static_cast<uint32_t>(clearValues.size()),
			.pClearValues = clearValues.data()
		};
		vkCmdBeginRenderPass(cmdBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}
	void CEVulkanRenderPass::End(VkCommandBuffer cmdBuffer) const
	{
		vkCmdEndRenderPass(cmdBuffer);
	}
}