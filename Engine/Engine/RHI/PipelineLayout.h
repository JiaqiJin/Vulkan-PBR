#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace RHI
{
	class VulkanContext;

	// The pipeline layout represents a sequence of descriptor sets with each having a specific layout (uniform).
	// This sequence of layouts is used to determine the interface between shader stages and shader resources. 
	// Each pipeline is created using a pipeline layout
	class PipelineLayout
	{
	public:
		PipelineLayout(const VulkanContext* context)
			: context(context) { }

		inline VkPipelineLayout getPipelineLayout() const { return pipelineLayout; }

		void addDescriptorSetLayout(VkDescriptorSetLayout descriptorSetLayout);

		VkPipelineLayout build();
	private:
		const VulkanContext* context{ nullptr };

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts;

		VkPipelineLayout pipelineLayout{ VK_NULL_HANDLE };
	};
}