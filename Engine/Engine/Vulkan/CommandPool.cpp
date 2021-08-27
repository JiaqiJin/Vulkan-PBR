#include "CommandPool.h"

#include "../Common/Logger.h"

namespace Vulkan
{
	CommandPool::CommandPool(const std::shared_ptr<Queue>& queue, VkCommandPoolCreateFlags flags)
		: m_queue(queue)
	{
		VkCommandPoolCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		create_info.queueFamilyIndex = queue->GetFamilyIndex();
		create_info.flags = flags;

		if (vkCreateCommandPool(queue->GetDevicePtr()->GetHandle(), &create_info, nullptr, &m_command_pool) != VK_SUCCESS)
			K_ERROR("Failed to create Command Pool");
	}

	VkResult CommandPool::Reset(VkCommandPoolResetFlags flags) const
	{
		return vkResetCommandPool(GetDevicePtr()->GetHandle(), m_command_pool, flags);
	}

	CommandPool::~CommandPool() 
	{
		if (m_command_pool)
			vkDestroyCommandPool(m_queue->GetDevicePtr()->GetHandle(), m_command_pool, nullptr);
	}
}
