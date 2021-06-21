#pragma once

#include <vulkan/vulkan.h>
#include <string>

#include "VulkanRendererContext.h"

namespace RHI
{
	class VulkanTexture
	{
	public:
		VulkanTexture(const VulkanRendererContext& context)
			: context(context) { }

		~VulkanTexture();

		inline VkImage getImage() const { return image; }
		inline VkImageView getImageView() const { return imageView; }
		inline VkSampler getSampler() const { return imageSampler; }
		inline VkFormat getImageFormat() const { return imageFormat; }

		inline int getNumLayers() const { return layers; }
		inline int getNumMipLevels() const { return mipLevels; }
		inline int getWidth() const { return width; }
		inline int getHeight() const { return height; }

		bool loadHDRFromFile(const std::string& path);
		bool loadFromFile(const std::string& path);

		void clearGPUData();
		void clearCPUData();;

	private:
		void uploadToGPU(VkFormat format, VkImageTiling tiling, size_t pixel_size);

	private:
		VulkanRendererContext context;

		unsigned char* pixels{ nullptr };
		int width{ 0 };
		int height{ 0 };
		int channels{ 0 };
		int mipLevels{ 0 };
		int layers { 0 };

		VkFormat imageFormat { VK_FORMAT_R8G8B8A8_UNORM };

		VkImage image{ VK_NULL_HANDLE };
		VkDeviceMemory imageMemory{ VK_NULL_HANDLE };
		VkImageView imageView{ VK_NULL_HANDLE };
		VkSampler imageSampler{ VK_NULL_HANDLE };
	};
}

