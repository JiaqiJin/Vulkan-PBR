#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "VulkanRendererContext.h"

namespace RHI
{
	struct UniformBufferObject;

	class SwapChain
	{
	public:
		SwapChain(const VulkanRendererContext& context, VkDeviceSize uboSize);
		virtual ~SwapChain();

		void init(int width, int height);
		void reinit(int width, int height);
		void shutdown();

		bool acquire(const UniformBufferObject& ubo);
		bool present();

		inline uint32_t getNumImages() const { return static_cast<uint32_t>(swapChainImages.size()); }
		inline VkExtent2D getExtent() const { return swapChainExtent; }
		inline VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }
		inline VkRenderPass getRenderPass() const { return renderPass; }
		inline VkRenderPass getNoClearRenderPass() const { return noClearRenderPass; }
		inline uint32_t getImageIndex() const { return imageIndex; }

		inline VkFormat getSwapChainImageFormat() const { return swapChainImageFormat; }
		inline VkFormat getDepthFormat() const { return depthFormat; }
		inline std::vector<VkImageView> getSwapChainImageViews() const { return swapChainImageViews; }
		inline VkImageView getDepthImageView() const { return depthImageView; }
		inline VkImageView getColorImageView() const { return colorImageView; }

	private:
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

		SwapChainSupportDetails fetchSwapChainSupportDetails() const;
		SwapChainSettings selectOptimalSwapChainSettings(const SwapChainSupportDetails& details, int width, int height) const;
	private:
		void initTransient(int width, int height);
		void shutdownTransient();

		void initPersistent();
		void shutdownPersistent();

		void initFrames(VkDeviceSize uboSize);
		void shutdownFrames();

	private:
		VulkanRendererContext context;
		VkDeviceSize uboSize;

		VkSwapchainKHR swapChain{ VK_NULL_HANDLE };

		std::vector<VkImage> swapChainImages;
		std::vector<VkImageView> swapChainImageViews;

		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;

		// Color image
		VkImage colorImage{ VK_NULL_HANDLE };
		VkImageView colorImageView{ VK_NULL_HANDLE };
		VkDeviceMemory colorImageMemory{ VK_NULL_HANDLE };

		// Depth image
		VkImage depthImage{ VK_NULL_HANDLE };
		VkImageView depthImageView{ VK_NULL_HANDLE };
		VkDeviceMemory depthImageMemory{ VK_NULL_HANDLE };

		VkFormat depthFormat;

		// Render pass
		VkRenderPass renderPass{ VK_NULL_HANDLE };
		VkRenderPass noClearRenderPass{ VK_NULL_HANDLE };
		VkDescriptorSetLayout descriptorSetLayout{ VK_NULL_HANDLE };

		// Semaphore
		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;

		uint32_t imageIndex{ 0 };
		uint32_t currentFrame{ 0 };

		bool framebufferResized{ false };
		enum
		{
			MAX_FRAMES_IN_FLIGHT = 2,
		};
	};
}