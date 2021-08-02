#pragma once

#include <vulkan/vulkan.h>

namespace RHI
{
	class Device;

	class Platform
	{
	public:
		static const char* getInstanceExtension();
		static VkSurfaceKHR createSurface(const Device* device, void* native_window);
		static void destroySurface(const Device* device, VkSurfaceKHR surface);
	};
}