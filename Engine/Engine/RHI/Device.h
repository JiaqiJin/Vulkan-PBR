#pragma once

#include <vulkan/vulkan.h>
#include "../Vendor/vma/vk_mem_alloc.h"

#include <optional>

namespace RHI
{
	class Device
	{
	public:
		void init(const char* applicationName, const char* engineName);
		void shutdown();
		void wait();

		const char* getInstanceExtension();
		VkSurfaceKHR createSurface(void* native_window);
		void destroySurface(VkSurfaceKHR surface);

	public:
		// Getters
		inline VkInstance getInstance() const { return instance; }
		inline VkDevice getDevice() const { return device; }
		inline VkPhysicalDevice getPhysicalDevice() const { return physicalDevice; }
		inline VkCommandPool getCommandPool() const { return commandPool; }
		inline VkDescriptorPool getDescriptorPool() const { return descriptorPool; }
		inline uint32_t getGraphicsQueueFamily() const { return graphicsQueueFamily; }
		inline VkQueue getGraphicsQueue() const { return graphicsQueue; }
		inline VkSampleCountFlagBits getMaxSampleCount() const { return maxMSAASamples; }
		inline VmaAllocator getVRAMAllocator() const { return vram_allocator; }

	private:
		int examinePhysicalDevice(VkPhysicalDevice physicalDevice) const;

	private:
		VkInstance instance{ VK_NULL_HANDLE };

		VkDevice device{ VK_NULL_HANDLE };
		VkPhysicalDevice physicalDevice{ VK_NULL_HANDLE };

		VkCommandPool commandPool{ VK_NULL_HANDLE };
		VkDescriptorPool descriptorPool{ VK_NULL_HANDLE };

		uint32_t graphicsQueueFamily{ 0xFFFF };
		VkQueue graphicsQueue { VK_NULL_HANDLE };

		VkSampleCountFlagBits maxMSAASamples { VK_SAMPLE_COUNT_1_BIT };

		VmaAllocator vram_allocator { VK_NULL_HANDLE };
	};
}