#include "CommandBuffer.h"
#include "Device.h"

#include "../Common/Logger.h"

namespace Vulkan
{
	CommandBuffer::CommandBuffer(const std::shared_ptr<CommandPool>& command_pool, VkCommandBufferLevel level)
		: m_command_pool(command_pool)
	{
		VkCommandBufferAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		alloc_info.commandPool = command_pool->GetHandle();
		alloc_info.level = level;
		alloc_info.commandBufferCount = 1;

		if (vkAllocateCommandBuffers(command_pool->GetDevicePtr()->GetHandle(), &alloc_info, &m_command_buffer) != VK_SUCCESS)
			K_ERROR("Failed to create Command Buffer");
	}

	CommandBuffer::~CommandBuffer()
	{
		if (m_command_buffer) {
			vkFreeCommandBuffers(m_command_pool->GetQueuePtr()->GetDevicePtr()->GetHandle(),m_command_pool->GetHandle(), 1, &m_command_buffer);
		}
	}

	VkResult CommandBuffer::Submit(const SemaphoreStageGroup& wait_semaphores, const SemaphoreGroup& signal_semaphores,
		const std::shared_ptr<Fence>& fence) const
	{
		VkSubmitInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		info.commandBufferCount = 1;
		info.pCommandBuffers = &m_command_buffer;

		if (wait_semaphores.GetCount())
		{
			info.waitSemaphoreCount = wait_semaphores.GetCount();
			info.pWaitSemaphores = wait_semaphores.GetSemaphoresPtr();
			info.pWaitDstStageMask = wait_semaphores.GetWaitStagesPtr();
		}

		if (signal_semaphores.GetCount())
		{
			info.signalSemaphoreCount = signal_semaphores.GetCount();
			info.pSignalSemaphores = signal_semaphores.GetSemaphoresPtr();
		}

		std::lock_guard<std::mutex> lock_guard{ m_command_pool->GetQueuePtr()->GetMutex() };

		return vkQueueSubmit(m_command_pool->GetQueuePtr()->GetHandle(), 1, &info,
			fence ? fence->GetHandle() : VK_NULL_HANDLE);
	}

	VkResult CommandBuffer::Submit(const std::shared_ptr<Fence>& fence = nullptr) const
	{
		VkSubmitInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		info.commandBufferCount = 1;
		info.pCommandBuffers = &m_command_buffer;

		std::lock_guard<std::mutex> lock_guard{ m_command_pool->GetQueuePtr()->GetMutex() };

		return vkQueueSubmit(m_command_pool->GetQueuePtr()->GetHandle(), 1, &info,
			fence ? fence->GetHandle() : VK_NULL_HANDLE);
	}
}