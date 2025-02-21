
#include "CEFileUtil.h"
#include "CEEntryPoint.h"
#include "Render/CERenderTarget.h"
#include "Render/CEMesh.h"
#include "Graphics/CEVulkanRenderPass.h"
#include "Graphics/CEVulkanPipeline.h"
#include "Graphics/CEVulkanCommandPool.h"
#include "Graphics/CEVulkanQueue.h"


struct PushConstants
{
	glm::mat4 matrix{1.f};
};


class SandBoxApp : public CE::CEApplication
{
 protected:
	void OnConfiguration(CE::AppSettings *appSettings) override {
		appSettings->width = 1360;
		appSettings->height = 768;
		appSettings->title = "SandBox";
	}

	void OnInit() override
	{
		CE::CERenderContext* renderContext = CEApplication::GetAppContext()->RenderContext;
		CE::CEVulkanLogicDevice* device = renderContext->GetDevice();
		CE::CESwapchain* swapchain = renderContext->GetSwapchain();

		std::vector<CE::Attachment> attachments =
			{
				{
					.format = swapchain->GetSurfaceInfo().SurfaceFormat.format,
					.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
					.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
					.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
					.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
					.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
					.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				},
				{
					.format = device->GetSettings().DepthFormat,
					.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
					.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
					.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
					.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
					.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
					.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
				}
			};

		std::vector<CE::RenderSubPass> subpasses = {
			{
				.ColorAttachments = { 0 },
				.DepthStencilAttachments = { 1 },
				.SampleCount = VK_SAMPLE_COUNT_4_BIT
			}
		};
		mRenderPass = std::make_shared<CE::CEVulkanRenderPass>(device, attachments, subpasses);

		mRenderTarget = std::make_shared<CE::CERenderTarget>(mRenderPass.get());
		mRenderTarget->SetColorClearValue({0.1f, 0.2f, 0.3f, 1.f});
		mRenderTarget->SetDepthStencilClearValue({ 1, 0 });

		CE::ShaderLayout shaderLayout = {
			.pushConstants = {
				{
					.stageFlags = VK_SHADER_STAGE_VERTEX_BIT, //用于vertex shader
					.offset = 0,
					.size = sizeof(PushConstants)
				}
			}
		};
		mPipelineLayout = std::make_shared<CE::CEVulkanPipelineLayout>(device, CE_RES_SHADER_DIR"01_hello_buffer.vert", CE_RES_SHADER_DIR"01_hello_buffer.frag", shaderLayout);
		std::vector<VkVertexInputBindingDescription> vertexBindings = {
			{
				.binding = 0,
				.stride = sizeof(CE::CEVertex),
				.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
			}
		};

		/*
	   layout(location=0)      in vec3 a_Pos;
	   layout(location=1)      in vec2 a_Texcoord;
	   layout(location=2)      in vec3 a_Normal;
		*/
		std::vector<VkVertexInputAttributeDescription> vertexAttrs = {
			{
				.location = 0,
				.binding = 0,
				.format = VK_FORMAT_R32G32B32_SFLOAT,
				.offset = offsetof(CE::CEVertex, position)
			},
			{
				.location = 1,
				.binding = 0,
				.format = VK_FORMAT_R32G32_SFLOAT,
				.offset = offsetof(CE::CEVertex, texcoord0)
			},
			{
				.location = 2,
				.binding = 0,
				.format = VK_FORMAT_R32G32B32_SFLOAT,
				.offset = offsetof(CE::CEVertex, normal)
			},
		};
		mPipeline = std::make_shared<CE::CEVulkanPipeline>(device, mRenderPass.get(), mPipelineLayout.get());
		mPipeline->SetVertexInputState(vertexBindings, vertexAttrs);
		mPipeline->SetInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)->EnableDepthTest();
		mPipeline->SetDynamicState({ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR });
		mPipeline->SetMultisampleState(VK_SAMPLE_COUNT_4_BIT, VK_FALSE);
		mPipeline->Create();

