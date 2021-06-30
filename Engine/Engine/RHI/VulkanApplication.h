#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <optional>

#include "VulkanRendererContext.h"

// Forward declaration
struct GLFWwindow;

namespace RHI
{
	class Renderer;
	class RenderScene;

	// Queu family indices
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily{ std::nullopt };
		std::optional<uint32_t> presentFamily{ std::nullopt };

		inline bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
	};

	// Swap chain support
	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	struct SwapChainSettings
	{
		VkSurfaceFormatKHR format;
		VkPresentModeKHR presentMode;
		VkExtent2D extent;
	};

	class Application
	{
	public:
		void run();

	private:
		void initWindow();
		void shutdownWindow();

		bool checkRequiredValidationLayers(std::vector<const char*>& layers) const;
		bool checkRequiredExtensions(std::vector<const char*>& extensions) const;
		bool checkRequiredPhysicalDeviceExtensions(VkPhysicalDevice device, std::vector<const char*>& extensions) const;
		bool checkPhysicalDevice(VkPhysicalDevice device, VkSurfaceKHR surface) const;

		SwapChainSupportDetails fetchSwapChainSupportDetails(VkPhysicalDevice device, VkSurfaceKHR surface) const;
		QueueFamilyIndices fetchQueueFamilyIndices(VkPhysicalDevice device) const;

		SwapChainSettings selectOptimalSwapChainSettings(const SwapChainSupportDetails& details) const;
		VkFormat selectOptimalSupportedFormat(
			const std::vector<VkFormat>& candidates,
			VkImageTiling tiling,
			VkFormatFeatureFlags features) const;

		VkFormat selectOptimalDepthFormat() const;

		void initVulkan();
		void shutdownVulkan();

		void initVulkanSwapChain();
		void shutdownVulkanSwapChain();
		void recreateVulkanSwapChain();

		void initRenderer();
		void shutdownRenderer();

		void initRenderScene();
		void shutdownRenderScene();

		void initImGui();
		void shutdownImGui();

		void update();
		void mainloop();
		void render();

		static void onFramebufferResize(GLFWwindow* window, int width, int height);

	private:
		GLFWwindow* window{ nullptr };
		RHI::Renderer* renderer{ nullptr };
		RHI::RenderScene* scene{ nullptr };

		VulkanRendererContext context = {};

		VkInstance instance{ VK_NULL_HANDLE };
		VkPhysicalDevice physicalDevice{ VK_NULL_HANDLE };
		VkDevice device{ VK_NULL_HANDLE };
		VkSurfaceKHR surface{ VK_NULL_HANDLE };

		VkQueue graphicsQueue{ VK_NULL_HANDLE };
		VkQueue presentQueue{ VK_NULL_HANDLE };

		VkCommandPool commandPool{ VK_NULL_HANDLE };
		VkDebugUtilsMessengerEXT debugMessenger{ VK_NULL_HANDLE };

		VkSwapchainKHR swapChain{ VK_NULL_HANDLE };
		std::vector<VkImage> swapChainImages;
		std::vector<VkImageView> swapChainImageViews;

		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;

		VkImage colorImage{ VK_NULL_HANDLE };
		VkImageView colorImageView{ VK_NULL_HANDLE };
		VkDeviceMemory colorImageMemory{ VK_NULL_HANDLE };

		VkImage depthImage{ VK_NULL_HANDLE };
		VkImageView depthImageView{ VK_NULL_HANDLE };
		VkDeviceMemory depthImageMemory{ VK_NULL_HANDLE };

		VkFormat depthFormat;

		VkDescriptorPool descriptorPool{ VK_NULL_HANDLE };

		// Syncronizations objects
		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;
		size_t currentFrame{ 0 };

		bool framebufferResized{ false };
		enum
		{
			MAX_FRAMES_IN_FLIGHT = 2,
		};
	};
}