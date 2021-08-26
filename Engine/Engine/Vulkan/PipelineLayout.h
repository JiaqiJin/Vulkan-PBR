#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

#include "DeviceObjectBase.h"

namespace Vulkan
{
	class DescriptorSetLayout;

	class PipelineLayout : public DeviceObjectBase
	{
	public:
		PipelineLayout(const std::shared_ptr<Device>& device,
			const std::vector<std::shared_ptr<DescriptorSetLayout>>& descriptor_layouts,
			const std::vector<VkPushConstantRange>& push_constant_ranges);
		~PipelineLayout();

		VkPipelineLayout GetHandle() const { return m_pipeline_layout; }

		const std::shared_ptr<Device>& GetDevicePtr() const override { return m_device; }
		const std::vector<std::shared_ptr<DescriptorSetLayout>>& GetDescriptorSetPtrs() const { return m_descriptor_layout; }

	private:
		std::shared_ptr<Device> m_device;
		std::vector<std::shared_ptr<DescriptorSetLayout>> m_descriptor_layout;

		VkPipelineLayout m_pipeline_layout{ VK_NULL_HANDLE };
	};
}