//
// Created by Chillstep on 25-2-14.
//

#ifndef CENGINE_PLATFORM_PUBLIC_GRAPHICS_CEVULKANPIPELINE_H_
#define CENGINE_PLATFORM_PUBLIC_GRAPHICS_CEVULKANPIPELINE_H_

#include "Graphics/CEVulkanCommon.h"

namespace CE{
	class CEVulkanLogicDevice;
	class CEVulkanRenderPass;

	struct ShaderLayout{
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts;// 资源数据
		std::vector<VkPushConstantRange> pushConstants; //常量数据
	};

	struct PipelineVertexInputState{
		std::vector<VkVertexInputBindingDescription> vertexBindings;
		std::vector<VkVertexInputAttributeDescription> vertexAttributes;
	};

	struct PipelineInputAssemblyState{
		VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		VkBool32 primitiveRestartEnable = VK_FALSE;
	};

	struct PipelineRasterizationState{
		VkBool32 depthClampEnable = VK_FALSE;
		VkBool32 rasterizerDiscardEnable = VK_FALSE;
		VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;
		VkCullModeFlags cullMode = VK_CULL_MODE_NONE;
		VkFrontFace frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		VkBool32 depthBiasEnable = VK_FALSE;
		float depthBiasConstantFactor = 0;
		float depthBiasClamp = 0;
		float depthBiasSlopeFactor = 0;
		float lineWidth = 1.f;
	};

	struct PipelineMultisampleState{
		VkSampleCountFlagBits rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		VkBool32 sampleShadingEnable = VK_FALSE;
		float minSampleShading = 0.2f;
	};

	struct PipelineDepthStencilState{
		VkBool32 depthTestEnable = VK_FALSE;
		VkBool32 depthWriteEnable = VK_FALSE;
		VkCompareOp depthCompareOp = VK_COMPARE_OP_NEVER;
		VkBool32 depthBoundsTestEnable = VK_FALSE;
		VkBool32 stencilTestEnable = VK_FALSE;
	};

	struct PipelineDynamicState{
		std::vector<VkDynamicState> dynamicStates;
	};

	struct PipelineConfig{
		PipelineVertexInputState vertexInputState;
		PipelineInputAssemblyState inputAssemblyState;
		PipelineRasterizationState rasterizationState;
		PipelineMultisampleState multisampleState;
		PipelineDepthStencilState depthStencilState;
		VkPipelineColorBlendAttachmentState colorBlendAttachmentState{
			.blendEnable = VK_FALSE,
			.srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
			.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
			.colorBlendOp = VK_BLEND_OP_ADD,
			.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
			.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
			.alphaBlendOp = VK_BLEND_OP_ADD,
			.colorWriteMask = VK_COLOR_COMPONENT_R_BIT
				| VK_COLOR_COMPONENT_G_BIT
				| VK_COLOR_COMPONENT_B_BIT
				| VK_COLOR_COMPONENT_A_BIT
		};
		PipelineDynamicState dynamicState;
	};

	class CEVulkanPipelineLayout{
	 public:
		CEVulkanPipelineLayout(CEVulkanLogicDevice *device, const std::string &vertexShaderFile, const std::string &fragShaderFile, const ShaderLayout &shaderLayout = {});
		~CEVulkanPipelineLayout();

		inline VkPipelineLayout GetHandle() const { return mHandle; }
		inline VkShaderModule GetVertexShaderModule() const { return mVertexShaderModule; }
		inline VkShaderModule GetFragShaderModule() const { return mFragShaderModule; }
	 private:
		VkResult CreateShaderModule(const std::string &filePath, VkShaderModule *outShaderModule);

		VkPipelineLayout mHandle = VK_NULL_HANDLE;

		VkShaderModule mVertexShaderModule = VK_NULL_HANDLE;
		VkShaderModule mFragShaderModule = VK_NULL_HANDLE;
		CEVulkanLogicDevice* mDevice;
	};



	class CEVulkanPipeline{
	 public:
		CEVulkanPipeline(CEVulkanLogicDevice *device, CEVulkanRenderPass *renderPass, CEVulkanPipelineLayout *pipelineLayout);
		~CEVulkanPipeline();
		void Create();

		void Bind(VkCommandBuffer cmdBuffer);

		CEVulkanPipeline *SetVertexInputState(const std::vector<VkVertexInputBindingDescription> &vertexBindings, const std::vector<VkVertexInputAttributeDescription> &vertexAttrs);
		CEVulkanPipeline *SetInputAssemblyState(VkPrimitiveTopology topology, VkBool32 primitiveRestartEnable = VK_FALSE);
		CEVulkanPipeline *SetRasterizationState(const PipelineRasterizationState &rasterizationState);
		CEVulkanPipeline *SetMultisampleState(VkSampleCountFlagBits samples, VkBool32 sampleShadingEnable, float minSampleShading = 0.f);
		CEVulkanPipeline *SetDepthStencilState(const PipelineDepthStencilState &depthStencilState);
		CEVulkanPipeline *SetColorBlendAttachmentState(VkBool32 blendEnable,
			VkBlendFactor srcColorBlendFactor = VK_BLEND_FACTOR_ONE, VkBlendFactor dstColorBlendFactor = VK_BLEND_FACTOR_ZERO, VkBlendOp colorBlendOp = VK_BLEND_OP_ADD,
			VkBlendFactor srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE, VkBlendFactor dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO, VkBlendOp alphaBlendOp = VK_BLEND_OP_ADD);
		CEVulkanPipeline *SetDynamicState(const std::vector<VkDynamicState> &dynamicStates);
		CEVulkanPipeline *EnableAlphaBlend();
		CEVulkanPipeline *EnableDepthTest();

		VkPipeline GetHandle() const { return mHandle; }
	 private:
		VkPipeline mHandle = VK_NULL_HANDLE;
		CEVulkanLogicDevice *mDevice;
		CEVulkanRenderPass *mRenderPass;
		CEVulkanPipelineLayout *mPipelineLayout;

		PipelineConfig mPipelineConfig;
	};


}

#endif //CENGINE_PLATFORM_PUBLIC_GRAPHICS_CEVULKANPIPELINE_H_
