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

	void create2D(Format format, int width, int height, int num_mips);
	void createTexture2D(Format format, int width, int height, int num_mips,
		Multisample samples = Multisample::COUNT_1, const void* data = nullptr, uint32_t num_data_mipmaps = 1);

	void createCube(Format format, int width, int height, int num_mips);

	bool import(const char* path);
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
	VkSampler sampler{ VK_NULL_HANDLE };
	VmaAllocation memory{ VK_NULL_HANDLE };
	VkImageType type{ VK_IMAGE_TYPE_2D };
	VkFormat textureFormat{ VK_FORMAT_UNDEFINED };

	VkImageTiling tiling{ VK_IMAGE_TILING_OPTIMAL };
	VkSampleCountFlagBits samplesBits{ VK_SAMPLE_COUNT_1_BIT };
	VkImageCreateFlags flags{ 0 };
};