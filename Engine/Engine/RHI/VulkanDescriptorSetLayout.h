#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "VulkanRendererContext.h"

namespace RHI
{
	class VulkanDescriptorSetLayout
	{
	public:
		VulkanDescriptorSetLayout(const VulkanRendererContext& context)
			: context(context) { }

		inline VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

		void addDescriptorBinding(VkDescriptorType type, VkShaderStageFlags shaderStageFlags, int descriptorCount = 1);

		VkDescriptorSetLayout build();
	private:
		VulkanRendererContext context;

		std::vector<VkDescriptorSetLayoutBinding> bindings;

		VkDescriptorSetLayout descriptorSetLayout{ VK_NULL_HANDLE };
	};
}