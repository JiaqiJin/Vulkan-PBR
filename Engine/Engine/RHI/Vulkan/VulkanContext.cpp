#define VK_USE_PLATFORM_WIN32_KHR
#include "VulkanContext.h"
#include "VulkanUtils.h"
#include "Platform.h"

#include <array>
#include <iostream>
#include <set>

#include "Common/Logger.h"

namespace RHI
{
	static std::vector<const char*> requiredInstanceExtensions = {
		VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
		VK_KHR_SURFACE_EXTENSION_NAME,
	};

	static std::vector<const char*> requiredPhysicalDeviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	};

	static std::vector<const char*> requiredValidationLayers = {
		"VK_LAYER_KHRONOS_validation",
	};


	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}


	void VulkanContext::Init(const char* applicationName, const char* engineName)
	{
		// Check required instance extensions
		requiredInstanceExtensions.push_back(Platform::GetInstanceExtension());
		if (!VulkanUtils::CheckInstanceExtensions(requiredInstanceExtensions, true))
			throw std::runtime_error("This device doesn't have required Vulkan extensions");

		// Check required instance validation layers
		if (!VulkanUtils::CheckInstanceValidationLayers(requiredValidationLayers, true))
			throw std::runtime_error("This device doesn't have required Vulkan validation layers");

		// Fill instance structures
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = applicationName;
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = engineName;
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo = {};
		debugMessengerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugMessengerInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugMessengerInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugMessengerInfo.pfnUserCallback = debugCallback;
		debugMessengerInfo.pUserData = nullptr;

		VkInstanceCreateInfo instanceInfo = {};
		instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceInfo.pApplicationInfo = &appInfo;
		instanceInfo.enabledExtensionCount = static_cast<uint32_t>(requiredInstanceExtensions.size());
		instanceInfo.ppEnabledExtensionNames = requiredInstanceExtensions.data();
		instanceInfo.pNext = &debugMessengerInfo;

#if SCAPES_VULKAN_USE_VALIDATION_LAYERS
		instanceInfo.enabledLayerCount = static_cast<uint32_t>(requiredValidationLayers.size());
		instanceInfo.ppEnabledLayerNames = requiredValidationLayers.data();
#endif

		// Create Vulkan instance
		VkResult result = vkCreateInstance(&instanceInfo, nullptr, &instance);
		if (result != VK_SUCCESS)
			throw std::runtime_error("Failed to create Vulkan instance");

		// Create Vulkan debug messenger
		result = vkCreateDebugUtilsMessengerEXT(instance, &debugMessengerInfo, nullptr, &debugMessenger);
		if (result != VK_SUCCESS)
			throw std::runtime_error("Can't create Vulkan debug messenger");

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
			int current_estimate = ExaminePhysicalDevice(device);
			if (current_estimate == -1)
				continue;

			if (estimate > current_estimate)
				continue;

			estimate = current_estimate;
			physicalDevice = device;
		}

		if (physicalDevice == VK_NULL_HANDLE)
			throw std::runtime_error("Failed to find a suitable GPU");

		// Create logical device
		graphicsQueueFamily = VulkanUtils::GetGraphicsQueueFamily(physicalDevice);
		const float queuePriority = 1.0f;

		VkDeviceQueueCreateInfo graphicsQueueInfo = {};
		graphicsQueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		graphicsQueueInfo.queueFamilyIndex = graphicsQueueFamily;
		graphicsQueueInfo.queueCount = 1;
		graphicsQueueInfo.pQueuePriorities = &queuePriority;

		VkPhysicalDeviceFeatures deviceFeatures = {};
		deviceFeatures.samplerAnisotropy = VK_TRUE;
		deviceFeatures.sampleRateShading = VK_TRUE;
		deviceFeatures.geometryShader = VK_TRUE;
		deviceFeatures.tessellationShader = VK_TRUE;

		VkDeviceCreateInfo deviceCreateInfo = {};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
		deviceCreateInfo.queueCreateInfoCount = 1;
		deviceCreateInfo.pQueueCreateInfos = &graphicsQueueInfo;
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(requiredPhysicalDeviceExtensions.size());
		deviceCreateInfo.ppEnabledExtensionNames = requiredPhysicalDeviceExtensions.data();
		deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(requiredValidationLayers.size());
		deviceCreateInfo.ppEnabledLayerNames = requiredValidationLayers.data();

		result = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device);
		if (result != VK_SUCCESS)
			throw std::runtime_error("Can't create logical device");

		// Get graphics queue
		vkGetDeviceQueue(device, graphicsQueueFamily, 0, &graphicsQueue);
		if (graphicsQueue == VK_NULL_HANDLE)
			throw std::runtime_error("Can't get graphics queue from logical device");

		// Create command pool
		VkCommandPoolCreateInfo commandPoolInfo = {};
		commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolInfo.queueFamilyIndex = graphicsQueueFamily;
		commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		if (vkCreateCommandPool(device, &commandPoolInfo, nullptr, &commandPool) != VK_SUCCESS)
			throw std::runtime_error("Can't create command pool");

		// Create descriptor pools
		std::array<VkDescriptorPoolSize, 2> descriptorPoolSizes = {};
		descriptorPoolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorPoolSizes[0].descriptorCount = MAX_UNIFORM_BUFFERS;
		descriptorPoolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorPoolSizes[1].descriptorCount = MAX_COMBINED_IMAGE_SAMPLERS;

		VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
		descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size());
		descriptorPoolInfo.pPoolSizes = descriptorPoolSizes.data();
		descriptorPoolInfo.maxSets = MAX_DESCRIPTOR_SETS;
		descriptorPoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

		if (vkCreateDescriptorPool(device, &descriptorPoolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
			throw std::runtime_error("Can't create descriptor pool");

		maxMSAASamples = VulkanUtils::GetMaxUsableSampleCount(physicalDevice);

		VmaAllocatorCreateInfo allocatorInfo = {};
		allocatorInfo.physicalDevice = physicalDevice;
		allocatorInfo.device = device;
		allocatorInfo.instance = instance;

		if (vmaCreateAllocator(&allocatorInfo, &vram_allocator) != VK_SUCCESS)
			throw std::runtime_error("Can't create VRAM allocator");
	}

	void VulkanContext::Shutdown()
	{
		vkDestroyDescriptorPool(device, descriptorPool, nullptr);
		descriptorPool = VK_NULL_HANDLE;

		vkDestroyCommandPool(device, commandPool, nullptr);
		commandPool = VK_NULL_HANDLE;

		vmaDestroyAllocator(vram_allocator);
		vram_allocator = VK_NULL_HANDLE;

		vkDestroyDevice(device, nullptr);
		device = VK_NULL_HANDLE;

		vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		debugMessenger = VK_NULL_HANDLE;

		vkDestroyInstance(instance, nullptr);
		instance = VK_NULL_HANDLE;

		graphicsQueueFamily = 0xFFFF;
		graphicsQueue = VK_NULL_HANDLE;

		maxMSAASamples = VK_SAMPLE_COUNT_1_BIT;
		physicalDevice = VK_NULL_HANDLE;
	}

	int VulkanContext::ExaminePhysicalDevice(VkPhysicalDevice physicalDevice) const
	{
		if (!VulkanUtils::CheckPhysicalDeviceExtensions(physicalDevice, requiredPhysicalDeviceExtensions))
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

		if (!physicalDeviceFeatures.geometryShader)
			return -1;

		if (!physicalDeviceFeatures.tessellationShader)
			return -1;

		return estimate;
	}
}