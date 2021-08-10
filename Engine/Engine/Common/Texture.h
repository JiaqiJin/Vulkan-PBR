#pragma once

#include <vulkan/vulkan.h>
#include <algorithm>
#include <memory>
#include "../Vendor/vma/vk_mem_alloc.h"
#include "GraphicsEnums.h"

namespace RHI
{
	class Device;
}

class Texture
{
public:
	Texture(std::shared_ptr<RHI::Device> device) : device(device) { }
	~Texture();

	inline VkImage getImage() const { return image; }
	inline VkSampler getSampler() const { return imageSampler; }
	inline VkFormat getTextureFormat() const { return textureFormat; }

	inline int getNumLayers() const { return layers; }
	inline int getNumMipLevels() const { return mip_levels; }
	inline int getWidth() const { return width; }
	inline int getHeight() const { return height; }

	void create2D(Format format, int width, int height, int num_mips);
	void createTexture2D(Format format, int width, int height, int num_mips,
		Multisample samples = Multisample::COUNT_1, const void* data = nullptr, uint32_t num_data_mipmaps = 1);

	void createCube(Format format, int width, int height, int num_mips);
	void createTextureCube( uint32_t width, uint32_t height, uint32_t num_mipmaps, 
		Format format, const void* data = nullptr, uint32_t num_data_mipmaps = 1);

	bool importTexture(const char* path);
	void generateTexture2DMipmaps(int width, int height);

	void clearGPUData();
	void clearCPUData();

private:
	std::shared_ptr<RHI::Device> device;

	unsigned char* pixels = nullptr;
	int width = 0;
	int height = 0;
	int mip_levels = 0;
	int layers = 0;

	// Vulkan objects
	VkImage image{ VK_NULL_HANDLE };
	VkSampler imageSampler{ VK_NULL_HANDLE };
	VmaAllocation memory{ VK_NULL_HANDLE };
	VkImageType type{ VK_IMAGE_TYPE_2D };
	VkFormat textureFormat{ VK_FORMAT_UNDEFINED };

	VkImageTiling tiling{ VK_IMAGE_TILING_OPTIMAL };
	VkSampleCountFlagBits samplesBits{ VK_SAMPLE_COUNT_1_BIT };
	VkImageCreateFlags flags{ 0 };
};