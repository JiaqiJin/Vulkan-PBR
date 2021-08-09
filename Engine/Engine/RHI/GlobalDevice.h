#pragma once

#include <vulkan/vulkan.h>
#include "../Common/GraphicsEnums.h"
#include "../Vendor/vma/vk_mem_alloc.h"

#include <memory>

#define MAX_ATTRIBUTES 16

namespace RHI
{
	class Device;
	class FrameBuffer;

	struct VertexBuffer
	{
		BufferType type{ BufferType::STATIC };
		VkBuffer buffer{ VK_NULL_HANDLE };
		VmaAllocation memory{ VK_NULL_HANDLE };
		uint16_t vertex_size{ 0 };
		uint32_t num_vertices{ 0 };
		uint8_t num_attributes{ 0 };
		VkFormat attribute_formats[MAX_ATTRIBUTES];
		uint32_t attribute_offsets[MAX_ATTRIBUTES];
	};

	struct IndexBuffer
	{
		BufferType type{ BufferType::STATIC };
		VkBuffer buffer{ VK_NULL_HANDLE };
		VmaAllocation memory{ VK_NULL_HANDLE };
		VkIndexType index_type{ VK_INDEX_TYPE_UINT16 };
		uint32_t num_indices{ 0 };
	};

	struct Texture
	{
		VkImage image{ VK_NULL_HANDLE };
		VkSampler sampler{ VK_NULL_HANDLE };
		VmaAllocation memory{ VK_NULL_HANDLE };
		VkImageType type{ VK_IMAGE_TYPE_2D };
		VkFormat format{ VK_FORMAT_UNDEFINED };
		uint32_t width{ 0 };
		uint32_t height{ 0 };
		uint32_t depth{ 0 };
		uint32_t num_layers{ 0 };
		uint32_t num_mipmaps{ 0 };
		VkImageTiling tiling{ VK_IMAGE_TILING_OPTIMAL };
		VkSampleCountFlagBits samples{ VK_SAMPLE_COUNT_1_BIT };
		VkImageCreateFlags flags{ 0 };
	};	

	class GlobalDevice
	{
	public:
		GlobalDevice(const char* application_name, const char* engine_name);
		~GlobalDevice();

		inline const std::shared_ptr<Device> getDevice() const { return device; }

		void wait();

		// Object Initialization
		std::shared_ptr<FrameBuffer> createFramebuffer(const FrameBufferAttachment* attachments, uint8_t num_attachments);

		// Texture

	private:
		std::shared_ptr<Device> device;
	};
}