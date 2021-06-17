#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "VulkanRendererContext.h"

namespace RHI
{
	class VulkanDescriptorSet
	{
	public:
		VulkanDescriptorSet(const VulkanRendererContext& context, uint32_t imageCount)
			: context(context), imageCount(imageCount) { }

		std::vector<VkDescriptorSet> build(VkDescriptorSetLayout layouts);

	private:
		VulkanRendererContext context;
		std::vector<VkDescriptorSet> descriptorSets;
		uint32_t imageCount = 0;
	};
}