		mImageAvailableSemaphores.resize(mNumBuffer);
		mSubmitedSemaphores.resize(mNumBuffer);
		mFrameFences.resize(mNumBuffer);

		VkSemaphoreCreateInfo semaphoreCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0
		};

		VkFenceCreateInfo fenceCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.pNext = nullptr,
			.flags = VK_FENCE_CREATE_SIGNALED_BIT //创建后立刻生效
		};

		for(int i = 0; i < mNumBuffer; i++)
		{
			CALL_VK(vkCreateSemaphore(device->GetHandle(), &semaphoreCreateInfo, nullptr, &mImageAvailableSemaphores[i]));
			CALL_VK(vkCreateSemaphore(device->GetHandle(), &semaphoreCreateInfo, nullptr, &mSubmitedSemaphores[i]));
			CALL_VK(vkCreateFence(device->GetHandle(), &fenceCreateInfo, nullptr, &mFrameFences[i]));
		}

		mCmdBuffers = device->GetDefaultCommandPool()->AllocateCommandBuffer(swapchain->GetImages().size());

		//Get geometry data, Create CEMesh
		std::vector<CE::CEVertex> vertices;
		std::vector<uint32_t> indices;
		CE::CEGeometryUtil::CreateCube(-0.3f, 0.3f, -0.3f, 0.3f, -0.3f, 0.3f, vertices, indices);
		mCubeMesh = std::make_shared<CE::CEMesh>(vertices, indices);

	}

	void OnUpdate(float deltaTime) override {
		CE::CERenderContext *renderCxt = CEApplication::GetAppContext()->RenderContext;
		CE::CESwapchain *swapchain = renderCxt->GetSwapchain();

		float time = std::chrono::duration<float>(std::chrono::steady_clock::now() - mStartTimePoint).count();
		mPushConstants.matrix = glm::rotate(glm::mat4(1.f), glm::radians(-17.f), glm::vec3(1, 0, 0));
		mPushConstants.matrix = glm::rotate(mPushConstants.matrix, glm::radians(time * 100.f), glm::vec3(0, 1, 0));
		//mPushConstants.matrix = glm::ortho(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f) * mPushConstants.matrix;
		glm::mat4 projMat = glm::perspective(glm::radians(65.f), swapchain->GetWidth() * 1.f / swapchain->GetHeight(), 0.01f, 100.f);
		glm::mat4 viewMat = glm::lookAt(glm::vec3{ 0, 0, 1.5f }, glm::vec3{ 0, 0, -1 }, glm::vec3{ 0, 1, 0 });

		mPushConstants.matrix = projMat * viewMat * mPushConstants.matrix; //MVP Matrix
	}

	void OnRender() override {
		CE::CERenderContext *renderCxt = CEApplication::GetAppContext()->RenderContext;
		CE::CEVulkanLogicDevice *device = renderCxt->GetDevice();
		CE::CESwapchain *swapchain = renderCxt->GetSwapchain();

		CALL_VK(vkWaitForFences(device->GetHandle(), 1, &mFrameFences[mCurrentBuffer], VK_TRUE, UINT64_MAX));
		CALL_VK(vkResetFences(device->GetHandle(), 1, &mFrameFences[mCurrentBuffer]));

		int32_t imageIndex;
		VkResult ret = swapchain->AcquireImage(&imageIndex, mImageAvailableSemaphores[mCurrentBuffer]);
		if(ret == VK_ERROR_OUT_OF_DATE_KHR){
			CALL_VK(vkDeviceWaitIdle(device->GetHandle())); // Wait render finish to recreate
			VkExtent2D originExtent = { swapchain->GetWidth(), swapchain->GetHeight() };
			bool bSuc = swapchain->Recreate();

			VkExtent2D newExtent = { swapchain->GetWidth(), swapchain->GetHeight() };
			if(bSuc && (originExtent.width != newExtent.width || originExtent.height != newExtent.height)){
				mRenderTarget->SetExtent(newExtent);
			}
			ret = swapchain->AcquireImage(&imageIndex, mImageAvailableSemaphores[mCurrentBuffer]);
			if(ret != VK_SUCCESS && ret != VK_SUBOPTIMAL_KHR){
				LOG_E("Recreate swapchain error: {0}", vk_result_string(ret));
			}
		}

		VkCommandBuffer cmdBuffer = mCmdBuffers[imageIndex];
		CE::CEVulkanCommandPool::BeginCommandBuffer(cmdBuffer);

		mRenderTarget->Begin(cmdBuffer);
		CE::CEVulkanFrameBuffer *frameBuffer = mRenderTarget->GetFrameBuffer();

		mPipeline->Bind(cmdBuffer);
		VkViewport viewport = {
			.x = 0,
			.y = 0,
			.width = static_cast<float>(frameBuffer->GetWidth()),
			.height = static_cast<float>(frameBuffer->GetHeight()),
			.minDepth = 0.f,
			.maxDepth = 1.f
		};
		vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);
		VkRect2D scissor = {
			.offset = { 0, 0 },
			.extent = { frameBuffer->GetWidth(), frameBuffer->GetHeight() }
		};
		vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);

		vkCmdPushConstants(cmdBuffer, mPipelineLayout->GetHandle(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mPushConstants), &mPushConstants);

		mCubeMesh->Draw(cmdBuffer);

		mRenderTarget->End(cmdBuffer);

		CE::CEVulkanCommandPool::EndCommandBuffer(cmdBuffer);
		device->GetFirstGraphicQueue()->Submit({ cmdBuffer }, { mImageAvailableSemaphores[mCurrentBuffer] }, { mSubmitedSemaphores[mCurrentBuffer] }, mFrameFences[mCurrentBuffer]);
		ret = swapchain->Present(imageIndex, { mSubmitedSemaphores[mCurrentBuffer] });

		if(ret == VK_SUBOPTIMAL_KHR){
			CALL_VK(vkDeviceWaitIdle(device->GetHandle()));
			VkExtent2D originExtent = { swapchain->GetWidth(), swapchain->GetHeight() };
			bool bSuc = swapchain->Recreate();

			VkExtent2D newExtent = { swapchain->GetWidth(), swapchain->GetHeight() };
			if(bSuc && (originExtent.width != newExtent.width || originExtent.height != newExtent.height)){
				mRenderTarget->SetExtent(newExtent);
			}
		}

		mCurrentBuffer = (mCurrentBuffer + 1) % mNumBuffer;
	}

	void OnDestroy() override {
		CE::CERenderContext *renderCxt = CE::CEApplication::GetAppContext()->RenderContext;
		CE::CEVulkanLogicDevice *device = renderCxt->GetDevice();
		vkDeviceWaitIdle(device->GetHandle());
		mCubeMesh.reset();
		mCmdBuffers.clear();
		mPipeline.reset();
		mPipelineLayout.reset();
		mRenderTarget.reset();
		mRenderPass.reset();
		for(int i = 0; i < mNumBuffer; i++){
			VK_D(Semaphore, device->GetHandle(), mImageAvailableSemaphores[i]);
			VK_D(Semaphore, device->GetHandle(), mSubmitedSemaphores[i]);
			VK_D(Fence, device->GetHandle(), mFrameFences[i]);
		}
	}


 private:
	std::shared_ptr<CE::CEVulkanRenderPass> mRenderPass;
	std::shared_ptr<CE::CERenderTarget> mRenderTarget;
	std::shared_ptr<CE::CEVulkanPipelineLayout> mPipelineLayout;
	std::shared_ptr<CE::CEVulkanPipeline> mPipeline;
	std::vector<VkCommandBuffer> mCmdBuffers;

	std::shared_ptr<CE::CEMesh> mCubeMesh;
	PushConstants mPushConstants;

	const uint32_t mNumBuffer = 2;
	uint32_t mCurrentBuffer = 0;
	std::vector<VkSemaphore> mImageAvailableSemaphores;
	std::vector<VkSemaphore> mSubmitedSemaphores;
	std::vector<VkFence> mFrameFences;
};


CE::CEApplication* CreateApplicationEntryPoint(){
	return new SandBoxApp();
}