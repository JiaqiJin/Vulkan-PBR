#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "RendererContext.h"

namespace RHI
{
	class PipelineLayout
	{
	public:
		PipelineLayout(const RendererContext& context)
			: context(context) { }

		inline VkPipelineLayout getPipelineLayout() const { return pipelineLayout; }

		void addDescriptorSetLayout(VkDescriptorSetLayout descriptorSetLayout);

		VkPipelineLayout build();
	private:
		RendererContext context;

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts;

		VkPipelineLayout pipelineLayout{ VK_NULL_HANDLE };
	};
}