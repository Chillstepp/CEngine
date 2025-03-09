//
// Created by wanghaoyu25 on 25-3-7.
//
#include "ECS/System/CEBaseMaterialSystem.h"

#include <CEFileUtil.h>
#include <Graphics/CEDescriptorPool.h>
#include <Graphics/CEVulkanPipeline.h>
#include "Graphics/CEVulkanRenderPass.h"
#include "CEApplication.h"
#include "Render/CERenderContext.h"
#include "Render/CERenderTarget.h"


namespace CE{
    void CEBaseMaterialSystem::OnInit(CEVulkanRenderPass *renderPass) {
        CE::CERenderContext* renderContext = CEApplication::GetAppContext()->RenderContext;
        CE::CEVulkanLogicDevice* device = renderContext->GetDevice();
        CE::CESwapchain* swapchain = renderContext->GetSwapchain();

		 CE::ShaderLayout shaderLayout = {
			 .pushConstants = {
				 {
					 .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
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
		 mPipeline = std::make_shared<CE::CEVulkanPipeline>(device, renderPass, mPipelineLayout.get());
		 mPipeline->SetVertexInputState(vertexBindings, vertexAttrs);
		 mPipeline->SetInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)->EnableDepthTest();
		 mPipeline->SetDynamicState({ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR });
		 mPipeline->SetMultisampleState(VK_SAMPLE_COUNT_4_BIT, VK_FALSE);
		 mPipeline->Create();

    }

    void CEBaseMaterialSystem::OnRender(VkCommandBuffer cmdBuffer, CERenderTarget *renderTarget)
	{
		CE::CEApplicationContext *appCxt = CEApplication::GetAppContext();
		CE::CEApplication *device = appCxt->App;
		CE::CEScene* scene = appCxt->Scene;
		if(!scene) return;

		entt::registry& registry = scene->GetEcsRegistry();
		auto view = registry.view<CETransformComponent, CEMeshComponent, CEBaseMaterialComponent>();
		if(view.begin() == view.end()) return;

		mPipeline->Bind(cmdBuffer);

		CE::CEVulkanFrameBuffer *frameBuffer = renderTarget->GetFrameBuffer();
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



		glm::mat4 projMat = glm::perspective(glm::radians(65.f), frameBuffer->GetWidth() * 1.f / frameBuffer->GetHeight(), 0.01f, 100.f);
		projMat[1][1] *= -1.f;
		glm::mat4 viewMat = glm::lookAt(glm::vec3{ 0, 0, 1.5f }, glm::vec3{ 0, 0, -1 }, glm::vec3{ 0, 1, 0 });

		// setup custom params
		view.each([this, &cmdBuffer, &projMat, &viewMat](const auto &e, const CETransformComponent &transComp, const CEMeshComponent &meshComp, const CEBaseMaterialComponent &materialComp){
		  // mesh list draw
		  if(meshComp.mMesh){
			  PushConstants pushConstants {
				  .matrix = projMat * viewMat * transComp.GetTransform(),
				  .colorType = static_cast<uint32_t>(materialComp.colorType)
			  };
			  vkCmdPushConstants(cmdBuffer, mPipelineLayout->GetHandle(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(pushConstants), &pushConstants);
			  meshComp.mMesh->Draw(cmdBuffer);
		  }
		});
    }

    void CEBaseMaterialSystem::OnDestroy() {
		mPipeline.reset();
		mPipelineLayout.reset();
    }
}
