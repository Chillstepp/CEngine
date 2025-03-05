
#include "CEFileUtil.h"
#include "CEEntryPoint.h"
#include "Render/CERenderTarget.h"
#include "Render/CEMesh.h"
#include "Graphics/CEVulkanRenderPass.h"
#include "Graphics/CEVulkanPipeline.h"
#include "Graphics/CEVulkanCommandPool.h"
#include "Graphics/CEVulkanQueue.h"
#include "Graphics/CEDescriptorPool.h"
#include "Graphics/CEVulkanImageView.h"
#include "Render/CETexture.h"
#include "Render/CERenderer.h"


struct GlobalUbo{
	glm::mat4 projMat {1.f};
	glm::mat4 viewMat {1.f};
};

struct InstanceUbo{
	glm::mat4 modelMat {1.f};
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

		 mRenderer = std::make_shared<CE::CERenderer>();

		 //descriptor set
		 std::vector<VkDescriptorSetLayoutBinding> desctLayoutBindings = {
			 {
				 .binding = 0,
				 .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				 .descriptorCount = 1,
				 .stageFlags = VK_SHADER_STAGE_VERTEX_BIT
			 },
			 {
				 .binding = 1,
				 .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				 .descriptorCount = 1,
				 .stageFlags = VK_SHADER_STAGE_VERTEX_BIT
			 },
			 {
				 .binding = 2,
				 .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				 .descriptorCount = 1,
				 .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
			 },
			 {
				 .binding = 3,
				 .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				 .descriptorCount = 1,
				 .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
			 }
		 };
		 mDescriptorSetLayout = std::make_shared<CE::CEDescriptorSetLayout>(device, desctLayoutBindings);

		 CE::ShaderLayout shaderLayout = {
			 .descriptorSetLayouts = { mDescriptorSetLayout->GetHandle() }
		 };
		 mPipelineLayout = std::make_shared<CE::CEVulkanPipelineLayout>(device, CE_RES_SHADER_DIR"02_descriptor_set.vert", CE_RES_SHADER_DIR"02_descriptor_set.frag", shaderLayout);
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

		 //create descriptor pool
		 std::vector<VkDescriptorPoolSize> poolSizes = {
			 {
				 .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				 .descriptorCount = 2
			 },
			 {
				 .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				 .descriptorCount = 2
			 }
		 };
		 mDescriptorPool = std::make_shared<CE::CEDescriptorPool>(device, 1, poolSizes);
		 mDescriptorSets = mDescriptorPool->AllocateDescriptorSet(mDescriptorSetLayout.get(), 1);

		 mGlobalBuffer = std::make_shared<CE::CEVulkanBuffer>(device, VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(GlobalUbo),nullptr, true);
		 mInstanceBuffer = std::make_shared<CE::CEVulkanBuffer>(device, VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(InstanceUbo),nullptr, true);
		 mTexture0 = std::make_shared<CE::CETexture>(CE_RES_TEXTURE_DIR"bird.jpg");
		 mTexture1 = std::make_shared<CE::CETexture>(CE_RES_TEXTURE_DIR"R-C.jpeg");


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
		 mInstanceUbo.modelMat = glm::rotate(glm::mat4(1.f), glm::radians(17.f), glm::vec3(1, 0, 0));
		 mInstanceUbo.modelMat = glm::rotate(mInstanceUbo.modelMat, glm::radians(time * 100.f), glm::vec3(0, 1, 0));
		 //mPushConstants.matrix = glm::ortho(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f) * mPushConstants.matrix;
		 mGlobalUbo.projMat = glm::perspective(glm::radians(65.f), swapchain->GetWidth() * 1.f / swapchain->GetHeight(), 0.01f, 100.f);
		 mGlobalUbo.projMat[1][1] *= -1.f;
		 mGlobalUbo.viewMat = glm::lookAt(glm::vec3{ 0, 0, 1.5f }, glm::vec3{ 0, 0, -1 }, glm::vec3{ 0, 1, 0 });

