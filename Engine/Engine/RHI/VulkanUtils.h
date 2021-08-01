#pragma once

#include <vulkan/vulkan.h>
#include "../Vendor/vma/vk_mem_alloc.h"
#include <vector>

namespace RHI
{
	class Device;

	class Utils
	{
	public:
		static VkImageUsageFlags getImageUsageFlags(VkFormat format);

		static VkImageAspectFlags getImageAspectFlags(VkFormat format);

		static VkImageViewType getImageBaseViewType(VkImageType type, VkImageCreateFlags flags, uint32_t num_layers);

		static bool checkInstanceValidationLayers(const std::vector<const char*>& requiredLayers, bool verbose = false);

		static bool checkInstanceExtensions(const std::vector<const char*>& requiredExtensions, bool verbose = false);

		static bool checkPhysicalDeviceExtensions(
			VkPhysicalDevice physicalDevice,
			const std::vector<const char*>& requiredExtensions,
			bool verbose = false);

		static VkSampleCountFlagBits getMaxUsableSampleCount(VkPhysicalDevice physicalDevice);

		static VkFormat selectOptimalImageFormat(
			VkPhysicalDevice physicalDevice,
			const std::vector<VkFormat>& candidates,
			VkImageTiling tiling,
			VkFormatFeatureFlags features);

		static VkFormat selectOptimalDepthFormat(VkPhysicalDevice physicalDevice);

		static uint32_t getGraphicsQueueFamily(VkPhysicalDevice physicalDevice);

		static uint32_t getPresentQueueFamily( VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t graphicsQueueFamily);

		static void createBuffer(
			const Device* device,
			VkDeviceSize size,
			VkBufferUsageFlags usage,
			VkMemoryPropertyFlags memoryProperties,
			VkBuffer& buffer,
			VmaAllocation& memory);

		static void fillDeviceLocalBuffer(
			const Device* device,
			VkBuffer buffer,
			VkDeviceSize size,
			const void* data);

		static void fillHostVisibleBuffer(
			const Device* device,
			VmaAllocation memory,
			VkDeviceSize size,
			const void* data);

		static VkShaderModule createShaderModule(
			const Device* device,
			const uint32_t* bytecode,
			size_t bytecodeSize);

		static void createImage(
			const Device* device,
			VkImageType type,
			uint32_t width,
			uint32_t height,
			uint32_t depth,
			uint32_t mipLevels,
			uint32_t arrayLayers,
			VkSampleCountFlagBits numSamples,
			VkFormat format,
			VkImageTiling tiling,
			VkImageUsageFlags usage,
			VkMemoryPropertyFlags memoryProperties,
			VkImageCreateFlags flags,
			VkImage& image,
			VmaAllocation& memory);

		static void createImageCube(
			const Device* device,
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

		static void createImage2D(
			const Device* device,
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

		static void createImage2D(
			const Device* device,
			uint32_t width,
			uint32_t height,
			uint32_t mipLevels,
			uint32_t pixelSize,
			const void* data,
			uint32_t dataMipLevels,
			VkSampleCountFlagBits numSamples,
			VkFormat format,
			VkImageTiling tiling,
			VkImage& image,
			VmaAllocation& memory);

		static VkImageView createImageView(
			const Device* device,
			VkImage image,
			VkFormat format,
			VkImageAspectFlags aspectFlags,
			VkImageViewType viewType,
			uint32_t baseMipLevel = 0,
			uint32_t numMipLevels = 1,
			uint32_t baseLayer = 0,
			uint32_t numLayers = 1);

		static VkSampler createSampler(
			const Device* device,
			uint32_t minMipLevel,
			uint32_t maxMipLevel);

		static void fillImage(
			const Device* device,
			VkImage image,
			uint32_t width,
			uint32_t height,
			uint32_t depth,
			uint32_t mipLevels,
			uint32_t arrayLayers,
			uint32_t pixelSize,
			VkFormat format,
			const void* data,
			uint32_t dataMipLevels,
			uint32_t dataArrayLayers);

		static void generateImage2DMipmaps(
			const Device* device,
			VkImage image,
			VkFormat imageFormat,
			uint32_t width,
			uint32_t height,
			uint32_t mipLevels,
			VkFormat format,
			VkFilter filter);

		static void transitionImageLayout(
			const Device* device,
			VkImage image,
			VkFormat format,
			VkImageLayout oldLayout,
			VkImageLayout newLayout,
			uint32_t baseMipLevel = 0,
			uint32_t numMipLevels = 1,
			uint32_t baseLayer = 0,
			uint32_t numLayers = 1);

		static VkCommandBuffer beginSingleTimeCommands(const Device* context);

		static void endSingleTimeCommands(const Device* device, VkCommandBuffer commandBuffer);
	};
}