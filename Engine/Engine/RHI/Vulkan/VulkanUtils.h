#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace RHI
{
	class VulkanContext;

	class VulkanUtils
	{
	public:
		static bool CheckInstanceExtensions(const std::vector<const char*>& requiredExtensions, bool verbose = false);

		static bool CheckInstanceValidationLayers(const std::vector<const char*>& requiredLayers, bool verbose = false);

		static bool CheckPhysicalDeviceExtensions(VkPhysicalDevice physicalDevice,
			const std::vector<const char*>& requiredExtensions, bool verbose = false);

		static VkSampleCountFlagBits GetMaxUsableSampleCount(VkPhysicalDevice physicalDevice);

		static uint32_t GetGraphicsQueueFamily(VkPhysicalDevice physicalDevice);
	};
}