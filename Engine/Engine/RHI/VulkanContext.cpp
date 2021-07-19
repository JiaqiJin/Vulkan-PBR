#define VK_USE_PLATFORM_WIN32_KHR
#include "VulkanContext.h"
#include "VulkanUtils.h"

#include <array>
#include <iostream>
#include <set>

#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "../Common/Logger.h"

namespace RHI
{
	static int maxCombinedImageSamplers = 32;
	static int maxUniformBuffers = 32;

	static std::vector<const char*> requiredPhysicalDeviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	};

	static std::vector<const char*> requiredValidationLayers = {
		"VK_LAYER_KHRONOS_validation",
	};

	void VulkanContext::init(GLFWwindow* window, const char* appName, const char* engineName)
	{
		// Check required instance extensions
		uint32_t numGlfwExtensions = 0;
		const char** requiredGlfwExtensions = glfwGetRequiredInstanceExtensions(&numGlfwExtensions);

		std::vector<const char*> requiredInstanceExtensions(requiredGlfwExtensions, requiredGlfwExtensions + numGlfwExtensions);
		requiredInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		if (!checkInstanceExtensions(requiredInstanceExtensions, true))
			throw std::runtime_error("This device doesn't have required Vulkan extensions");

		// Check required instance validation layers
		if (!checkInstanceValidationLayers(requiredValidationLayers, true))
			throw std::runtime_error("This device doesn't have required Vulkan validation layers");

		// Fill instance structures
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = appName;
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = engineName;
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo instanceInfo = {};
		instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceInfo.pApplicationInfo = &appInfo;
		instanceInfo.enabledExtensionCount = static_cast<uint32_t>(requiredInstanceExtensions.size());
		instanceInfo.ppEnabledExtensionNames = requiredInstanceExtensions.data();
		instanceInfo.enabledLayerCount = static_cast<uint32_t>(requiredValidationLayers.size());
		instanceInfo.ppEnabledLayerNames = requiredValidationLayers.data();
		instanceInfo.pNext = nullptr;

		// Create Vulkan instance
		VkResult result = vkCreateInstance(&instanceInfo, nullptr, &instance);
		if (result != VK_SUCCESS)
			throw std::runtime_error("Failed to create Vulkan instance");

		// Create Vulkan win32 surface
		VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
		surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surfaceInfo.hwnd = glfwGetWin32Window(window);
		surfaceInfo.hinstance = GetModuleHandle(nullptr);

		result = vkCreateWin32SurfaceKHR(instance, &surfaceInfo, nullptr, &surface);
		if (result != VK_SUCCESS)
			throw std::runtime_error("Can't create Vulkan win32 surface KHR");

		// Enumerate physical devices
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		if (deviceCount == 0)
			throw std::runtime_error("Failed to find GPUs with Vulkan support");

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		// Pick the best physical device
		int estimate = -1;
		for (const auto& device : devices)
		{
			int currentEstimate = examinePhysicalDevice(device, surface);
			if (currentEstimate == -1)
				continue;

			if (estimate > currentEstimate)
				continue;

			estimate = currentEstimate;
			physicalDevice = device;
		}

		if (physicalDevice == VK_NULL_HANDLE)
			throw std::runtime_error("Failed to find a suitable GPU");

		// Create logical device
		QueueFamilyIndices indices = fetchQueueFamilyIndices(physicalDevice);

		const float queuePriority = 1.0f;

		std::vector<VkDeviceQueueCreateInfo> queuesInfo;
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		for (uint32_t queueFamilyIndex : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			info.queueFamilyIndex = queueFamilyIndex;
			info.queueCount = 1;
			info.pQueuePriorities = &queuePriority;
			queuesInfo.push_back(info);
		}

		VkPhysicalDeviceFeatures deviceFeatures = {};
		deviceFeatures.samplerAnisotropy = VK_TRUE;
		deviceFeatures.sampleRateShading = VK_TRUE;

		VkDeviceCreateInfo deviceCreateInfo = {};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queuesInfo.size());
		deviceCreateInfo.pQueueCreateInfos = queuesInfo.data();
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(requiredPhysicalDeviceExtensions.size());
		deviceCreateInfo.ppEnabledExtensionNames = requiredPhysicalDeviceExtensions.data();

		// next two parameters are ignored, but it's still good to pass layers for backward compatibility
		deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(requiredValidationLayers.size());
		deviceCreateInfo.ppEnabledLayerNames = requiredValidationLayers.data();

		result = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device);
		if (result != VK_SUCCESS)
			throw std::runtime_error("Can't create logical device");

		// Get logical device queues
		vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
		if (graphicsQueue == VK_NULL_HANDLE)
			throw std::runtime_error("Can't get graphics queue from logical device");

		vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
		if (presentQueue == VK_NULL_HANDLE)
			throw std::runtime_error("Can't get present queue from logical device");

		// Create command pool
		VkCommandPoolCreateInfo commandPoolInfo = {};
		commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolInfo.queueFamilyIndex = indices.graphicsFamily.value();
		commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		if (vkCreateCommandPool(device, &commandPoolInfo, nullptr, &commandPool) != VK_SUCCESS)
			throw std::runtime_error("Can't create command pool");

		// Create descriptor pools
		std::array<VkDescriptorPoolSize, 2> descriptorPoolSizes = {};
		descriptorPoolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorPoolSizes[0].descriptorCount = maxUniformBuffers;
		descriptorPoolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorPoolSizes[1].descriptorCount = maxCombinedImageSamplers;

		VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
		descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size());
		descriptorPoolInfo.pPoolSizes = descriptorPoolSizes.data();
		descriptorPoolInfo.maxSets = maxCombinedImageSamplers + maxUniformBuffers;
		descriptorPoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

		if (vkCreateDescriptorPool(device, &descriptorPoolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
			throw std::runtime_error("Can't create descriptor pool");

		maxMSAASamples = VulkanUtils::getMaxUsableSampleCount(physicalDevice);

	}

	void VulkanContext::shutdown()
	{
		vkDestroyDescriptorPool(device, descriptorPool, nullptr);
		descriptorPool = VK_NULL_HANDLE;

		vkDestroyCommandPool(device, commandPool, nullptr);
		commandPool = VK_NULL_HANDLE;

		vkDestroyDevice(device, nullptr);
		device = VK_NULL_HANDLE;

		vkDestroySurfaceKHR(instance, surface, nullptr);
		surface = VK_NULL_HANDLE;

		vkDestroyInstance(instance, nullptr);
		instance = VK_NULL_HANDLE;

		graphicsQueueFamily = 0;
		presentQueueFamily = 0;

		maxMSAASamples = VK_SAMPLE_COUNT_1_BIT;
		physicalDevice = VK_NULL_HANDLE;
	}

	void VulkanContext::wait()
	{
		vkDeviceWaitIdle(device);
	}

	// ----------------------------- Helper functions ---------------------------
	int VulkanContext::examinePhysicalDevice(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) const
	{
		QueueFamilyIndices indices = fetchQueueFamilyIndices(physicalDevice);
		if (!indices.isComplete())
			return -1;

		if (!VulkanUtils::checkPhysicalDeviceExtensions(physicalDevice, requiredPhysicalDeviceExtensions))
			return -1;

		uint32_t formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

		uint32_t presentModeCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

		if (formatCount == 0 || presentModeCount == 0)
			return -1;

		VkPhysicalDeviceProperties physicalDeviceProperties;
		vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

		VkPhysicalDeviceFeatures physicalDeviceFeatures;
		vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);

		int estimate = 0;

		switch (physicalDeviceProperties.deviceType)
		{
		case VK_PHYSICAL_DEVICE_TYPE_OTHER:
		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
		case VK_PHYSICAL_DEVICE_TYPE_CPU: estimate = 10; break;

		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: estimate = 100; break;
		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: estimate = 1000; break;
		}

		if (physicalDeviceFeatures.geometryShader)
			estimate++;

		if (physicalDeviceFeatures.tessellationShader)
			estimate++;

		return estimate;
	}

	VulkanContext::QueueFamilyIndices VulkanContext::fetchQueueFamilyIndices(VkPhysicalDevice device) const
	{
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		QueueFamilyIndices indices = {};

		for (uint32_t i = 0; i < queueFamilyCount; i++) {
			const auto& queueFamily = queueFamilies[i];
			if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				indices.graphicsFamily = std::make_optional(i);

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
			if (queueFamily.queueCount > 0 && presentSupport)
				indices.presentFamily = std::make_optional(i);

			if (indices.isComplete())
				break;
		}

		return indices;
	}

	bool VulkanContext::checkInstanceValidationLayers(const std::vector<const char*>& requiredLayers, bool verbose)
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
					K_ERROR(" This layer is not supported : {0}", requiredLayer);

				return false;
			}

			if (verbose)
				K_INFO("Have {0}", requiredLayer);
		}

		return true;
	}

	bool VulkanContext::checkInstanceExtensions(const std::vector<const char*>& requiredExtensions, bool verbose)
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
					K_ERROR(" This extension is not supported : {0}", requiredExtension);

				return false;
			}

			if (verbose)
				K_INFO("Have {0}", requiredExtension);
		}

		return true;
	}
}