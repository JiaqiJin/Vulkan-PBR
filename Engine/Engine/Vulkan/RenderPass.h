#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

#include "DeviceObjectBase.h"

namespace Vulkan
{
	class RenderPass : public DeviceObjectBase
	{
	public:
		RenderPass(const std::shared_ptr<Device>& device,const VkRenderPassCreateInfo& create_info);
		~RenderPass();
		VkRenderPass GetHandle() const { return m_render_pass; }
		const std::shared_ptr<Device>& GetDevicePtr() const override { return m_device; }

	private:
		std::shared_ptr<Device> m_device;
		VkRenderPass m_render_pass{ VK_NULL_HANDLE };
	};
}