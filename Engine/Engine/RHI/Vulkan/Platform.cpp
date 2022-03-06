#define VK_USE_PLATFORM_WIN32_KHR
#include "Platform.h"
#include "VulkanContext.h"

#include "Common/Logger.h"

namespace RHI
{
	const char* Platform::GetInstanceExtension()
	{
		return "VK_KHR_win32_surface";
	}

	VkSurfaceKHR Platform::CreateSurface(const VulkanContext* context, void* native_window)
	{
		VkSurfaceKHR surface = VK_NULL_HANDLE;

		VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
		surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surfaceInfo.hwnd = reinterpret_cast<HWND>(native_window);
		surfaceInfo.hinstance = GetModuleHandle(nullptr);

		if (vkCreateWin32SurfaceKHR(context->getInstance(), &surfaceInfo, nullptr, &surface) != VK_SUCCESS)
			K_ERROR("Platform::createSurface(): vkCreateWin32SurfaceKHR failed");

		return surface;
	}

	void Platform::DestroySurface(const VulkanContext* context, VkSurfaceKHR surface)
	{
		vkDestroySurfaceKHR(context->getInstance(), surface, nullptr);
	}
}
