#include "SwapChain.h"
#include "Device.h"
#include "VulkanUtils.h"

#include "../Common/Logger.h"

#include <assert.h>

namespace RHI
{
	SwapChain::SwapChain(const std::shared_ptr<Device> device, void* native_window)
		: device(device), native_window(native_window)
	{
		assert(native_window != nullptr && "Invalid window");
	}

	SwapChain::~SwapChain()
	{
		shutdown();
	}

	void SwapChain::init(uint32_t width, uint32_t height, uint32_t ubo_size)
	{
		// Create surface
		surface = device->createSurface(native_window);
		if (surface == VK_NULL_HANDLE)
		{
			K_ERROR("Can't create platform surface");
		}

		present_queue_family = Utils::getPresentQueueFamily(device->getPhysicalDevice(), surface, device->getGraphicsQueueFamily());

		// Get present queue
		vkGetDeviceQueue(device->getDevice(), present_queue_family, 0, &present_queue);
		if (present_queue == VK_NULL_HANDLE)
		{
			K_ERROR("Can't get present queue from logical device");
		}

		selectOptimalSwapChainSettings(device, width, height);

		const VkSurfaceCapabilitiesKHR& capabilities = surface_capabilities;

		VkSwapchainCreateInfoKHR info = {};
		info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		info.surface = surface;
		info.minImageCount = num_images;
		info.imageFormat = surface_format.format;
		info.imageColorSpace = surface_format.colorSpace;
		info.imageExtent = sizes;
		info.imageArrayLayers = 1;
		info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		if (device->getGraphicsQueueFamily() != present_queue_family)
		{
			uint32_t families[] = { device->getGraphicsQueueFamily(), present_queue_family };
			info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			info.queueFamilyIndexCount = 2;
			info.pQueueFamilyIndices = families;
		}
		else
		{
			info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			info.queueFamilyIndexCount = 0;
			info.pQueueFamilyIndices = nullptr;
		}

		info.preTransform = capabilities.currentTransform;
		info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		info.presentMode = present_mode;
		info.clipped = VK_TRUE;
		info.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(device->getDevice(), &info, nullptr, &swap_chain) != VK_SUCCESS)
		{
			K_ERROR("vkCreateSwapchainKHR failed");
			return;
		}

		// Get surface images
		vkGetSwapchainImagesKHR(device->getDevice(), swap_chain, &num_images, nullptr);
		assert(num_images != 0 && num_images < MAX_IMAGES/*Max_Image*/);
		vkGetSwapchainImagesKHR(device->getDevice(), swap_chain, &num_images, images);

		// Create frame objects
		for (size_t i = 0; i < num_images; i++)
		{
			VkSemaphoreCreateInfo semaphore_info = {};
			semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

			if (vkCreateSemaphore(device->getDevice(), &semaphore_info, nullptr, &image_available_gpu[i]) != VK_SUCCESS)
			{
				K_ERROR("Can't create 'image available' semaphore");
				return;
			}
		}

	}

	void SwapChain::resize(uint32_t width, uint32_t height)
	{

	}

	void SwapChain::shutdown()
	{
		for (size_t i = 0; i < num_images; ++i)
		{
			images[i] = VK_NULL_HANDLE;

			vkDestroySemaphore(device->getDevice(), image_available_gpu[i], nullptr);
			image_available_gpu[i] = VK_NULL_HANDLE;
		}

		vkDestroySwapchainKHR(device->getDevice(), swap_chain, nullptr);
		swap_chain = VK_NULL_HANDLE;
		swap_chain = nullptr;
	}

	void SwapChain::selectOptimalSwapChainSettings(const std::shared_ptr<Device> device, uint32_t width, uint32_t height)
	{
		// Get surface capabilities
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->getPhysicalDevice(), surface, &surface_capabilities);

		// Select the best surface format
		uint32_t num_surface_formats = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device->getPhysicalDevice(), surface, &num_surface_formats, nullptr);
		assert(num_surface_formats != 0);

		std::vector<VkSurfaceFormatKHR> surface_formats(num_surface_formats);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device->getPhysicalDevice(), surface, &num_surface_formats, surface_formats.data());

		// Select the best format if the surface has no preferred format
		if (surface_formats.size() == 1 && surface_formats[0].format == VK_FORMAT_UNDEFINED)
		{
			surface_format = { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
		}
		// Otherwise, select one of the available formats
		else
		{
			surface_format = surface_formats[0];
			for (const auto& surfaceFormat : surface_formats)
			{
				if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM && surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				{
					surface_format = surfaceFormat;
					break;
				}
			}
		}

		// Select the best present mode
		uint32_t num_present_modes = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device->getPhysicalDevice(), surface, &num_present_modes, nullptr);
		assert(num_present_modes != 0);

		std::vector<VkPresentModeKHR> present_modes(num_present_modes);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device->getPhysicalDevice(), surface, &num_present_modes, present_modes.data());

		present_mode = VK_PRESENT_MODE_FIFO_KHR;
		for (const auto& availablePresentMode : present_modes)
		{
			// Some drivers currently don't properly support FIFO present mode,
			// so we should prefer IMMEDIATE mode if MAILBOX mode is not available
			if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
				present_mode = availablePresentMode;

			if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				present_mode = availablePresentMode;
				break;
			}
		}

		const VkSurfaceCapabilitiesKHR& capabilities = surface_capabilities;

		// Select current swap extent if window manager doesn't allow to set custom extent
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			sizes = capabilities.currentExtent;
		}
		// Otherwise, manually set extent to match the min/max extent bounds
		else
		{
			sizes.width = std::clamp(
				width,
				capabilities.minImageExtent.width,
				capabilities.maxImageExtent.width
			);

			sizes.height = std::clamp(
				height,
				capabilities.minImageExtent.height,
				capabilities.maxImageExtent.height
			);
		}

		// Simply sticking to this minimum means that we may sometimes have to wait
		// on the driver to complete internal operations before we can acquire another image to render to.
		// Therefore it is recommended to request at least one more image than the minimum
		num_images = capabilities.minImageCount + 1;

		// We should also make sure to not exceed the maximum number of images while doing this,
		// where 0 is a special value that means that there is no maximum
		if (capabilities.maxImageCount > 0)
			num_images = std::min(num_images, capabilities.maxImageCount);
	}
}