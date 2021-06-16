#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "VulkanRendererContext.h"

namespace RHI
{
	class VulkanPipelineLayout
	{
	public:
		VulkanPipelineLayout(const VulkanRendererContext& context)
			: context(context) { }

		inline VkPipelineLayout getPipelineLayout() const { return pipelineLayout; }

		void addDescriptorSetLayout(VkDescriptorSetLayout descriptorSetLayout);

		VkPipelineLayout build();
	private:
		VulkanRendererContext context;

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts;

		VkPipelineLayout pipelineLayout{ VK_NULL_HANDLE };
	};
}