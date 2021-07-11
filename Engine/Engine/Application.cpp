#define VK_USE_PLATFORM_WIN32_KHR
#include "Application.h"
#include "RHI/Renderer.h"
#include "RHI/VulkanUtils.h"
#include "RHI/SwapChain.h"
#include "RHI/VulkanContext.h"

#include "Common/RenderScene.h"

#include "Vendor/imgui/imgui.h"
#include "Vendor/imgui/imgui_impl_glfw.h"

#include "GUI/ImGuiRenderer.h"

#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <array>
#include <iostream>
#include <set>
#include <limits>

using namespace RHI;

static int maxCombinedImageSamplers = 32;
static int maxUniformBuffers = 32;

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
	initImGui();
	initVulkan();
	initVulkanSwapChain();
	initRenderScene();
	initRenderers();
	mainloop();
	shutdownRenderers();
	shutdownRenderScene();
	shutdownVulkanSwapChain();
	shutdownVulkan();
	shutdownImGui();
	shutdownWindow();
}

void Application::update()
{
	renderer->update(&ubo, scene);
	imguiRenderer->update(&ubo, scene);
}


void Application::render()
{
	VulkanRenderFrame frame;
	if (!swapChain->acquire(ubo, frame))
	{
		recreateVulkanSwapChain();
		return;
	}

	renderer->render(&ubo, scene, frame);
	imguiRenderer->render(&ubo, scene, frame);

	if (!swapChain->present(frame) || windowResized)
	{
		windowResized = false;
		recreateVulkanSwapChain();
	}
}

void Application::mainloop()
{
	if (!window)
		return;

	while (!glfwWindowShouldClose(window))
	{
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		update();

		ImGui::Render();

		render();
		glfwPollEvents();
	}
	//context->wait();
	vkDeviceWaitIdle(device);
}

void Application::initWindow()
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window = glfwCreateWindow(1024, 768, "Vulkan", nullptr, nullptr);

	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, &Application::onFramebufferResize);
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
	app->windowResized = true;
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

	VkInstanceCreateInfo instanceInfo = {};
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pApplicationInfo = &appInfo;
	instanceInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	instanceInfo.ppEnabledExtensionNames = extensions.data();
	instanceInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
	instanceInfo.ppEnabledLayerNames = layers.data();
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
	deviceFeatures.sampleRateShading = VK_TRUE;

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

	context.instance = instance;
	context.surface = surface;
	context.device = device;
	context.physicalDevice = physicalDevice;
	context.commandPool = commandPool;
	context.descriptorPool = descriptorPool;
	context.graphicsQueueFamily = indices.graphicsFamily.value();
	context.presentQueueFamily = indices.presentFamily.value();
	context.graphicsQueue = graphicsQueue;
	context.presentQueue = presentQueue;
	context.msaaSamples = VulkanUtils::getMaxUsableSampleCount(context);
}

void Application::shutdownVulkan()
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
}


void Application::initVulkanSwapChain()
{
	VkDeviceSize ubosize = sizeof(UniformBufferObject);
	if (!swapChain)
		swapChain = new SwapChain(context, ubosize);

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	swapChain->init(width, height);
}

void Application::shutdownVulkanSwapChain()
{
	delete swapChain;
	swapChain = nullptr;
}

void Application::recreateVulkanSwapChain()
{
	int width = 0, height = 0;
	while (width == 0 || height == 0)
	{
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
	}
	vkDeviceWaitIdle(device);

	glfwGetWindowSize(window, &width, &height);
	swapChain->reinit(width, height);
	renderer->resize(swapChain);
	imguiRenderer->resize(swapChain);
}

void Application::initRenderScene()
{
	scene = new RenderScene(context);
	scene->init();
}

void Application::shutdownRenderScene()
{
	scene->shutdown();

	delete scene;
	scene = nullptr;
}

void Application::initRenderers()
{
	renderer = new Renderer(context, swapChain->getExtent(), swapChain->getDescriptorSetLayout(), swapChain->getRenderPass());
	renderer->init(&ubo, scene);

	imguiRenderer = new ImGuiRenderer(context, swapChain->getExtent(), swapChain->getNoClearRenderPass());
	imguiRenderer->init(&ubo, scene, swapChain);
}

void Application::shutdownRenderers()
{
	delete renderer;
	renderer = nullptr;

	delete imguiRenderer;
	imguiRenderer = nullptr;
}

void Application::initImGui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui::StyleColorsDark();

	// TODO: use own GLFW callbacks
	ImGui_ImplGlfw_InitForVulkan(window, true);
}

void Application::shutdownImGui()
{
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

// -------------------- Helper Functions ---------------------------

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

	/* TODO: swap chain
	SwapChainSupportDetails details = fetchSwapChainSupportDetails(device, surface);
	if (details.formats.empty() || details.presentModes.empty())
		return false;
	/**/

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