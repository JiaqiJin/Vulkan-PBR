#define VK_USE_PLATFORM_WIN32_KHR
#include "VulkanApplication.h"
#include "VulkanRenderer.h"
#include "VulkanUtils.h"

#include "RenderScene.h"

#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <array>
#include <iostream>
#include <set>
#include <limits>

static std::string vertex_shader_path = "Assert/Shader/shader.vert";
static std::string fragment_shader_path = "Assert/Shader/shader.frag";
static std::string albedoTexturePath = "Assert/Texture/Default_albedo.jpg";
static std::string normalTexturePath = "Assert/Texture/Default_normal.jpg";
static std::string aoTexturePath = "Assert/Texture/Default_AO.jpg";
static std::string shadingTexturePath = "Assert/Texture/Default_metalRoughness.jpg";
static std::string emissionTexturePath = "Assert/Texture/Default_emissive.jpg";
static std::string model_path = "Assert/Model/DamagedHelmet.fbx";

using namespace RHI;

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


void Application::run()
{
	initWindow();

	mainloop();
}

void Application::initWindow()
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window = glfwCreateWindow(1024, 768, "Vulkan", nullptr, nullptr);

	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, &Application::onFramebufferResize);
}

void Application::mainloop()
{
	if (!window)
		return;

	while (!glfwWindowShouldClose(window))
	{
		render();
		glfwPollEvents();
	}

	vkDeviceWaitIdle(device);
}


void Application::shutdownWindow()
{
	glfwDestroyWindow(window);
	window = nullptr;
}

void Application::onFramebufferResize(GLFWwindow* window, int width, int height)
{
	Application* app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
	assert(app != nullptr);
	app->framebufferResized = true;
}

// --------------------- Vulkan Object Initializations ---------------------------------

void Application::render()
{
	vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());


}

void Application::initVulkan()
{
	// Check required extensions & layers
	std::vector<const char*> extensions;
	if (!checkRequiredExtensions(extensions))
		throw std::runtime_error("This device doesn't have required Vulkan extensions");

	std::vector<const char*> layers;
	if (!checkRequiredValidationLayers(layers))
		throw std::runtime_error("This device doesn't have required Vulkan validation layers");

	// Fill instance structures
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "PBR Sandbox";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
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
	instanceInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	instanceInfo.ppEnabledExtensionNames = extensions.data();
	instanceInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
	instanceInfo.ppEnabledLayerNames = layers.data();
	//instanceInfo.pNext = &debugMessengerInfo;

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

	// TODO: pick the best physical device
	for (const auto& device : devices)
	{
		if (checkPhysicalDevice(device, surface))
		{
			physicalDevice = device;
			break;
		}
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

	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queuesInfo.size());
	deviceCreateInfo.pQueueCreateInfos = queuesInfo.data();
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(requiredPhysicalDeviceExtensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = requiredPhysicalDeviceExtensions.data();

	// next two parameters are ignored, but it's still good to pass layers for backward compatibility
	deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
	deviceCreateInfo.ppEnabledLayerNames = layers.data();

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
	commandPoolInfo.flags = 0; // Optional

	if (vkCreateCommandPool(device, &commandPoolInfo, nullptr, &commandPool) != VK_SUCCESS)
		throw std::runtime_error("Can't create command pool");

	// Create sync objects
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		result = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]);
		if (result != VK_SUCCESS)
			throw std::runtime_error("Can't create 'image available' semaphore");

		result = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]);
		if (result != VK_SUCCESS)
			throw std::runtime_error("Can't create 'render finished' semaphore");

		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		result = vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]);
		if (result != VK_SUCCESS)
			throw std::runtime_error("Can't create in flight frame fence");
	}

	context.device = device;
	context.physicalDevice = physicalDevice;
	context.commandPool = commandPool;
	context.graphicsQueue = graphicsQueue;
	context.presentQueue = presentQueue;
	context.msaaSamples = VulkanUtils::getMaxUsableSampleCount(context);
}

void Application::shutdownVulkan()
{
	vkDestroyCommandPool(device, commandPool, nullptr);
	commandPool = VK_NULL_HANDLE;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(device, inFlightFences[i], nullptr);
	}

	renderFinishedSemaphores.clear();
	imageAvailableSemaphores.clear();
	inFlightFences.clear();

	vkDestroyDevice(device, nullptr);
	device = VK_NULL_HANDLE;

	vkDestroySurfaceKHR(instance, surface, nullptr);
	surface = VK_NULL_HANDLE;

	vkDestroyInstance(instance, nullptr);
	instance = VK_NULL_HANDLE;
}

void Application::initRenderer()
{

}

void Application::shutdownRenderer()
{
	
}

// ----------------------------- Helper Functions ---------------------------------------
bool Application::checkRequiredValidationLayers(std::vector<const char*>& layers) const
{
	uint32_t vulkanLayerCount = 0;
	vkEnumerateInstanceLayerProperties(&vulkanLayerCount, nullptr);
	std::vector<VkLayerProperties> vulkanLayers(vulkanLayerCount);
	vkEnumerateInstanceLayerProperties(&vulkanLayerCount, vulkanLayers.data());

	layers.clear();
	for (const auto& requiredLayer : requiredValidationLayers)
	{
		bool supported = false;
		for (const auto& vulkanLayer : vulkanLayers)
		{
			if (strcmp(requiredLayer, vulkanLayer.layerName) == 0)
			{
				supported = true;
				break;
			}
		}

		if (!supported)
		{
			std::cerr << requiredLayer << " is not supported on this device" << std::endl;
			return false;
		}

		std::cout << "Have " << requiredLayer << std::endl;
		layers.push_back(requiredLayer);
	}

	return true;
}

