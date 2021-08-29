#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>
#include <map>

#include "DeviceObjectBase.h"
#include "BufferBase.h"
#include "CommandPool.h"
#include "DescriptorSet.h"
#include "Fence.h"
#include "Framebuffer.h"
#include "PipelineBase.h"
#include "QueryPool.h"
#include "RenderPass.h"
#include "Semaphore.h"

namespace Vulkan
{
	class CommandBuffer : public DeviceObjectBase
	{
	public:
		CommandBuffer(const std::shared_ptr<CommandPool>& command_pool, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		~CommandBuffer();

		VkResult Submit(const SemaphoreStageGroup& wait_semaphores = SemaphoreStageGroup(),
			const SemaphoreGroup& signal_semaphores = SemaphoreGroup(),
			const std::shared_ptr<Fence>& fence = nullptr) const;

		VkResult Submit(const std::shared_ptr<Fence>& fence = nullptr) const;
	private:
		VkCommandBuffer m_command_buffer{ VK_NULL_HANDLE };
		std::shared_ptr<CommandPool> m_command_pool;
	};
}