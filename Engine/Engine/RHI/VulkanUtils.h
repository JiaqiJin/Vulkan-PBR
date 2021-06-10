#pragma once

#include <vulkan/vulkan.h>
#include <vector>

struct VulkanRendererContext;

namespace RHI
{
	class VulkanUtils
	{
	public:
		static VkImageView createImage2DView(
			const VulkanRendererContext& context,
			VkImage image,
			uint32_t mipLevels,
			VkFormat format,
			VkImageAspectFlags aspectFlags);

		static VkSampleCountFlagBits getMaxUsableSampleCount(const VulkanRendererContext& context);
	private:

	};
}