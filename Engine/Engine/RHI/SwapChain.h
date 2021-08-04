#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>
#include "GlobalDevice.h"

namespace RHI
{
	class Device;

	class SwapChain
	{
	public:
		SwapChain(const std::shared_ptr<Device> device, void* native_window);
		~SwapChain();

		void init(uint32_t width, uint32_t height, uint32_t ubo_size = 0);
		void resize(uint32_t width, uint32_t height);
		void shutdown();

	private:
		void selectOptimalSwapChainSettings(const std::shared_ptr<Device> device, uint32_t width, uint32_t height);

	private:
		const std::shared_ptr<Device> device;
		void* native_window = nullptr;

		// Swap Chain objects
		VkSwapchainKHR swap_chain{ nullptr };
		VkExtent2D sizes{ 0, 0 };

		VkSurfaceKHR surface{ nullptr };
		VkSurfaceCapabilitiesKHR surface_capabilities;
		VkSurfaceFormatKHR surface_format;

		uint32_t present_queue_family{ 0xFFFF };
		VkQueue present_queue{ VK_NULL_HANDLE };
		VkPresentModeKHR present_mode{ VK_PRESENT_MODE_FIFO_KHR };

		VkImage msaa_color_image{ VK_NULL_HANDLE };
		VkImageView msaa_color_view{ VK_NULL_HANDLE };
		VmaAllocation msaa_color_memory{ VK_NULL_HANDLE };

		VkImage depth_image{ VK_NULL_HANDLE };
		VkImageView depth_view{ VK_NULL_HANDLE };
		VmaAllocation depth_memory{ VK_NULL_HANDLE };

		uint32_t num_images{ 0 };
		uint32_t current_image{ 0 };

		enum
		{
			MAX_IMAGES = 8,
		};

		VkSemaphore image_available_gpu[SwapChain::MAX_IMAGES];
		VkImage images[SwapChain::MAX_IMAGES];
	};
}