#include "FrameManager.h"
#include "CommandBuffer.h"

namespace Vulkan
{
	void FrameManager::Initialize(const std::shared_ptr<Queue>& graphics_queue,
		const std::shared_ptr<PresentQueue>& present_queue, bool use_vsync, uint32_t frame_count)
	{
		m_swapchain = std::make_shared<Swapchain>(graphics_queue, present_queue, use_vsync);
		m_swapchain_images = SwapchainImage::Create(m_swapchain);
		m_swapchain_image_views.resize(m_swapchain->GetImageCount());
		for (uint32_t i = 0; i < m_swapchain->GetImageCount(); ++i)
			m_swapchain_image_views[i] = std::make_shared<ImageView>(m_swapchain_images[i]);

		m_frame_count = frame_count;
		m_image_fences.resize(m_swapchain->GetImageCount(), nullptr);

		m_frame_fences.resize(frame_count);
		m_render_done_semaphores.resize(frame_count);
		m_acquire_done_semaphores.resize(frame_count);

		for (uint32_t i = 0; i < frame_count; ++i)
		{
			m_frame_fences[i] = std::make_shared<Fence>(m_swapchain->GetDevicePtr(), VK_FENCE_CREATE_SIGNALED_BIT);
			m_render_done_semaphores[i] = std::make_shared<Semaphore>(m_swapchain->GetDevicePtr());
			m_acquire_done_semaphores[i] = std::make_shared<Semaphore>(m_swapchain->GetDevicePtr());
		}
	}

	bool FrameManager::AcquireNextImage()
	{
		m_frame_fences[m_current_frame]->Wait();
		return false;
	}

	void FrameManager::SubmitAndPresent(const std::shared_ptr<CommandBuffer>& command_buffer)
	{

	}

	void FrameManager::recreate_swapchain()
	{
		GLFWwindow* window = m_swapchain->GetPresentQueuePtr()->GetSurfacePtr()->GetGlfwWindow();
		int width = 0, height = 0;
		glfwGetFramebufferSize(window, &width, &height);
		while (width == 0 || height == 0) 
		{
			glfwGetFramebufferSize(window, &width, &height);
			glfwWaitEvents();
		}

		for (const auto& i : m_frame_fences)
			i->Wait();
		m_swapchain->recreate(m_swapchain);

		m_swapchain_images = SwapchainImage::Create(m_swapchain);
		m_swapchain_image_views.clear();
		m_swapchain_image_views.resize(m_swapchain->GetImageCount());
		for (uint32_t i = 0; i < m_swapchain->GetImageCount(); ++i)
			m_swapchain_image_views[i] = std::make_shared<ImageView>(m_swapchain_images[i]);
	}
}