#define VK_USE_PLATFORM_WIN32_KHR
#include "Platform.h"
#include "Device.h"

#include "../Common/Logger.h"

namespace RHI
{
	const char* Platform::getInstanceExtension()
	{
		return "VK_KHR_win32_surface";
	}

	VkSurfaceKHR Platform::createSurface(const Device* device, void* native_window)
	{
		VkSurfaceKHR surface = VK_NULL_HANDLE;

		VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
		surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surfaceInfo.hwnd = reinterpret_cast<HWND>(native_window);
		surfaceInfo.hinstance = GetModuleHandle(nullptr);

		if (vkCreateWin32SurfaceKHR(device->getInstance(), &surfaceInfo, nullptr, &surface) != VK_SUCCESS)
			K_ERROR("Platform::createSurface(): vkCreateWin32SurfaceKHR failed");
		return surface;
	}

	void Platform::destroySurface(const Device* device, VkSurfaceKHR surface)
	{
		vkDestroySurfaceKHR(device->getInstance(), surface, nullptr);
	}
}