		 //mPushConstants.matrix = projMat * viewMat * mPushConstants.matrix; //MVP Matrix
	 }

	 void OnRender() override {
		 CE::CERenderContext *renderCxt = CEApplication::GetAppContext()->RenderContext;
		 CE::CEVulkanLogicDevice *device = renderCxt->GetDevice();
		 CE::CESwapchain *swapchain = renderCxt->GetSwapchain();

		 int32_t imageIndex;
		 if(mRenderer->Begin(&imageIndex)){
			 mRenderTarget->SetExtent({ swapchain->GetWidth(), swapchain->GetHeight() });
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

		 //vkCmdPushConstants(cmdBuffer, mPipelineLayout->GetHandle(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mPushConstants), &mPushConstants);
		 mGlobalBuffer->WriteData(&mGlobalUbo);
		 mInstanceBuffer->WriteData(&mInstanceUbo);
		 UpdateDescriptorSets();
		 vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout->GetHandle(), 0, 1, mDescriptorSets.data(), 0, nullptr);

		 mCubeMesh->Draw(cmdBuffer);

		 mRenderTarget->End(cmdBuffer);

		 CE::CEVulkanCommandPool::EndCommandBuffer(cmdBuffer);

		 if(mRenderer->End(imageIndex, { cmdBuffer })){
			 mRenderTarget->SetExtent({ swapchain->GetWidth(), swapchain->GetHeight() });
		 }
	 }

	 void OnDestroy() override {
		 CE::CERenderContext *renderCxt = CE::CEApplication::GetAppContext()->RenderContext;
		 CE::CEVulkanLogicDevice *device = renderCxt->GetDevice();
		 vkDeviceWaitIdle(device->GetHandle());
		 mGlobalBuffer.reset();
		 mInstanceBuffer.reset();
		 mTexture0.reset();
		 mTexture1.reset();

		 mCubeMesh.reset();
		 mCmdBuffers.clear();
		 mDescriptorPool.reset();
		 mDescriptorSetLayout.reset();
		 mPipeline.reset();
		 mPipelineLayout.reset();
		 mRenderTarget.reset();
		 mRenderPass.reset();
		 mRenderer.reset();

	 }

	 void UpdateDescriptorSets(){
		 CE::CERenderContext *renderCxt = CE::CEApplication::GetAppContext()->RenderContext;
		 CE::CEVulkanLogicDevice *device = renderCxt->GetDevice();

		 VkDescriptorBufferInfo globalBufferInfo = {
			 .buffer = mGlobalBuffer->GetHandle(),
			 .offset = 0,
			 .range = sizeof(mGlobalUbo)
		 };

		 VkDescriptorBufferInfo instanceBufferInfo = {
			 .buffer = mInstanceBuffer->GetHandle(),
			 .offset = 0,
			 .range = sizeof(mInstanceUbo)
		 };

		 VkDescriptorImageInfo textureImageInfo0 = {
			 .sampler = mTexture0->GetSampler(),
			 .imageView = mTexture0->GetImageView()->GetHandle(),
			 .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		 };

		 VkDescriptorImageInfo textureImageInfo1 = {
			 .sampler = mTexture1->GetSampler(),
			 .imageView = mTexture1->GetImageView()->GetHandle(),
			 .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		 };

		 VkDescriptorSet descriptorSet = mDescriptorSets[0];
		 std::vector<VkWriteDescriptorSet> writeDescriptorSets = {
			 {
				 .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				 .pNext = nullptr,
				 .dstSet = descriptorSet,
				 .dstBinding = 0,
				 .dstArrayElement = 0,
				 .descriptorCount = 1,
				 .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				 .pBufferInfo = &globalBufferInfo
			 },
			 {
				 .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				 .pNext = nullptr,
				 .dstSet = descriptorSet,
				 .dstBinding = 1,
				 .dstArrayElement = 0,
				 .descriptorCount = 1,
				 .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				 .pBufferInfo = &instanceBufferInfo
			 },
			 {
				 .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				 .pNext = nullptr,
				 .dstSet = descriptorSet,
				 .dstBinding = 2,
				 .dstArrayElement = 0,
				 .descriptorCount = 1,
				 .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				 .pImageInfo = &textureImageInfo0
			 },
			 {
				 .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				 .pNext = nullptr,
				 .dstSet = descriptorSet,
				 .dstBinding = 3,
				 .dstArrayElement = 0,
				 .descriptorCount = 1,
				 .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				 .pImageInfo = &textureImageInfo1
			 },
		 };
		 vkUpdateDescriptorSets(device->GetHandle(), writeDescriptorSets.size(), writeDescriptorSets.data(), 0, nullptr);
	 }


  private:

		 std::shared_ptr<CE::CEVulkanRenderPass> mRenderPass;
		 std::shared_ptr<CE::CERenderTarget> mRenderTarget;
		 std::shared_ptr<CE::CERenderer> mRenderer;
		 std::shared_ptr<CE::CEVulkanPipelineLayout> mPipelineLayout;
		 std::shared_ptr<CE::CEVulkanPipeline> mPipeline;

		 std::shared_ptr<CE::CEDescriptorSetLayout> mDescriptorSetLayout;
		 std::shared_ptr<CE::CEDescriptorPool> mDescriptorPool;
		 std::vector<VkDescriptorSet> mDescriptorSets;


		 GlobalUbo mGlobalUbo;
		 std::shared_ptr<CE::CEVulkanBuffer> mGlobalBuffer;
		 InstanceUbo mInstanceUbo;
	 	 std::shared_ptr<CE::CEVulkanBuffer> mInstanceBuffer;

	  	 std::shared_ptr<CE::CETexture> mTexture0;
	 	 std::shared_ptr<CE::CETexture> mTexture1;

		 std::vector<VkCommandBuffer> mCmdBuffers;

		 std::shared_ptr<CE::CEMesh> mCubeMesh;


 };


 CE::CEApplication* CreateApplicationEntryPoint(){
	return new SandBoxApp();
}