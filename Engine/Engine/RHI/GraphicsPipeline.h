#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "RendererContext.h"

namespace RHI
{
	//  The graphics pipeline is the sequence of operations that take the vertices and textures of your meshes 
	// all the way to the pixels in the render targets. 
	// Vertex/Index buffer -> Input Assambler -> VS -> Tessellation -> GS -> Rasterization -> FS -> Color Blending -> Framebuffer
	class GraphicsPipeline
	{
	public:
		GraphicsPipeline(const RendererContext& context, VkPipelineLayout pipelineLayout, VkRenderPass renderPass)
			: context(context), renderPass(renderPass), pipelineLayout(pipelineLayout) { }

		inline VkPipeline getPipeline() const { return pipeline; }

		// Shader 
		void addShaderStage(
			VkShaderModule shader,
			VkShaderStageFlagBits stage,
			const char* entry = "main");

		// Vertex input
		void addVertexInput(
			const VkVertexInputBindingDescription& binding,
			const std::vector<VkVertexInputAttributeDescription>& attributes);

		// Dynamic
		void addDynamicState(VkDynamicState state);

		// Viewport
		void addViewport(const VkViewport& viewport);

		// Scissor
		void addScissor(const VkRect2D& scissor);

		// Color blending
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

		// Rasterizer
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

		// Multisampling
		void setMultisampleState(
			VkSampleCountFlagBits msaaSamples,
			bool sampleShading = false,
			float minSampleShading = 1.0f);

		// Depth and stencil testing
		void setDepthStencilState(
			bool depthTest,
			bool depthWrite,
			VkCompareOp depthCompareOp);

		VkPipeline build();

	private:
		RendererContext context;
		VkRenderPass renderPass{ VK_NULL_HANDLE };
		VkPipelineLayout pipelineLayout{ VK_NULL_HANDLE };

		std::vector<VkPipelineShaderStageCreateInfo> shaderStages; // shader stage
		std::vector<VkVertexInputBindingDescription> vertexInputBindings; // Vertex Input Stage
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
