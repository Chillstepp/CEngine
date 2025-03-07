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


namespace CE{
    void CEBaseMaterialSystem::OnInit(CEVulkanRenderPass *renderPass) {
        CE::CERenderContext* renderContext = CEApplication::GetAppContext()->RenderContext;
        CE::CEVulkanLogicDevice* device = renderContext->GetDevice();
        CE::CESwapchain* swapchain = renderContext->GetSwapchain();

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

    }

    void CEBaseMaterialSystem::OnRender(VkCommandBuffer cmdBuffer, CERenderTarget *renderTarget) {

    }

    void CEBaseMaterialSystem::OnDestroy() {

    }
}
