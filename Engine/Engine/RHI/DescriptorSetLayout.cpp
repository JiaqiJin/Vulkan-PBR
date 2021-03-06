#include "DescriptorSetLayout.h"
#include "VulkanUtils.h"
#include "VulkanContext.h"
#include <stdexcept>

namespace RHI
{
	void DescriptorSetLayout::addDescriptorBinding(VkDescriptorType type, VkShaderStageFlags shaderStageFlags, int descriptorCount)
	{
		VkDescriptorSetLayoutBinding descriptorBinding = {};
		descriptorBinding.binding = static_cast<uint32_t>(bindings.size());
		descriptorBinding.descriptorType = type;
		descriptorBinding.descriptorCount = descriptorCount;
		descriptorBinding.stageFlags = shaderStageFlags;

		bindings.push_back(descriptorBinding);
	}

	VkDescriptorSetLayout DescriptorSetLayout::build()
	{
		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo = {};
		descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		descriptorSetLayoutInfo.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(context->getDevice(), &descriptorSetLayoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
			throw std::runtime_error("Can't create descriptor set layout");

		return descriptorSetLayout;
	}
}