#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <cassert>
#include <iostream>

#include "../RHI/GlobalDevice.h"
#include "../RHI/Device.h"
#include "../RHI/VulkanUtils.h"


namespace Helper
{
	static void createTextureData(const std::shared_ptr<RHI::Device> device, VkFormat textureFormat, Format format,
		int width, int height, int depth, int num_mipmaps, int num_layers,
		const void* data, int num_data_mipmaps, int num_data_layers,
		VkImage image, VmaAllocation memory)
	{
		VkImageUsageFlags usage_flags = RHI::Utils::getImageUsageFlags(textureFormat);
		VkSampleCountFlagBits samples = RHI::Utils::getSamples(Multisample::COUNT_1);

		RHI::Utils::createImage(
			device,
			VK_IMAGE_TYPE_2D,
			width, height, depth,
			num_mipmaps, num_layers,
			samples, textureFormat, VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | usage_flags,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			0,
			image,
			memory
		);

		VkImageLayout source_layout = VK_IMAGE_LAYOUT_UNDEFINED;

		if (data != nullptr)
		{
			// prepare for transfer
			RHI::Utils::transitionImageLayout(
				device,
				image,
				textureFormat,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				0, num_mipmaps,
				0, num_layers
			);

			// transfer data to GPU
			RHI::Utils::fillImage(
				device,
				image,
				width, height, depth,
				num_mipmaps, num_layers,
				RHI::Utils::getPixelSize(format),
				textureFormat,
				data,
				num_data_mipmaps,
				num_data_layers
			);

			source_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		}

		// prepare for shader access
		RHI::Utils::transitionImageLayout(
			device,
			image,
			textureFormat,
			source_layout,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			0, num_mipmaps,
			0, num_layers
		);
	}
}

static Format deduceFormat(size_t pixelSize, int channels)
{
	assert(channels > 0 && channels <= 4);

	if (pixelSize == sizeof(stbi_uc))
	{
		switch (channels)
		{
		case 1: return Format::R8_UNORM;
		case 2: return Format::R8G8_UNORM;
		case 3: return Format::R8G8B8_UNORM;
		case 4: return Format::R8G8B8A8_UNORM;
		}
	}

	if (pixelSize == sizeof(float))
	{
		switch (channels)
		{
		case 1: return Format::R32_SFLOAT;
		case 2: return Format::R32G32_SFLOAT;
		case 3: return Format::R32G32B32_SFLOAT;
		case 4: return Format::R32G32B32A32_SFLOAT;
		}
	}

	return Format::UNDEFINED;
}

Texture::~Texture()
{
	clearGPUData();
	clearCPUData();
}

void Texture::create2D(Format format, int w, int h, int mips)
{
	width = w;
	height = h;
	mip_levels = mips;
	layers = 1;
	textureFormat = RHI::Utils::getFormat(format);

	createTexture2D(format, w, h, mips);
}

void Texture::createCube(Format format, int w, int h, int mips)
{
	width = w;
	height = h;
	mip_levels = mips;
	layers = 6;

	createTextureCube(width, height, mips, format);
}

bool Texture::importTexture(const char* path)
{
	if (stbi_info(path, nullptr, nullptr, nullptr) == 0)
	{
		std::cerr << "Texture::import(): unsupported image format for \"" << path << "\" file" << std::endl;
		return false;
	}

	void* stb_pixels = nullptr;
	size_t pixel_size = 0;
	int channels = 0;

	if (stbi_is_hdr(path))
	{
		stb_pixels = stbi_loadf(path, &width, &height, &channels, STBI_default);
		pixel_size = sizeof(float);
	}
	else
	{
		stb_pixels = stbi_load(path, &width, &height, &channels, STBI_default);
		pixel_size = sizeof(stbi_uc);
	}

	if (!stb_pixels)
	{
		std::cerr << "Texture::import(): " << stbi_failure_reason() << std::endl;
		return false;
	}

	layers = 1;
	mip_levels = static_cast<int>(std::floor(std::log2(std::max(width, height))) + 1);

	bool convert = false;
	if (channels == 3)
	{
		channels = 4;
		convert = true;
	}

	size_t image_size = width * height * channels * pixel_size;
	if (pixels != nullptr)
		delete[] pixels;

	pixels = new unsigned char[image_size];

	// As most hardware doesn't support rgb textures, convert it to rgba
	if (convert)
	{
		size_t numPixels = width * height;
		size_t stride = pixel_size * 3;

		unsigned char* d = pixels;
		unsigned char* s = reinterpret_cast<unsigned char*>(stb_pixels);

		for (size_t i = 0; i < numPixels; i++)
		{
			memcpy(d, s, stride);
			s += stride;
			d += stride;

			memset(d, 0, pixel_size);
			d += pixel_size;
		}
	}
	else
		memcpy(pixels, stb_pixels, image_size);

	stbi_image_free(stb_pixels);
	stb_pixels = nullptr;

	Format format = deduceFormat(pixel_size, channels);
	Multisample samples = Multisample::COUNT_1;

	textureFormat = RHI::Utils::getFormat(format);
	// Upload CPU data to GPU
	clearGPUData();

	createTexture2D(format, width, height, mip_levels, samples, pixels);
	generateTexture2DMipmaps(width, height);

	return true;
}

