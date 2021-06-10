#pragma once

#include <vulkan/vulkan.h>
#include <vector>

struct VulkanRendererContext;

namespace RHI
{
	class VulkanUtils
	{
	public:

		static VkSampleCountFlagBits getMaxUsableSampleCount(const VulkanRendererContext& context);
	private:

	};
}