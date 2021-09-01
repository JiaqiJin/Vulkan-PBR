#include "DescriptorPool.h"

#include "../Common/Logger.h"

namespace Vulkan
{
	DescriptorPool::DescriptorPool(const std::shared_ptr<Device>& device, uint32_t max_sets, const std::vector<VkDescriptorPoolSize>& sizes)
		: m_device(device)
	{
		VkDescriptorPoolCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		create_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		create_info.maxSets = max_sets;
		create_info.poolSizeCount = sizes.size();
		create_info.pPoolSizes = sizes.data();

		if (vkCreateDescriptorPool(device->GetHandle(), &create_info, nullptr, &m_descriptor_pool) != VK_SUCCESS)
			K_ERROR("Failed to create Descriptor Pool");
	}

	DescriptorPool::~DescriptorPool()
	{
		if (m_descriptor_pool)
			vkDestroyDescriptorPool(m_device->GetHandle(), m_descriptor_pool, nullptr);
	}
}