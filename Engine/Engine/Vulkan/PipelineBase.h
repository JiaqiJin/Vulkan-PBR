#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

#include "DeviceObjectBase.h"
#include "PipelineLayout.h"

namespace Vulkan
{
	class PipelineBase : public DeviceObjectBase
	{
	public:
		PipelineBase(const std::shared_ptr<PipelineLayout>& pipeline_layout);
		~PipelineBase();

		VkPipeline GetHandle() const { return m_pipeline; }

		virtual VkPipelineBindPoint GetBindPoint() const = 0;

		const std::shared_ptr<Device>& GetDevicePtr() const override { return m_pipeline_layout->GetDevicePtr(); }
		const std::shared_ptr<PipelineLayout>& GetPipelineLayoutPtr() const { return m_pipeline_layout; }
	protected:
		std::shared_ptr<PipelineLayout> m_pipeline_layout;
		VkPipeline m_pipeline{ VK_NULL_HANDLE };
	};
}