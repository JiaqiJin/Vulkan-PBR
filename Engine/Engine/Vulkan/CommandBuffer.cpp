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

	VkResult CommandBuffer::Submit(const std::shared_ptr<Fence>& fence) const
	{
		VkSubmitInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		info.commandBufferCount = 1;
		info.pCommandBuffers = &m_command_buffer;

		std::lock_guard<std::mutex> lock_guard{ m_command_pool->GetQueuePtr()->GetMutex() };

		return vkQueueSubmit(m_command_pool->GetQueuePtr()->GetHandle(), 1, &info,
			fence ? fence->GetHandle() : VK_NULL_HANDLE);
	}

	VkResult CommandBuffer::Reset(VkCommandBufferResetFlags flags) const
	{
		return vkResetCommandBuffer(m_command_buffer, flags);
	}

	VkResult CommandBuffer::Begin(VkCommandBufferUsageFlags usage) const
	{
		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = usage;
		return vkBeginCommandBuffer(m_command_buffer, &begin_info);
	}

	VkResult CommandBuffer::End() const
	{
		return vkEndCommandBuffer(m_command_buffer);
	}

	void CommandBuffer::CmdBeginRenderPass(const std::shared_ptr<RenderPass>& render_pass,
		const std::shared_ptr<Framebuffer>& framebuffer,
		const std::vector<VkClearValue>& clear_values, const VkOffset2D& offset,
		const VkExtent2D& extent,
		VkSubpassContents subpass_contents) const
	{
		VkRenderPassBeginInfo render_begin_info = {};
		render_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		render_begin_info.renderPass = render_pass->GetHandle();
		render_begin_info.framebuffer = framebuffer->GetHandle();
		render_begin_info.renderArea.offset = offset;
		render_begin_info.renderArea.extent = extent;
		render_begin_info.clearValueCount = clear_values.size();
		render_begin_info.pClearValues = clear_values.data();

		vkCmdBeginRenderPass(m_command_buffer, &render_begin_info, subpass_contents);
	}

	void CommandBuffer::CmdBeginRenderPass(const std::shared_ptr<RenderPass>& render_pass,
		const std::shared_ptr<Framebuffer>& framebuffer,
		const std::vector<VkClearValue>& clear_values,
		VkSubpassContents subpass_contents) const
	{
		VkRenderPassBeginInfo render_begin_info = {};
		render_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		render_begin_info.renderPass = render_pass->GetHandle();
		render_begin_info.framebuffer = framebuffer->GetHandle();
		render_begin_info.renderArea.offset = { 0, 0 };
		render_begin_info.renderArea.extent = framebuffer->GetExtent();
		render_begin_info.clearValueCount = clear_values.size();
		render_begin_info.pClearValues = clear_values.data();

		vkCmdBeginRenderPass(m_command_buffer, &render_begin_info, subpass_contents);
	}

	void CommandBuffer::CmdEndRenderPass() const
	{
		vkCmdEndRenderPass(m_command_buffer);
	}
}