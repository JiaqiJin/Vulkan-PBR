#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

#include "DeviceObjectBase.h"

namespace Vulkan
{
	class DescriptorSetLayout : public DeviceObjectBase
	{
	public:
		DescriptorSetLayout(const std::shared_ptr<Device>& device, const std::vector<VkDescriptorSetLayoutBinding>& bindings);
		~DescriptorSetLayout();
		VkDescriptorSetLayout GetHandle() const { return m_descriptor_set_layout; }
		const std::shared_ptr<Device>& GetDevicePtr() const override { return m_device; }

	private:
		std::shared_ptr<Device> m_device;
		VkDescriptorSetLayout m_descriptor_set_layout{ VK_NULL_HANDLE };
	};
}