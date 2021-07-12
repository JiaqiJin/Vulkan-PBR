#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace RHI
{
	class VulkanContext;

	class DescriptorSet
	{
	public:
		DescriptorSet(const VulkanContext* context, uint32_t imageCount)
			: context(context), imageCount(imageCount) { }

		std::vector<VkDescriptorSet> build(VkDescriptorSetLayout layouts);

	private:
		const VulkanContext* context{nullptr};
		std::vector<VkDescriptorSet> descriptorSets;
		uint32_t imageCount = 0;
	};
}
