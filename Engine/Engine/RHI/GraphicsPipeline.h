#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "RendererContext.h"

namespace RHI
{
	class GraphicsPipeline
	{
	public:
		GraphicsPipeline(const RendererContext& context, VkPipelineLayout pipelineLayout, VkRenderPass renderPass)
			: context(context), renderPass(renderPass), pipelineLayout(pipelineLayout) { }

		inline VkPipeline getPipeline() const { return pipeline; }

		void addShaderStage(
			VkShaderModule shader,
			VkShaderStageFlagBits stage,
			const char* entry = "main");

		void addVertexInput(
			const VkVertexInputBindingDescription& binding,
			const std::vector<VkVertexInputAttributeDescription>& attributes);

		void addDynamicState(VkDynamicState state);

		void addViewport(
			const VkViewport& viewport);

		void addScissor(
			const VkRect2D& scissor);

		void addBlendColorAttachment(
			bool blend = false,
			VkBlendFactor srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
			VkBlendFactor dstColorBlendFactor = VK_BLEND_FACTOR_ONE,
			VkBlendOp colorBlendOp = VK_BLEND_OP_ADD,
			VkBlendFactor srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
			VkBlendFactor dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
			VkBlendOp alphaBlendOp = VK_BLEND_OP_ADD,
			VkColorComponentFlags colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
		);

		void setInputAssemblyState(
			VkPrimitiveTopology topology,
			bool primitiveRestart = false);

		void setRasterizerState(
			bool depthClamp,
			bool rasterizerDiscard,
			VkPolygonMode polygonMode,
			float lineWidth,
			VkCullModeFlags cullMode,
			VkFrontFace frontFace,
			bool depthBias = false,
			float depthBiasConstantFactor = 0.0f,
			float depthBiasClamp = 0.0f,
			float depthBiasSlopeFactor = 0.0f);

		void setMultisampleState(
			VkSampleCountFlagBits msaaSamples,
			bool sampleShading = false,
			float minSampleShading = 1.0f);

		void setDepthStencilState(
			bool depthTest,
			bool depthWrite,
			VkCompareOp depthCompareOp);

		VkPipeline build();

	private:
		RendererContext context;
		VkRenderPass renderPass{ VK_NULL_HANDLE };
		VkPipelineLayout pipelineLayout{ VK_NULL_HANDLE };

		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
		std::vector<VkVertexInputBindingDescription> vertexInputBindings;
		std::vector<VkVertexInputAttributeDescription> vertexInputAttributes;
		std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
		std::vector<VkDynamicState> dynamicStates;

		std::vector<VkViewport> viewports;
		std::vector<VkRect2D> scissors;

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{};
		VkPipelineRasterizationStateCreateInfo rasterizerState{};
		VkPipelineMultisampleStateCreateInfo multisamplingState{};
		VkPipelineDepthStencilStateCreateInfo depthStencilState{};

		VkPipeline pipeline{ VK_NULL_HANDLE };
	};
}
