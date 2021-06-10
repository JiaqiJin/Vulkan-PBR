#include "VulkanUtils.h"
#include "VulkanRendererContext.h"

#include <algorithm>
#include <stdexcept>

namespace RHI
{
	VkSampleCountFlagBits VulkanUtils::getMaxUsableSampleCount(const VulkanRendererContext& context)
	{
		VkPhysicalDeviceProperties physicalDeviceProperties;
		vkGetPhysicalDeviceProperties(context.physicalDevice, &physicalDeviceProperties);

		VkSampleCountFlags counts = std::min(
			physicalDeviceProperties.limits.framebufferColorSampleCounts,
			physicalDeviceProperties.limits.framebufferDepthSampleCounts
		);

		if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
		if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
		if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
		if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
		if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
		if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

		return VK_SAMPLE_COUNT_1_BIT;
	}
}
