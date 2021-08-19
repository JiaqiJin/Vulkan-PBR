#pragma once

#include <vulkan/vulkan.h>
#include "../Vendor/vma/vk_mem_alloc.h"
#include "ImageBase.h"
#include <memory>
#include <vector>

namespace Vulkan
{
	class Device;
	class Queue;

	class Image : public ImageBase
	{
	public:
		Image(const std::shared_ptr<Device>& device, const VkExtent2D& size,
			uint32_t mip_level, VkFormat format, VkImageUsageFlags usage, const std::vector<std::shared_ptr<Queue>>& access_queue = {});

		void Init(VmaMemoryUsage memory_usage, const VkImageCreateInfo& create_info, const std::vector<std::shared_ptr<Queue>>& access_queues = {});

		~Image();

		static uint32_t QueryMipLevel(uint32_t w);
		static uint32_t QueryMipLevel(const VkExtent2D& size);
		static uint32_t QueryMipLevel(const VkExtent3D& size);

	private:
		std::shared_ptr<Device> m_device;

		VmaAllocation m_allocation{ VK_NULL_HANDLE };
	};
}