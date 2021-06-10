#include "VulkanUtils.h"
#include "VulkanRendererContext.h"

#include <algorithm>
#include <stdexcept>

namespace RHI
{
	VkImageView VulkanUtils::createImage2DView(
		const VulkanRendererContext& context,
		VkImage image,
		uint32_t mipLevels,
		VkFormat format,
		VkImageAspectFlags aspectFlags)
	{
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = image;
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = format;
		createInfo.subresourceRange.aspectMask = aspectFlags;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = mipLevels;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		VkImageView imageView;
		if (vkCreateImageView(context.device, &createInfo, nullptr, &imageView) != VK_SUCCESS)
			throw std::runtime_error("Can't create image view!");

		return imageView;
	}

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
