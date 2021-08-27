#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>
#include <map>

#include "DeviceObjectBase.h"

namespace Vulkan
{
	class CommandBuffer : public DeviceObjectBase
	{
	public:

	private:
		VkCommandBuffer m_command_buffer{ VK_NULL_HANDLE };
		std::shared_ptr<class CommandPool> m_command_pool;
	};
}