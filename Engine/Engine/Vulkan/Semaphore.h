#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

#include "DeviceObjectBase.h"

namespace Vulkan
{
	class Semaphore : public DeviceObjectBase
	{
	public:
		Semaphore(const std::shared_ptr<Device>& device);
		~Semaphore();

		VkSemaphore GetHandle() const { return m_semaphore; }
		const std::shared_ptr<Device>& GetDevicePtr() const override { return m_device; }
	private:
		std::shared_ptr<Device> m_device;

		VkSemaphore m_semaphore{ VK_NULL_HANDLE };
	};
}