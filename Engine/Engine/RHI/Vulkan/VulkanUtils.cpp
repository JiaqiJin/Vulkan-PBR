#include "VulkanUtils.h"
#include "VulkanContext.h"
#include "VulkanContext.h"

#include <algorithm>
#include <stdexcept>

#include "Common/Logger.h"

namespace RHI
{
	bool VulkanUtils::CheckInstanceExtensions(const std::vector<const char*>& requiredExtensions, bool verbose)
	{
		uint32_t availableExtensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, availableExtensions.data());

		for (const char* requiredExtension : requiredExtensions)
		{
			bool supported = false;
			for (const VkExtensionProperties& availableExtension : availableExtensions)
			{
				if (strcmp(requiredExtension, availableExtension.extensionName) == 0)
				{
					supported = true;
					break;
				}
			}

			if (!supported)
			{
				if (verbose)
					K_ERROR("This extension is not supported on this physical device", requiredExtension);

				return false;
			}

			if (verbose)
				K_INFO("HAVE", requiredExtension);
		}

		return true;
	}

	bool VulkanUtils::CheckInstanceValidationLayers(const std::vector<const char*>& requiredLayers, bool verbose)
	{
		uint32_t availableLayerCount = 0;
		vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(availableLayerCount);
		vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data());

		for (const char* requiredLayer : requiredLayers)
		{
			bool supported = false;
			for (const VkLayerProperties& layer : availableLayers)
			{
				if (strcmp(requiredLayer, layer.layerName) == 0)
				{
					supported = true;
					break;
				}
			}

			if (!supported)
			{
				if (verbose)
					K_ERROR("This layer is not supported on this device", requiredLayer);

				return false;
			}

			if (verbose)
				K_INFO("HAVE", requiredLayer);
		}

		return true;
	}

	bool VulkanUtils::CheckPhysicalDeviceExtensions(VkPhysicalDevice physicalDevice,
		const std::vector<const char*>& requiredExtensions, bool verbose = false)
	{
		uint32_t availableDeviceExtensionCount = 0;
		vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &availableDeviceExtensionCount, nullptr);

		std::vector<VkExtensionProperties> availableDeviceExtensions(availableDeviceExtensionCount);
		vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &availableDeviceExtensionCount, availableDeviceExtensions.data());

		for (const char* requiredExtension : requiredExtensions)
		{
			bool supported = false;
			for (const VkExtensionProperties& availableDeviceExtension : availableDeviceExtensions)
			{
				if (strcmp(requiredExtension, availableDeviceExtension.extensionName) == 0)
				{
					supported = true;
					break;
				}
			}

			if (!supported)
			{
				if (verbose)
					K_ERROR("This extension is not supported on this physical device", requiredExtension);

				return false;
			}

			if (verbose)
				K_INFO("HAVE", requiredExtension);
		}

		return true;
	}

	VkSampleCountFlagBits VulkanUtils::GetMaxUsableSampleCount(VkPhysicalDevice physicalDevice)
	{
		VkPhysicalDeviceProperties physicalDeviceProperties;
		vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

		VkSampleCountFlags counts = std::min(
			physicalDeviceProperties.limits.framebufferColorSampleCounts,
			physicalDeviceProperties.limits.framebufferDepthSampleCounts
		);

		if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
		if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
		if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
		if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
		if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
		if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

		return VK_SAMPLE_COUNT_1_BIT;
	}

	uint32_t VulkanUtils::GetGraphicsQueueFamily(VkPhysicalDevice physicalDevice)
	{
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

		for (uint32_t i = 0; i < queueFamilyCount; i++) {
			const auto& queueFamily = queueFamilies[i];
			if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				return i;
		}

		return 0xFFFF;
	}
}