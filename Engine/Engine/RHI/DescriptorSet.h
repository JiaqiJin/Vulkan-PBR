#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "RendererContext.h"

namespace RHI
{
	class DescriptorSet
	{
	public:
		DescriptorSet(const RendererContext& context, uint32_t imageCount)
			: context(context), imageCount(imageCount) { }

		std::vector<VkDescriptorSet> build(VkDescriptorSetLayout layouts);

	private:
		RendererContext context;
		std::vector<VkDescriptorSet> descriptorSets;
		uint32_t imageCount = 0;
	};
}
