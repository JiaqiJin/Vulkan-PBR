#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

#include "DeviceObjectBase.h"
#include "Surface.h"
#include "Fence.h"
#include "Semaphore.h"
#include "Queue.h"

namespace Vulkan
{
	class Swapchain : public DeviceObjectBase
	{
	public:
		Swapchain(const std::shared_ptr<Queue>& graphics_queue, const std::shared_ptr<PresentQueue>& present_queue, bool use_vsync);
		~Swapchain();

		//static std::shared_ptr<Swapchain> CreateNewSwapChain(const std::shared_ptr<Swapchain>& old_swapchain);
		void recreate(const std::shared_ptr<Swapchain>& old_swapchain);

		uint32_t GetImageCount() const { return m_image_count; }
		VkSwapchainKHR GetHandle() const { return m_swapchain; };
		VkFormat GetImageFormat() const { return m_swapchain_create_info.imageFormat; }
		VkColorSpaceKHR GetImageColorSpace() const { return m_swapchain_create_info.imageColorSpace; }
		VkExtent2D GetExtent() const { return m_swapchain_create_info.imageExtent; }
		VkSwapchainCreateInfoKHR GetCreateInfo() const { return m_swapchain_create_info; }
		const std::shared_ptr<Queue>& GetGraphicsQueuePtr() const { return m_graphics_queue; }
		const std::shared_ptr<PresentQueue>& GetPresentQueuePtr() const { return m_present_queue; }
		const std::shared_ptr<Device>& GetDevicePtr() const override { return m_graphics_queue->GetDevicePtr(); }
	
	private:
		VkSwapchainKHR m_swapchain{ VK_NULL_HANDLE };

		VkSwapchainCreateInfoKHR m_swapchain_create_info;
		uint32_t m_image_count;

		std::shared_ptr<Queue> m_graphics_queue;
		std::shared_ptr<PresentQueue> m_present_queue;
	};
}