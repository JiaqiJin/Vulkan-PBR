#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

#include "DeviceObjectBase.h"
#include "PipelineBase.h"

namespace Vulkan
{
	class RenderPass;
	struct GraphicsPipelineState;

	class GraphicsPipeline : public PipelineBase
	{
	public:
		GraphicsPipeline(const std::shared_ptr<PipelineLayout>& pipeline_layout,
			const std::shared_ptr<RenderPass>& render_pass,
			const VkGraphicsPipelineCreateInfo& create_info);

		GraphicsPipeline(const std::shared_ptr<PipelineLayout>& pipeline_layout,
			const std::shared_ptr<RenderPass>& render_pass,
			const std::vector<VkPipelineShaderStageCreateInfo>& shader_stages,
			const GraphicsPipelineState& pipeline_state, uint32_t subpass);

		VkPipelineBindPoint GetBindPoint() const override { return VK_PIPELINE_BIND_POINT_GRAPHICS; }

		const std::shared_ptr<RenderPass>& GetRenderPassPtr() const { return m_render_pass; }
	private:
		std::shared_ptr<RenderPass> m_render_pass;
	};

	struct GraphicsPipelineState 
	{
		// Rasterization State
		struct RasterizationState 
		{
			VkPipelineRasterizationStateCreateInfo m_create_info{ VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
			void Initialize(VkPolygonMode polygon_mode, VkFrontFace front_face, VkCullModeFlags cull_mode = VK_CULL_MODE_BACK_BIT);
		} m_rasterization_state{};

		// Vertex Input State
		struct VertexInputState 
		{
			std::vector<VkVertexInputBindingDescription> m_bindings;
			std::vector<VkVertexInputAttributeDescription> m_attributes;
			VkPipelineVertexInputStateCreateInfo m_create_info{ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
			bool m_enable{ false };
			void Enable(const std::vector<VkVertexInputBindingDescription>& bindings, const std::vector<VkVertexInputAttributeDescription>& attributes);
			void Enable();
		} m_vertex_input_state{};

		// Input Assembly State
		struct InputAssemblyState 
		{
			VkPipelineInputAssemblyStateCreateInfo m_create_info{ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
			bool m_enable{ false };
			void Enable(VkPrimitiveTopology topology, VkBool32 primitive_restart_enable = VK_FALSE);
		} m_input_assembly_state{};

		// Tessellation State
		struct TessellationState 
		{
			VkPipelineTessellationStateCreateInfo m_create_info{ VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO };
			bool m_enable{ false };
			void Enable(uint32_t patch_control_points);
		} m_tessellation_state{};

		// Viewport
		struct ViewportState 
		{
			std::vector<VkViewport> m_viewports;
			std::vector<VkRect2D> m_scissors;
			VkPipelineViewportStateCreateInfo m_create_info{ VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
			bool m_enable{ false };
			void Enable(uint32_t viewport_count = 1, uint32_t scissor_count = 1);
			void Enable(const std::vector<VkViewport>& viewports, const std::vector<VkRect2D>& scissors);
		} m_viewport_state{};

		// Multisamples
		struct MultisampleState
 {
			VkPipelineMultisampleStateCreateInfo m_create_info{ VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
			bool m_enable{ false };
			void Enable(VkSampleCountFlagBits samples);
		} m_multisample_state{};

		// DepthStencil State
		struct DepthStencilState 
		{
			VkPipelineDepthStencilStateCreateInfo m_create_info{ VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
			bool m_enable{ false };
			void Enable(VkBool32 depth_test_enable = VK_TRUE, VkBool32 depth_write_enable = VK_TRUE,
				VkCompareOp depth_compare_op = VK_COMPARE_OP_LESS);
		} m_depth_stencil_state{};

		// Color Blend State
		struct ColorBlendState 
		{
			std::vector<VkPipelineColorBlendAttachmentState> m_color_blend_attachments;
			VkPipelineColorBlendStateCreateInfo m_create_info{ VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
			bool m_enable{ false };
			void Enable(uint32_t attachment_count, VkBool32 blend_enable);
			void Enable(const std::vector<VkPipelineColorBlendAttachmentState>& color_blend_attachments);
		} m_color_blend_state;

		// Dynamic State
		struct DynamicState
		{
			std::vector<VkDynamicState> m_dynamic_states;
			VkPipelineDynamicStateCreateInfo m_create_info{ VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
			bool m_enable{ false };
			void Enable(const std::vector<VkDynamicState>& dynamic_states);
		} m_dynamic_state;

		void SetGraphicsPipelineCreateInfo(VkGraphicsPipelineCreateInfo* info) const;
	};
}
