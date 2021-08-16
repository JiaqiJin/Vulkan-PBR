#include "Fence.h"
#include "Device.h"

#include "../Common/Logger.h"

namespace Vulkan
{
	Fence::Fence(const std::shared_ptr<Device>& device, VkFenceCreateFlags flags)
		: m_device(device)
	{
		VkFenceCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		info.flags = flags;

		if (vkCreateFence(device->GetHandle(), &info, nullptr, &m_fence) != VK_SUCCESS)
			K_ERROR("Failed create Fence");
	}

	Fence::~Fence()
	{
		if (m_fence)
			vkDestroyFence(m_device->GetHandle(), m_fence, nullptr);
	}

	bool Fence::Signaled() const
	{
		return vkGetFenceStatus(m_device->GetHandle(), m_fence) == VK_SUCCESS;
	}

	VkResult Fence::Wait(uint64_t timeout) const
	{
		return vkWaitForFences(m_device->GetHandle(), 1, &m_fence, VK_TRUE, timeout);
	}

	VkResult Fence::Reset() const
	{
		return vkResetFences(m_device->GetHandle(), 1, &m_fence);
	}
}