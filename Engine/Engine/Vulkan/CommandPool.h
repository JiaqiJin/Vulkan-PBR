#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>
#include <cinttypes>

#include "DeviceObjectBase.h"
#include "Queue.h"

namespace Vulkan
{
	class CommandPool : public DeviceObjectBase
	{
	public:
		CommandPool(const std::shared_ptr<Queue>& queue, VkCommandPoolCreateFlags flags = 0);
		~CommandPool();

		VkResult Reset(VkCommandPoolResetFlags flags = 0) const;

		VkCommandPool GetHandle() const { return m_command_pool; }
		const std::shared_ptr<Queue>& GetQueuePtr() const { return m_queue; }
		const std::shared_ptr<Device>& GetDevicePtr() const override { return m_queue->GetDevicePtr(); };

	private:
		VkCommandPool m_command_pool{ VK_NULL_HANDLE };

		std::shared_ptr<Queue> m_queue;
	};
}