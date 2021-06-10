#include "VulkanUtils.h"
#include "VulkanRendererContext.h"

#include <algorithm>
#include <stdexcept>

namespace RHI
{
	uint32_t VulkanUtils::findMemoryType(
		const VulkanRendererContext& context,
		uint32_t typeFilter,
		VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(context.physicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			uint32_t memoryTypeProperties = memProperties.memoryTypes[i].propertyFlags;
			if ((typeFilter & (1 << i)) && (memoryTypeProperties & properties) == properties)
				return i;
		}

		throw std::runtime_error("Can't find suitable memory type");
	}

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

	void VulkanUtils::createImage2D(const VulkanRendererContext& context,
		uint32_t width,
		uint32_t height,
		uint32_t mipLevels,
		VkSampleCountFlagBits numSamples,
		VkFormat format,
		VkImageTiling tiling,
		VkImageUsageFlags usage,
		VkMemoryPropertyFlags memoryProperties,
		VkImage& image,
		VkDeviceMemory& memory)
	{
		VkImageCreateInfo imageInfo = {};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = mipLevels;
		imageInfo.arrayLayers = 1;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.usage = usage;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = numSamples;
		imageInfo.flags = 0; // Optional

		if (vkCreateImage(context.device, &imageInfo, nullptr, &image) != VK_SUCCESS)
			throw std::runtime_error("Can't create image");

		// Allocate memory for the image
		VkMemoryRequirements memoryRequirements = {};
		vkGetImageMemoryRequirements(context.device, image, &memoryRequirements);

		VkMemoryAllocateInfo memoryAllocateInfo = {};
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.allocationSize = memoryRequirements.size;
		memoryAllocateInfo.memoryTypeIndex = findMemoryType(context, memoryRequirements.memoryTypeBits, memoryProperties);

		if (vkAllocateMemory(context.device, &memoryAllocateInfo, nullptr, &memory) != VK_SUCCESS)
			throw std::runtime_error("Can't allocate image memory");

		if (vkBindImageMemory(context.device, image, memory, 0) != VK_SUCCESS)
			throw std::runtime_error("Can't bind image memory");
	}

	
	void VulkanUtils::createBuffer(
		const VulkanRendererContext& context,
		VkDeviceSize size,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags memoryProperties,
		VkBuffer& buffer,
		VkDeviceMemory& memory)
	{
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(context.device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
			throw std::runtime_error("Can't create buffer");

		// Allocate memory for the buffer
		VkMemoryRequirements memoryRequirements = {};
		vkGetBufferMemoryRequirements(context.device, buffer, &memoryRequirements);

		VkMemoryAllocateInfo memoryAllocateInfo = {};
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.allocationSize = memoryRequirements.size;
		memoryAllocateInfo.memoryTypeIndex = findMemoryType(context, memoryRequirements.memoryTypeBits, memoryProperties);

		if (vkAllocateMemory(context.device, &memoryAllocateInfo, nullptr, &memory) != VK_SUCCESS)
			throw std::runtime_error("Can't allocate buffer memory");

		if (vkBindBufferMemory(context.device, buffer, memory, 0) != VK_SUCCESS)
			throw std::runtime_error("Can't bind buffer memory");
	}

	void VulkanUtils::copyBuffer(
		const VulkanRendererContext& context,
		VkBuffer src,
		VkBuffer dst,
		VkDeviceSize size)
	{
		VkCommandBuffer commandBuffer = beginSingleTimeCommands(context);

		VkBufferCopy copyRegion = {};
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, src, dst, 1, &copyRegion);

		endSingleTimeCommands(context, commandBuffer);
	}

	void VulkanUtils::copyBufferToImage(
		const VulkanRendererContext& context,
		VkBuffer src,
		VkImage dst,
		uint32_t width,
		uint32_t height)
	{
		VkCommandBuffer commandBuffer = beginSingleTimeCommands(context);

		VkBufferImageCopy region = {};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent.width = width;
		region.imageExtent.height = height;
		region.imageExtent.depth = 1;

		vkCmdCopyBufferToImage(commandBuffer, src, dst, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		endSingleTimeCommands(context, commandBuffer);
	}

	// Command buffer requier the image in right layout first
	void VulkanUtils::transitionImageLayout(
		const VulkanRendererContext& context,
		VkImage image,
		uint32_t mipLevels,
		VkFormat format,
		VkImageLayout oldLayout,
		VkImageLayout newLayout)
	{
		VkCommandBuffer commandBuffer = beginSingleTimeCommands(context);

		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;

		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		barrier.image = image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = mipLevels;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			if (hasStencilComponent(format))
				barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
		else
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

		VkPipelineStageFlags srcStage;
		VkPipelineStageFlags dstStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			dstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			dstStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else
			throw std::runtime_error("Unsupported layout transition");

		vkCmdPipelineBarrier(
			commandBuffer,
			srcStage, dstStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		endSingleTimeCommands(context, commandBuffer);
	}

	VkCommandBuffer VulkanUtils::beginSingleTimeCommands(const VulkanRendererContext& context)
	{
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = context.commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(context.device, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		return commandBuffer;
	}

	void VulkanUtils::endSingleTimeCommands(const VulkanRendererContext& context, VkCommandBuffer commandBuffer)
	{
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(context.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(context.graphicsQueue);

		vkFreeCommandBuffers(context.device, context.commandPool, 1, &commandBuffer);
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

	bool VulkanUtils::hasStencilComponent(VkFormat format)
	{
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}

}
