#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "RendererContext.h"

namespace RHI
{
	class DescriptorSetLayout
	{
	public:
		DescriptorSetLayout(const RendererContext& context)
			: context(context) { }

		inline VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

		void addDescriptorBinding(VkDescriptorType type, VkShaderStageFlags shaderStageFlags, int descriptorCount = 1);

		VkDescriptorSetLayout build();
	private:
		RendererContext context;

		std::vector<VkDescriptorSetLayoutBinding> bindings;

		VkDescriptorSetLayout descriptorSetLayout{ VK_NULL_HANDLE };
	};
}