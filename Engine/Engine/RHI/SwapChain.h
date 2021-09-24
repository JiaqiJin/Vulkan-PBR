#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include <vk_mem_alloc.h>

namespace RHI
{
	class VulkanContext;

	struct VulkanRenderFrame
	{
		VkDescriptorSet descriptorSet{ VK_NULL_HANDLE };

		VkFramebuffer frameBuffer{ VK_NULL_HANDLE };
		VkCommandBuffer commandBuffer{ VK_NULL_HANDLE };

		VkBuffer uniformBuffer{ VK_NULL_HANDLE };
		VmaAllocation uniformBufferMemory{ VK_NULL_HANDLE };
	};

	class SwapChain
	{
	public:
		SwapChain(const VulkanContext* context, VkDeviceSize uboSize);
		virtual ~SwapChain();

		void init(int width, int height);
		void reinit(int width, int height);
		void shutdown();

		bool acquire(void* ubo, VulkanRenderFrame& frame);
		bool present(const VulkanRenderFrame& frame);

		inline uint32_t getNumImages() const { return static_cast<uint32_t>(swapChainImages.size()); }
		inline VkExtent2D getExtent() const { return swapChainExtent; }
		inline VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }
		inline VkRenderPass getRenderPass() const { return renderPass; }
		inline VkRenderPass getNoClearRenderPass() const { return noClearRenderPass; }

	private:
		// Querying details of swap chain support
		struct SupportDetails
		{
			VkSurfaceCapabilitiesKHR capabilities; // Basic surface capabilities (min/max number of images in SwapChain, min/max w/h of images)
			std::vector<VkSurfaceFormatKHR> formats; // surface formats
			std::vector<VkPresentModeKHR> presentModes; // Available presentation modes
		};

		// Choosing the right settings for the swap chain
		struct Settings 
		{
			VkSurfaceFormatKHR format; // Surface format (color depth)
			VkPresentModeKHR presentMode; // Presentation mode (conditions for "swapping" images to the screen)
			VkExtent2D extent; // Swap extent (resolution of images in swap chain)
		};

		SupportDetails fetchSwapChainSupportDetails() const;
		Settings selectOptimalSwapChainSettings(const SupportDetails& details, int width, int height) const;
	private:
		void initTransient(int width, int height);
		void shutdownTransient();

		void initPersistent();
		void shutdownPersistent();

		void initFrames(VkDeviceSize uboSize);
		void shutdownFrames();

	private:
		const VulkanContext* context {nullptr};
		std::vector<VulkanRenderFrame> frames;
		VkDeviceSize uboSize;

		VkSwapchainKHR swapChain{ VK_NULL_HANDLE };

		std::vector<VkImage> swapChainImages; 
		std::vector<VkImageView> swapChainImageViews;

		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;

		VkImage colorImage{ VK_NULL_HANDLE };
		VkImageView colorImageView{ VK_NULL_HANDLE };
		VmaAllocation colorImageMemory{ VK_NULL_HANDLE };

		VkImage depthImage{ VK_NULL_HANDLE };
		VkImageView depthImageView{ VK_NULL_HANDLE };
		VmaAllocation depthImageMemory{ VK_NULL_HANDLE };

		VkFormat depthFormat;

		VkRenderPass renderPass{ VK_NULL_HANDLE };
		VkRenderPass noClearRenderPass{ VK_NULL_HANDLE };
		VkDescriptorSetLayout descriptorSetLayout{ VK_NULL_HANDLE };

		//
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