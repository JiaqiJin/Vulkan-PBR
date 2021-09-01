#include "DescriptorSet.h"

#include "../Common/Logger.h"

namespace Vulkan
{
	DescriptorSet::DescriptorSet(const std::shared_ptr<DescriptorPool>& descriptor_pool,
		const std::shared_ptr<DescriptorSetLayout>& descriptor_set_layout)
		: m_descriptor_pool(descriptor_pool), m_descriptor_set_layout(descriptor_set_layout)
	{
		VkDescriptorSetAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		alloc_info.descriptorPool = descriptor_pool->GetHandle();
		alloc_info.descriptorSetCount = 1;
		VkDescriptorSetLayout layout = descriptor_set_layout->GetHandle();
		alloc_info.pSetLayouts = &layout;

		if (vkAllocateDescriptorSets(descriptor_pool->GetDevicePtr()->GetHandle(), &alloc_info, &m_descriptor_set) != VK_SUCCESS)
			K_ERROR("Failed to create Descriptor Set");
	}

	DescriptorSet::~DescriptorSet()
	{
		if (m_descriptor_set)
			vkFreeDescriptorSets(m_descriptor_pool->GetDevicePtr()->GetHandle(), m_descriptor_pool->GetHandle(), 1,
				&m_descriptor_set);
	}
}