bool Application::checkRequiredExtensions(std::vector<const char*>& extensions) const
{
	uint32_t vulkanExtensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &vulkanExtensionCount, nullptr);
	std::vector<VkExtensionProperties> vulkanExtensions(vulkanExtensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &vulkanExtensionCount, vulkanExtensions.data());

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> requiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	extensions.clear();
	for (const auto& requiredExtension : requiredExtensions)
	{
		bool supported = false;
		for (const auto& vulkanExtension : vulkanExtensions)
		{
			if (strcmp(requiredExtension, vulkanExtension.extensionName) == 0)
			{
				supported = true;
				break;
			}
		}

		if (!supported)
		{
			std::cerr << requiredExtension << " is not supported on this device" << std::endl;
			return false;
		}

		std::cout << "Have " << requiredExtension << std::endl;
		extensions.push_back(requiredExtension);
	}

	return true;
}

bool Application::checkRequiredPhysicalDeviceExtensions(VkPhysicalDevice device, std::vector<const char*>& extensions) const
{
	uint32_t deviceExtensionCount = 0;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &deviceExtensionCount, nullptr);
	std::vector<VkExtensionProperties> deviceExtensions(deviceExtensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &deviceExtensionCount, deviceExtensions.data());

	extensions.clear();
	for (const char* requiredExtension : requiredPhysicalDeviceExtensions)
	{
		bool supported = false;
		for (const auto& deviceExtension : deviceExtensions)
		{
			if (strcmp(requiredExtension, deviceExtension.extensionName) == 0)
			{
				supported = true;
				break;
			}
		}

		if (!supported)
		{
			std::cerr << requiredExtension << " is not supported on this physical device" << std::endl;
			return false;
		}

		std::cout << "Have " << requiredExtension << std::endl;
		extensions.push_back(requiredExtension);
	}

	return true;
}

bool Application::checkPhysicalDevice(VkPhysicalDevice device, VkSurfaceKHR surface) const
{
	QueueFamilyIndices indices = fetchQueueFamilyIndices(device);
	if (!indices.isComplete())
		return false;

	std::vector<const char*> deviceExtensions;
	if (!checkRequiredPhysicalDeviceExtensions(device, deviceExtensions))
		return false;

	SwapChainSupportDetails details = fetchSwapChainSupportDetails(device, surface);
	if (details.formats.empty() || details.presentModes.empty())
		return false;

	// TODO: These checks are for testing purposes only, remove later
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);

	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	if (!deviceFeatures.geometryShader)
		return false;

	return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
}

QueueFamilyIndices Application::fetchQueueFamilyIndices(VkPhysicalDevice device) const
{
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	QueueFamilyIndices indices = {};

	for (int i = 0; i < queueFamilies.size(); i++) {
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

SwapChainSupportDetails Application::fetchSwapChainSupportDetails(VkPhysicalDevice device, VkSurfaceKHR surface) const
{
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	uint32_t formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

	if (formatCount > 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

	if (presentModeCount > 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

SwapChainSettings Application::selectOptimalSwapChainSettings(const SwapChainSupportDetails& details) const
{
	assert(!details.formats.empty());
	assert(!details.presentModes.empty());

	SwapChainSettings settings;

	// Select the best format if the surface has no preferred format
	if (details.formats.size() == 1 && details.formats[0].format == VK_FORMAT_UNDEFINED)
	{
		settings.format = { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}
	// Otherwise, select one of the available formats
	else
	{
		settings.format = details.formats[0];
		for (const auto& format : details.formats)
		{
			if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				settings.format = format;
				break;
			}
		}
	}

	// Select the best present mode
	settings.presentMode = VK_PRESENT_MODE_FIFO_KHR;
	for (const auto& presentMode : details.presentModes)
	{
		// Some drivers currently don't properly support FIFO present mode,
		// so we should prefer IMMEDIATE mode if MAILBOX mode is not available
		if (presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
			settings.presentMode = presentMode;

		if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			settings.presentMode = presentMode;
			break;
		}
	}

	// Select current swap extent if window manager doesn't allow to set custom extent
	if (details.capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		settings.extent = details.capabilities.currentExtent;
	}
	// Otherwise, manually set extent to match the min/max extent bounds
	else
	{
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		const VkSurfaceCapabilitiesKHR& capabilities = details.capabilities;

		settings.extent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		settings.extent.width = std::max(
			capabilities.minImageExtent.width,
			std::min(settings.extent.width, capabilities.maxImageExtent.width)
		);
		settings.extent.height = std::max(
			capabilities.minImageExtent.height,
			std::min(settings.extent.height, capabilities.maxImageExtent.height)
		);
	}

	return settings;
}

VkFormat Application::selectOptimalSupportedFormat(
	const std::vector<VkFormat>& candidates,
	VkImageTiling tiling,
	VkFormatFeatureFlags features
) const
{
	for (VkFormat format : candidates)
	{
		VkFormatProperties properties;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &properties);

		if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features)
			return format;

		if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features)
			return format;
	}

	throw std::runtime_error("Can't select optimal supported format");
}

VkFormat Application::selectOptimalDepthFormat() const
{
	return selectOptimalSupportedFormat(
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}
