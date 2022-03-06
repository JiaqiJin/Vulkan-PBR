#pragma once

#include <vulkan/vulkan.h>

namespace RHI
{
	class VulkanContext;

	class Platform
	{
	public:
		static const char* GetInstanceExtension();
		static VkSurfaceKHR CreateSurface(const VulkanContext* context, void* native_window);
		static void DestroySurface(const VulkanContext* context, VkSurfaceKHR surface);
	};
}