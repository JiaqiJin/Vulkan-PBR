#include "RenderPass.h"
#include "Device.h"
#include "../Common/Logger.h"

namespace Vulkan
{
	RenderPass::RenderPass(const std::shared_ptr<Device>& device, const VkRenderPassCreateInfo& create_info)
		: m_device(device)
	{
		if (vkCreateRenderPass(device->GetHandle(), &create_info, nullptr, &m_render_pass) != VK_SUCCESS)
			K_ERROR("Failed to create RenderPass");
	}

	RenderPass::~RenderPass()
	{
		if (m_render_pass)
			vkDestroyRenderPass(m_device->GetHandle(), m_render_pass, nullptr);
	}
}