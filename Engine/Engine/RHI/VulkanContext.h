#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <optional>

#include <vk_mem_alloc.h>

struct GLFWwindow;

namespace RHI
{
	class VulkanContext
	{
	public:
		void init(GLFWwindow* window, const char* appName, const char* engineName);
		void shutdown();
		void wait();

		inline VkInstance getInstance() const { return instance; }
		inline VkSurfaceKHR getSurface() const { return surface; }
		inline VkDevice getDevice() const { return device; }
		inline VkPhysicalDevice getPhysicalDevice() const { return physicalDevice; }
		inline VkCommandPool getCommandPool() const { return commandPool; }
		inline VkDescriptorPool getDescriptorPool() const { return descriptorPool; }
		inline uint32_t getGraphicsQueueFamily() const { return graphicsQueueFamily; }
		inline uint32_t getPresentQueueFamily() const { return presentQueueFamily; }
		inline VkQueue getGraphicsQueue() const { return graphicsQueue; }
		inline VkQueue getPresentQueue() const { return presentQueue; }
		inline VkSampleCountFlagBits getMaxMSAASamples() const { return maxMSAASamples; }
		inline VmaAllocator GetAllocatorHandle() const { return m_allocator; }

	private:
		// Check which queue families are supported by the device and which one of these supports the commands
		struct QueueFamilyIndices
		{
			std::optional<uint32_t> graphicsFamily{ std::nullopt };
			std::optional<uint32_t> presentFamily{ std::nullopt };

			inline bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
		};

		int examinePhysicalDevice(VkPhysicalDevice device, VkSurfaceKHR surface) const;
		QueueFamilyIndices fetchQueueFamilyIndices(VkPhysicalDevice device) const;
		bool checkInstanceValidationLayers(const std::vector<const char*>& requiredLayers, bool verbose = false);
		bool checkInstanceExtensions(const std::vector<const char*>& requiredExtensions, bool verbose = false);

	private:
		VkResult create_allocator();

	private:
		VkInstance instance{ VK_NULL_HANDLE };
		VkSurfaceKHR surface{ VK_NULL_HANDLE };

		VkDevice device{ VK_NULL_HANDLE };
		VkPhysicalDevice physicalDevice{ VK_NULL_HANDLE };

		VkCommandPool commandPool{ VK_NULL_HANDLE };
		VkDescriptorPool descriptorPool{ VK_NULL_HANDLE };

		uint32_t graphicsQueueFamily{ 0 };
		uint32_t presentQueueFamily{ 0 };

		VkQueue graphicsQueue{ VK_NULL_HANDLE };
		VkQueue presentQueue{ VK_NULL_HANDLE };

		VkSampleCountFlagBits maxMSAASamples{ VK_SAMPLE_COUNT_1_BIT };

		// Vma
		VmaAllocator m_allocator{ VK_NULL_HANDLE };
	};
}