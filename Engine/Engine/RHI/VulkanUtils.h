#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include <vk_mem_alloc.h>

namespace RHI
{
	class VulkanContext;

	class VulkanUtils
	{
	public:
		static bool checkPhysicalDeviceExtensions(
			VkPhysicalDevice physicalDevice,
			const std::vector<const char*>& requiredExtensions,
			bool verbose = false);

		static VkFormat selectOptimalImageFormat(
			const VulkanContext* context,
			const std::vector<VkFormat>& candidates,
			VkImageTiling tiling,
			VkFormatFeatureFlags features);

		static VkFormat selectOptimalDepthFormat(const VulkanContext* context);

		static uint32_t findMemoryType(
			const VulkanContext* context,
			uint32_t typeFilter,
			VkMemoryPropertyFlags properties);

		static VkSampler createSampler(
			const VulkanContext* context,
			uint32_t mipLevels);

		static VkShaderModule createShaderModule(
			const VulkanContext* context,
			const uint32_t* bytecode,
			size_t bytecodeSize);

		// Create the basic image view for every image
		static VkImageView createImageView(
			const VulkanContext* context,
			VkImage image,
			VkFormat format,
			VkImageAspectFlags aspectFlags,
			VkImageViewType viewType,
			uint32_t baseMipLevel = 0,
			uint32_t numMipLevels = 1,
			uint32_t baseLayer = 0,
			uint32_t numLayers = 1);

		static void createImageCube(
			const VulkanContext* context,
			uint32_t width,
			uint32_t height,
			uint32_t mipLevels,
			VkSampleCountFlagBits numSamples,
			VkFormat format,
			VkImageTiling tiling,
			VkImageUsageFlags usage,
			VkMemoryPropertyFlags memoryProperties,
			VkImage& image,
			VmaAllocation& memory);

		static void createImage2D(const VulkanContext* context,
			uint32_t width,
			uint32_t height,
			uint32_t mipLevels,
			VkSampleCountFlagBits numSamples,
			VkFormat format,
			VkImageTiling tiling,
			VkImageUsageFlags usage,
			VkMemoryPropertyFlags memoryProperties,
			VkImage& image,
			VmaAllocation& memory);

		static void createBuffer(const VulkanContext* context,
			VkDeviceSize size,
			VkBufferUsageFlags usage,
			VkMemoryPropertyFlags memoryProperties,
			VkBuffer& buffer,
			VmaAllocation& memory);

		// Helper functions recording and excuting a command buffer
		static void transitionImageLayout(
			const VulkanContext* context,
			VkImage image,
			VkFormat format,
			VkImageLayout oldLayout,
			VkImageLayout newLayout,
			uint32_t baseMipLevel = 0,
			uint32_t numMipLevels = 1,
			uint32_t baseLayer = 0,
			uint32_t numLayers = 1);

		static void copyBuffer(
			const VulkanContext* context,
			VkBuffer src,
			VkBuffer dst,
			VkDeviceSize size);

		// Which part of the buffer is going to be copied and which part of the image
		static void copyBufferToImage(
			const VulkanContext* context,
			VkBuffer src,
			VkImage dst,
			uint32_t width,
			uint32_t height);

		static void generateImage2DMipmaps(
			const VulkanContext* context,
			VkImage image,
			uint32_t width,
			uint32_t height,
			uint32_t mipLevels,
			VkFormat format,
			VkFilter filter);

		static void bindCombinedImageSampler(
			const VulkanContext* context,
			VkDescriptorSet descriptorSet,
			int binding,
			VkImageView imageView,
			VkSampler sampler);

		static void bindUniformBuffer(
			const VulkanContext* context,
			VkDescriptorSet descriptorSet,
			int binding,
			VkBuffer buffer,
			VkDeviceSize offset,
			VkDeviceSize size);

		static VkSampleCountFlagBits getMaxUsableSampleCount(VkPhysicalDevice physicalDevice);

		// Helper functions recording and excuting a command buffer
		static VkCommandBuffer beginSingleTimeCommands(const VulkanContext* context);
		static void endSingleTimeCommands(const VulkanContext* context, VkCommandBuffer commandBuffer);
	private:
		static bool hasStencilComponent(VkFormat format);
	};
}