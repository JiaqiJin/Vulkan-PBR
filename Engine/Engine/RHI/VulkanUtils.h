#pragma once

#include <vulkan/vulkan.h>
#include <vector>

struct RendererContext;

namespace RHI
{
	class VulkanUtils
	{
	public:
		static VkFormat selectOptimalImageFormat(
			const RendererContext& context,
			const std::vector<VkFormat>& candidates,
			VkImageTiling tiling,
			VkFormatFeatureFlags features);

		static VkFormat selectOptimalDepthFormat(const RendererContext& context);

		static uint32_t findMemoryType(
			const RendererContext& context,
			uint32_t typeFilter,
			VkMemoryPropertyFlags properties);

		static VkSampler createSampler(
			const RendererContext& context,
			uint32_t mipLevels);

		static VkShaderModule createShaderModule(
			const RendererContext& context,
			const uint32_t* bytecode,
			size_t bytecodeSize);

		static VkImageView createImageView(
			const RendererContext& context,
			VkImage image,
			VkFormat format,
			VkImageAspectFlags aspectFlags,
			VkImageViewType viewType,
			uint32_t baseMipLevel = 0,
			uint32_t numMipLevels = 1,
			uint32_t baseLayer = 0,
			uint32_t numLayers = 1);

		static void createImageCube(
			const RendererContext& context,
			uint32_t width,
			uint32_t height,
			uint32_t mipLevels,
			VkSampleCountFlagBits numSamples,
			VkFormat format,
			VkImageTiling tiling,
			VkImageUsageFlags usage,
			VkMemoryPropertyFlags memoryProperties,
			VkImage& image,
			VkDeviceMemory& memory);

		static void createImage2D(const RendererContext& context,
			uint32_t width,
			uint32_t height,
			uint32_t mipLevels,
			VkSampleCountFlagBits numSamples,
			VkFormat format,
			VkImageTiling tiling,
			VkImageUsageFlags usage,
			VkMemoryPropertyFlags memoryProperties,
			VkImage& image,
			VkDeviceMemory& memory);

		static void createBuffer(const RendererContext& context,
			VkDeviceSize size,
			VkBufferUsageFlags usage,
			VkMemoryPropertyFlags memoryProperties,
			VkBuffer& buffer,
			VkDeviceMemory& memory);

		// Helper functions recording and excuting a command buffer
		static void transitionImageLayout(
			const RendererContext& context,
			VkImage image,
			VkFormat format,
			VkImageLayout oldLayout,
			VkImageLayout newLayout,
			uint32_t baseMipLevel = 0,
			uint32_t numMipLevels = 1,
			uint32_t baseLayer = 0,
			uint32_t numLayers = 1);

		static void copyBuffer(
			const RendererContext& context,
			VkBuffer src,
			VkBuffer dst,
			VkDeviceSize size);

		// Which part of the buffer is going to be copied and which part of the image
		static void copyBufferToImage(
			const RendererContext& context,
			VkBuffer src,
			VkImage dst,
			uint32_t width,
			uint32_t height);

		static void generateImage2DMipmaps(
			const RendererContext& context,
			VkImage image,
			uint32_t width,
			uint32_t height,
			uint32_t mipLevels,
			VkFormat format,
			VkFilter filter);

		static void bindCombinedImageSampler(
			const RendererContext& context,
			VkDescriptorSet descriptorSet,
			int binding,
			VkImageView imageView,
			VkSampler sampler);

		static void bindUniformBuffer(
			const RendererContext& context,
			VkDescriptorSet descriptorSet,
			int binding,
			VkBuffer buffer,
			VkDeviceSize offset,
			VkDeviceSize size);

		static VkSampleCountFlagBits getMaxUsableSampleCount(const RendererContext& context);

		// Helper functions recording and excuting a command buffer
		static VkCommandBuffer beginSingleTimeCommands(const RendererContext& context);
		static void endSingleTimeCommands(const RendererContext& context, VkCommandBuffer commandBuffer);
	private:
		static bool hasStencilComponent(VkFormat format);
	};
}