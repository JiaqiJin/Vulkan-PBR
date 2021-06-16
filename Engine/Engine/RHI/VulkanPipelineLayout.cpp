#include "VulkanPipelineLayout.h"
#include "VulkanUtils.h"
#include <stdexcept>

namespace RHI
{
	void VulkanPipelineLayout::addDescriptorSetLayout(VkDescriptorSetLayout descriptorSetLayout)
	{
		descriptorSetLayouts.push_back(descriptorSetLayout);
	}

	VkPipelineLayout VulkanPipelineLayout::build()
	{
		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 0; // TODO: add support for push constants
		pipelineLayoutInfo.pPushConstantRanges = nullptr;

		if (vkCreatePipelineLayout(context.device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
			throw std::runtime_error("Can't create pipeline layout");

		return pipelineLayout;
	}
}