void Texture::createTexture2D(Format format,
	int width, int height, int num_mips,
	Multisample samples, 
	const void* data, 
	uint32_t num_data_mipmaps)
{
	VkImageUsageFlags usage_flags = RHI::Utils::getImageUsageFlags(textureFormat);
	samplesBits = RHI::Utils::getSamples(samples);

	RHI::Utils::createImage(
		device,
		VK_IMAGE_TYPE_2D,
		width, height, 1,
		num_mips, 1,
		samplesBits, textureFormat, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | usage_flags,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		0,
		image,
		memory);

	VkImageLayout source_layout = VK_IMAGE_LAYOUT_UNDEFINED;

	if (data != nullptr)
	{
		// prepare for transfer
		RHI::Utils::transitionImageLayout(
			device,
			image,
			textureFormat,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			0, num_mips,
			0, 1);

		// transfer data to GPU
		RHI::Utils::fillImage(
			device,
			image,
			width, height, 1,
			num_mips, 1,
			RHI::Utils::getPixelSize(format),
			textureFormat,
			data,
			num_data_mipmaps,
			1);

		source_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	}

	// prepare for shader access
	RHI::Utils::transitionImageLayout(
		device,
		image,
		textureFormat,
		source_layout,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		0, num_mips,
		0, 1);

	// create base sampler
	imageSampler = RHI::Utils::createSampler(device, 0, num_mips);
}

void Texture::generateTexture2DMipmaps(int w, int h)
{
	// prepare for transfer
	RHI::Utils::transitionImageLayout(
		device,
		image,
		textureFormat,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		0,
		mip_levels);

	// generate 2D mipmaps with linear filter
	RHI::Utils::generateImage2DMipmaps(
		device,
		image,
		textureFormat,
		w,
		h,
		mip_levels,
		textureFormat,
		VK_FILTER_LINEAR);

	// prepare for shader access
	RHI::Utils::transitionImageLayout(
		device,
		image,
		textureFormat,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		0,
		mip_levels);
}

void Texture::createTextureCube(uint32_t width, uint32_t height, uint32_t num_mipmaps,
	Format format, const void* data, uint32_t num_data_mipmaps)
{
	VkImageUsageFlags usage_flags = RHI::Utils::getImageUsageFlags(textureFormat);

	RHI::Utils::createImage(
		device,
		VK_IMAGE_TYPE_2D,
		width, height, 1,
		num_mipmaps, 1,
		VK_SAMPLE_COUNT_1_BIT, textureFormat, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | usage_flags,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		0,
		image,
		memory);

	VkImageLayout source_layout = VK_IMAGE_LAYOUT_UNDEFINED;

	if (data != nullptr)
	{
		// prepare for transfer
		RHI::Utils::transitionImageLayout(
			device,
			image,
			textureFormat,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			0, num_mipmaps,
			0, 1);

		// transfer data to GPU
		RHI::Utils::fillImage(
			device,
			image,
			width, height, 1,
			num_mipmaps, 1,
			RHI::Utils::getPixelSize(format),
			textureFormat,
			data,
			num_data_mipmaps,
			1);

		source_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	}

	// prepare for shader access
	RHI::Utils::transitionImageLayout(
		device,
		image,
		textureFormat,
		source_layout,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		0, num_mipmaps,
		0, 1);

	// create base sampler
	imageSampler = RHI::Utils::createSampler(device, 0, num_mipmaps);
}

void Texture::clearGPUData()
{
	vmaDestroyImage(device->getVRAMAllocator(), image, memory);

	vkDestroySampler(device->getDevice(), imageSampler, nullptr);
	imageSampler = nullptr;

	textureFormat = VK_FORMAT_UNDEFINED;
}

void Texture::clearCPUData()
{
	delete[] pixels;
	pixels = nullptr;

	width = height = 0;
}
