#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace RHI
{
	class VulkanContext;

	// A Descriptor is a way for shader to freely access respirces likes buffers and images
	class DescriptorSetLayout
	{
	public:
		DescriptorSetLayout(const VulkanContext* context)
			: context(context) { }

		inline VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

		void addDescriptorBinding(VkDescriptorType type, VkShaderStageFlags shaderStageFlags, int descriptorCount = 1);

		VkDescriptorSetLayout build();
	private:
		const VulkanContext* context{ nullptr };

		// Descriptor binding provides detail for shader pipeline creation (uniform buffer or image samples etc)
		std::vector<VkDescriptorSetLayoutBinding> bindings;
		// Descriptor set layout
		VkDescriptorSetLayout descriptorSetLayout{ VK_NULL_HANDLE };
	};
}