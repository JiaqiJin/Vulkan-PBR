#include "Swapchain.h"
#include "Device.h"
#include "PhysicalDevice.h"

#include <algorithm>
#include "../Common/Logger.h"

namespace Vulkan
{
	Swapchain::Swapchain(const std::shared_ptr<Queue>& graphics_queue, const std::shared_ptr<PresentQueue>& present_queue, bool use_vsync)
		:m_graphics_queue(graphics_queue), m_present_queue(present_queue)
	{
		VkPhysicalDevice physical_device = m_graphics_queue->GetDevicePtr()->GetPhysicalDevicePtr()->GetHandle();
		VkSurfaceKHR surface = present_queue->GetSurfacePtr()->GetHandle();

		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> surface_formats;
		std::vector<VkPresentModeKHR> present_modes;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &capabilities);

		uint32_t format_count;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, nullptr);
		if (format_count != 0) 
		{
			surface_formats.resize(format_count);
			vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, surface_formats.data());
		}

		uint32_t present_mode_count;
		vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, nullptr);
		if (present_mode_count != 0) 
		{
			present_modes.resize(present_mode_count);
			vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, present_modes.data());
		}

		if (surface_formats.empty() || present_modes.empty())
			K_ERROR("Surface format or Present mode are empty");

		// select surface format
		VkSurfaceFormatKHR surface_format = surface_formats.front();
		if (surface_formats.size() == 1 && surface_formats.front().format == VK_FORMAT_UNDEFINED)
			surface_format.format = VK_FORMAT_B8G8R8A8_UNORM;
		else 
		{
			for (const auto& i : surface_formats)
				if (i.format == VK_FORMAT_B8G8R8A8_UNORM) 
				{
					surface_format = i;
					break;
				}
		}

		// select present mode
		VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
		if (!use_vsync) 
		{
			present_mode = VK_PRESENT_MODE_IMMEDIATE_KHR;
			for (const auto& i : present_modes)
				if (i == VK_PRESENT_MODE_MAILBOX_KHR)
				{
					present_mode = i;
					break;
				}
		}

		// query extent
		VkExtent2D extent = capabilities.currentExtent;
		if (capabilities.currentExtent.width == UINT32_MAX)
		{
			int width, height;
			glfwGetWindowSize(present_queue->GetSurfacePtr()->GetGlfwWindow(), &width, &height);
			extent = { (uint32_t)width, (uint32_t)height };

			extent.width =
				std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, extent.width));
			extent.height =
				std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, extent.height));
		}

		// query image count
		uint32_t image_count = capabilities.minImageCount + 1u;
		if (capabilities.maxImageCount > 0 && image_count > capabilities.maxImageCount)
			image_count = capabilities.maxImageCount;

		// create swapchain
		VkSwapchainCreateInfoKHR create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		create_info.surface = surface;
		create_info.minImageCount = image_count;
		create_info.imageFormat = surface_format.format;
		create_info.imageColorSpace = surface_format.colorSpace;
		create_info.imageExtent = extent;
		create_info.imageArrayLayers = 1;
		create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		if (present_queue->GetFamilyIndex() != graphics_queue->GetFamilyIndex())
		{
			uint32_t queue_family_indices[] = { graphics_queue->GetFamilyIndex(), present_queue->GetFamilyIndex() };
			create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			create_info.queueFamilyIndexCount = 2;
			create_info.pQueueFamilyIndices = queue_family_indices;
		}
		else
		{
			create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			create_info.queueFamilyIndexCount = 0;
			create_info.pQueueFamilyIndices = nullptr;
		}

		create_info.preTransform = capabilities.currentTransform;
		create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		create_info.presentMode = present_mode;
		create_info.clipped = VK_TRUE;
		create_info.oldSwapchain = VK_NULL_HANDLE;

		// Set 
		m_swapchain_create_info = create_info;

		if (vkCreateSwapchainKHR(graphics_queue->GetDevicePtr()->GetHandle(), &create_info, nullptr, &m_swapchain) != VK_SUCCESS)
			K_ERROR("Failed to create Swap Chain");

		// get image count
		vkGetSwapchainImagesKHR(graphics_queue->GetDevicePtr()->GetHandle(), m_swapchain, &m_image_count, nullptr);
	}

	void Swapchain::recreate(const std::shared_ptr<Swapchain>& old_swapchain)
	{
		m_graphics_queue = old_swapchain->m_graphics_queue;
		m_present_queue = old_swapchain->m_present_queue;
		m_swapchain_create_info = old_swapchain->m_swapchain_create_info;

		VkPhysicalDevice physical_device = old_swapchain->GetDevicePtr()->GetPhysicalDevicePtr()->GetHandle();

		VkSurfaceCapabilitiesKHR capabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, m_present_queue->GetSurfacePtr()->GetHandle(),
			&capabilities);

		// query extent
		VkExtent2D extent = capabilities.currentExtent;
		if (capabilities.currentExtent.width == UINT32_MAX)
		{
			int width, height;
			glfwGetWindowSize(m_present_queue->GetSurfacePtr()->GetGlfwWindow(), &width, &height);
			extent = { (uint32_t)width, (uint32_t)height };

			extent.width =
				std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, extent.width));
			extent.height =
				std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, extent.height));
		}

		// create swapchain
		VkSwapchainCreateInfoKHR& create_info = m_swapchain_create_info;
		create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		create_info.imageExtent = extent;
		create_info.oldSwapchain = old_swapchain->GetHandle();

		if (vkCreateSwapchainKHR(old_swapchain->GetDevicePtr()->GetHandle(), &create_info, nullptr, &m_swapchain) != VK_SUCCESS)
			K_ERROR("Failed to Recreate Swap Chain class");

		// get image count
		vkGetSwapchainImagesKHR(old_swapchain->GetDevicePtr()->GetHandle(), m_swapchain, &m_image_count, nullptr);
	}

	Swapchain::~Swapchain() 
	{
		if (m_swapchain) 
		{
			vkDestroySwapchainKHR(m_graphics_queue->GetDevicePtr()->GetHandle(), m_swapchain, nullptr);
		}
	}
}