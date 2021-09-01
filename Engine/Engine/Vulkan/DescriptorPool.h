#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

#include "DeviceObjectBase.h"

namespace Vulkan
{
	class DescriptorPool : public DeviceObjectBase
	{
	public:
		DescriptorPool(const std::shared_ptr<Device>& device, uint32_t max_sets, const std::vector<VkDescriptorPoolSize>& sizes);
		~DescriptorPool();

		VkDescriptorPool GetHandle() const { return m_descriptor_pool; }
		const std::shared_ptr<Device>& GetDevicePtr() const override { return m_device; }
	private:
		std::shared_ptr<Device> m_device;
		VkDescriptorPool m_descriptor_pool{ VK_NULL_HANDLE };
	};
}