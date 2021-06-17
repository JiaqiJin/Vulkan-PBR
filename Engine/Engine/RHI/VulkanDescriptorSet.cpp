#include "VulkanDescriptorSet.h"
#include <stdexcept>

namespace RHI
{
	std::vector<VkDescriptorSet> VulkanDescriptorSet::build(VkDescriptorSetLayout descriptorSetLayout)
	{
		std::vector<VkDescriptorSetLayout> layouts(imageCount, descriptorSetLayout);

		VkDescriptorSetAllocateInfo descriptorSetAllocInfo = {};
		descriptorSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocInfo.descriptorPool = context.descriptorPool;
		descriptorSetAllocInfo.descriptorSetCount = imageCount;
		descriptorSetAllocInfo.pSetLayouts = layouts.data();

		descriptorSets.resize(imageCount);
		if (vkAllocateDescriptorSets(context.device, &descriptorSetAllocInfo, descriptorSets.data()) != VK_SUCCESS)
			throw std::runtime_error("Can't allocate descriptor sets");

		return descriptorSets;
	}
}