#pragma once

#include <vulkan/vulkan.h>
#include "../Vendor/vma/vk_mem_alloc.h"
#include <memory>
#include <vector>

namespace Vulkan
{
	class PhysicalDevice;
	class DeviceCreateInfo;

	class Device
	{
	public:
		static std::shared_ptr<Device> Create(const DeviceCreateInfo& device_create_info, void* p_next = nullptr);

		~Device();

		VmaAllocator GetAllocatorHandle() const { return m_allocator; }

		VkPipelineCache GetPipelineCacheHandle() const { return m_pipeline_cache; }

		const std::shared_ptr<PhysicalDevice>& GetPhysicalDevicePtr() const { return m_physical_device; }

		VkDevice GetHandle() const { return m_device; }

		VkResult WaitIdle() const;

	private:
		VkDevice m_device{ VK_NULL_HANDLE };
		VkPipelineCache m_pipeline_cache{ VK_NULL_HANDLE };
		VmaAllocator m_allocator{ VK_NULL_HANDLE };

		std::shared_ptr<PhysicalDevice> m_physical_device;

		VkResult create_allocator();

		VkResult create_device(const std::vector<VkDeviceQueueCreateInfo>& queue_create_infos,
			const std::vector<const char*>& extensions, void* p_next);

		VkResult create_pipeline_cache();
	};
}