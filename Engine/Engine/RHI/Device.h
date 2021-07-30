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

	private:
		VkInstance instance{ VK_NULL_HANDLE };

		VkDevice device{ VK_NULL_HANDLE };
		VkPhysicalDevice physicalDevice{ VK_NULL_HANDLE };

		VkCommandPool commandPool{ VK_NULL_HANDLE };
		VkDescriptorPool descriptorPool{ VK_NULL_HANDLE };

		uint32_t graphicsQueueFamily{ 0xFFFF };
		VkQueue graphicsQueue { VK_NULL_HANDLE };

		VkSampleCountFlagBits maxMSAASamples { VK_SAMPLE_COUNT_1_BIT };
		VkDebugUtilsMessengerEXT debugMessenger { VK_NULL_HANDLE };

		VmaAllocator vram_allocator { VK_NULL_HANDLE };
	};
}