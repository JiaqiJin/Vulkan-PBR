#include "Device.h"

#include "../Vendor/vma/vk_mem_alloc.h"

#include <stdexcept>

namespace RHI
{
	void Device::init(const char* applicationName, const char* engineName)
	{
		// Fill instance structures
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = applicationName;
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = engineName;
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkDeviceCreateInfo deviceCreateInfo = {};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		bool result = false;

		result = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device);
		if (result != VK_SUCCESS)
			throw std::runtime_error("Can't create logical device");

		VmaAllocatorCreateInfo allocatorInfo = {};
		allocatorInfo.physicalDevice = physicalDevice;
		allocatorInfo.device = device;
		allocatorInfo.instance = instance;
	}

	void Device::shutdown()
	{
		vmaDestroyAllocator(vram_allocator);
		vram_allocator = VK_NULL_HANDLE;

		vkDestroyDescriptorPool(device, descriptorPool, nullptr);
		descriptorPool = VK_NULL_HANDLE;

		vkDestroyCommandPool(device, commandPool, nullptr);
		commandPool = VK_NULL_HANDLE;

		vkDestroyDevice(device, nullptr);
		device = VK_NULL_HANDLE;

		vkDestroyInstance(instance, nullptr);
		instance = VK_NULL_HANDLE;

		graphicsQueueFamily = 0xFFFF;
		graphicsQueue = VK_NULL_HANDLE;

		maxMSAASamples = VK_SAMPLE_COUNT_1_BIT;
		physicalDevice = VK_NULL_HANDLE;
	}

	void Device::wait()
	{
		vkDeviceWaitIdle(device);
	}

}