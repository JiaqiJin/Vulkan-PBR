#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <optional>

#include <glm/glm.hpp>

#include "VulkanRendererContext.h"

// Forward declaration
struct GLFWwindow;

namespace RHI
{
	class Renderer;
	class RenderScene;
	class SwapChain;

	struct UniformBufferObject
	{
		glm::mat4 world;
		glm::mat4 view;
		glm::mat4 proj;
		glm::vec3 cameraPosWS;
		float lerpUserValues{ 0.0f };
		float userMetalness{ 0.0f };
		float userRoughness{ 0.0f };
	};

	// Queu family indices
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily{ std::nullopt };
		std::optional<uint32_t> presentFamily{ std::nullopt };

		inline bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
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

		QueueFamilyIndices fetchQueueFamilyIndices(VkPhysicalDevice device) const;

		void initVulkan();
		void shutdownVulkan();

		void initVulkanSwapChain();
		void shutdownVulkanSwapChain();
		void recreateVulkanSwapChain();

		void initRenderScene();
		void shutdownRenderScene();

		void initRenderers();
		void shutdownRenderers();

		void initImGui();
		void shutdownImGui();

		void update();
		void render();
		void mainloop();


		static void onFramebufferResize(GLFWwindow* window, int width, int height);

	private:
		GLFWwindow* window{ nullptr };
		RHI::Renderer* renderer{ nullptr };
		RHI::RenderScene* scene{ nullptr };

		UniformBufferObject ubo;

		VulkanRendererContext context = {};

		VkInstance instance{ VK_NULL_HANDLE };
		VkPhysicalDevice physicalDevice{ VK_NULL_HANDLE };
		VkDevice device{ VK_NULL_HANDLE };
		VkSurfaceKHR surface{ VK_NULL_HANDLE };

		SwapChain* swapChain{ nullptr };

		VkQueue graphicsQueue{ VK_NULL_HANDLE };
		VkQueue presentQueue{ VK_NULL_HANDLE };

		VkCommandPool commandPool{ VK_NULL_HANDLE };

		VkDescriptorPool descriptorPool{ VK_NULL_HANDLE };

		bool windowResized{ false };
	};
}