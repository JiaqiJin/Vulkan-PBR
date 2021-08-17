#pragma once

#include <vulkan/vulkan.h>
#include "../Vendor/vma/vk_mem_alloc.h"
#include "BufferBase.h"
#include <memory>
#include <vector>

namespace Vulkan
{
	class Device;
	class Queue;

	class Buffer : public BufferBase
	{
	public:
		Buffer(const std::shared_ptr<Device>& device, VkDeviceSize size,
			VmaMemoryUsage memory_usage, VkBufferUsageFlags buffer_usage,
			const std::vector<std::shared_ptr<Queue>>& access_queues = {});
		~Buffer();

		void* Map() const;
		void Unmap() const;

	protected:
		void Init(const VkBufferCreateInfo& create_info, VmaMemoryUsage memory_usage,
			const std::vector<std::shared_ptr<Queue>>& access_queues);

	protected:
		std::shared_ptr<Device> m_device;
		VmaAllocation m_allocation{ VK_NULL_HANDLE };
	};

	class StagingBuffer : public Buffer
	{
	public:
		StagingBuffer(const std::shared_ptr<Device>& device, VkDeviceSize size,
			const std::vector<std::shared_ptr<Queue>>& access_queues);
	};
}