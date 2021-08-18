#include "DescriptorSetLayout.h"
#include "Device.h"

#include "../Common/Logger.h"

namespace Vulkan
{
	DescriptorSetLayout::DescriptorSetLayout(const std::shared_ptr<Device>& device, const std::vector<VkDescriptorSetLayoutBinding>& bindings)
		: m_device(device)
	{
		VkDescriptorSetLayoutCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		info.bindingCount = bindings.size();
		info.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(device->GetHandle(), &info, nullptr, &m_descriptor_set_layout) != VK_SUCCESS)
			K_ERROR("Failed to create DescriptorSetLayout");
	}

	DescriptorSetLayout::~DescriptorSetLayout()
	{
		if (m_descriptor_set_layout)
			vkDestroyDescriptorSetLayout(m_device->GetHandle(), m_descriptor_set_layout, nullptr);
	}
}