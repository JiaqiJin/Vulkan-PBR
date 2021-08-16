#pragma once

#include <vulkan/vulkan.h>
#include <cstdint>
#include <memory>
#include <vector>

#include "DeviceObjectBase.h"

namespace Vulkan
{
	
	class Fence : public DeviceObjectBase
	{
	public:
		Fence(const std::shared_ptr<Device>& device, VkFenceCreateFlags flags = 0);

		~Fence();

		bool Signaled() const;

		VkResult Wait(uint64_t timeout = UINT64_MAX) const;

		VkResult Reset() const;

		VkFence GetHandle() const { return m_fence; }
		const std::shared_ptr<Device>& GetDevicePtr() const override { return m_device; }

	private:
		std::shared_ptr<Device> m_device;
		VkFence m_fence{ VK_NULL_HANDLE };
	};
}