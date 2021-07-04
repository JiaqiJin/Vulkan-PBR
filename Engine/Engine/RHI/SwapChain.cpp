#include "SwapChain.h"
#include "VulkanApplication.h"
#include "VulkanUtils.h"

#include "VulkanDescriptorSetLayout.h"
#include "VulkanRenderPass.h"

#include <array>
#include <algorithm>
#include <cassert>
#include <stdexcept>

namespace RHI
{
	SwapChain::SwapChain(const VulkanRendererContext& context, VkDeviceSize uboSize)
		: context(context), uboSize(uboSize)
	{

	}

	SwapChain::~SwapChain()
	{
		shutdown();
	}

	void SwapChain::init(int width, int height)
	{
		initTransient(width, height);
		initPersistent();
		initFrames(uboSize);
	}

	void SwapChain::reinit(int width, int height)
	{
		shutdownTransient();
		shutdownFrames();

		initTransient(width, height);
		initFrames(uboSize);
	}

	void SwapChain::shutdown()
	{
		shutdownTransient();
		shutdownFrames();
		shutdownPersistent();
	}

	void SwapChain::initTransient(int width, int height)
	{
		// Create swap chain
		SwapChain::SwapChainSupportDetails details = fetchSwapChainSupportDetails();
		SwapChain::SwapChainSettings settings = selectOptimalSwapChainSettings(details, width, height);

		// Simply sticking to this minimum means that we may sometimes have to wait
		// on the driver to complete internal operations before we can acquire another image to render to.
		// Therefore it is recommended to request at least one more image than the minimum
		uint32_t imageCount = details.capabilities.minImageCount + 1;

		// We should also make sure to not exceed the maximum number of images while doing this,
		// where 0 is a special value that means that there is no maximum
		if (details.capabilities.maxImageCount > 0)
			imageCount = std::min(imageCount, details.capabilities.maxImageCount);

		VkSwapchainCreateInfoKHR swapChainInfo = {};
		swapChainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapChainInfo.surface = context.surface;
		swapChainInfo.minImageCount = imageCount;
		swapChainInfo.imageFormat = settings.format.format;
		swapChainInfo.imageColorSpace = settings.format.colorSpace;
		swapChainInfo.imageExtent = settings.extent;
		swapChainInfo.imageArrayLayers = 1;
		swapChainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		if (context.graphicsQueueFamily != context.presentQueueFamily)
		{
			uint32_t queueFamilies[] = { context.graphicsQueueFamily, context.presentQueueFamily };
			swapChainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			swapChainInfo.queueFamilyIndexCount = 2;
			swapChainInfo.pQueueFamilyIndices = queueFamilies;
		}
		else
		{
			swapChainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			swapChainInfo.queueFamilyIndexCount = 0;
			swapChainInfo.pQueueFamilyIndices = nullptr;
		}

		swapChainInfo.preTransform = details.capabilities.currentTransform;
		swapChainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapChainInfo.presentMode = settings.presentMode;
		swapChainInfo.clipped = VK_TRUE;
		swapChainInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(context.device, &swapChainInfo, nullptr, &swapChain) != VK_SUCCESS)
			throw std::runtime_error("Can't create swapchain");

		uint32_t swapChainImageCount = 0;
		vkGetSwapchainImagesKHR(context.device, swapChain, &swapChainImageCount, nullptr);
		assert(swapChainImageCount != 0);

		swapChainImages.resize(swapChainImageCount);
		vkGetSwapchainImagesKHR(context.device, swapChain, &swapChainImageCount, swapChainImages.data());

		swapChainImageFormat = settings.format.format;
		swapChainExtent = settings.extent;

		// Create swap chain image views
		swapChainImageViews.resize(swapChainImageCount);
		for (size_t i = 0; i < swapChainImageViews.size(); i++)
			swapChainImageViews[i] = VulkanUtils::createImageView(
				context,
				swapChainImages[i],
				swapChainImageFormat,
				VK_IMAGE_ASPECT_COLOR_BIT,
				VK_IMAGE_VIEW_TYPE_2D
			);

		// Create color buffer & image view
		VulkanUtils::createImage2D(
			context,
			swapChainExtent.width,
			swapChainExtent.height,
			1,
			context.msaaSamples,
			swapChainImageFormat,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			colorImage,
			colorImageMemory
		);

		colorImageView = VulkanUtils::createImageView(
			context,
			colorImage,
			swapChainImageFormat,
			VK_IMAGE_ASPECT_COLOR_BIT,
			VK_IMAGE_VIEW_TYPE_2D
		);

		VulkanUtils::transitionImageLayout(
			context,
			colorImage,
			swapChainImageFormat,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		);

		// Create depth buffer & image view
		depthFormat = VulkanUtils::selectOptimalDepthFormat(context);

		VulkanUtils::createImage2D(
			context,
			swapChainExtent.width,
			swapChainExtent.height,
			1,
			context.msaaSamples,
			depthFormat,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			depthImage,
			depthImageMemory
		);

		depthImageView = VulkanUtils::createImageView(
			context,
			depthImage,
			depthFormat,
			VK_IMAGE_ASPECT_DEPTH_BIT,
			VK_IMAGE_VIEW_TYPE_2D
		);

		VulkanUtils::transitionImageLayout(
			context,
			depthImage,
			depthFormat,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		);
	}

	void SwapChain::shutdownTransient()
	{
		vkDestroyImageView(context.device, colorImageView, nullptr);
		colorImageView = VK_NULL_HANDLE;

		vkDestroyImage(context.device, colorImage, nullptr);
		colorImage = VK_NULL_HANDLE;

		vkFreeMemory(context.device, colorImageMemory, nullptr);
		colorImageMemory = VK_NULL_HANDLE;

		vkDestroyImageView(context.device, depthImageView, nullptr);
		depthImageView = VK_NULL_HANDLE;

		vkDestroyImage(context.device, depthImage, nullptr);
		depthImage = VK_NULL_HANDLE;

		vkFreeMemory(context.device, depthImageMemory, nullptr);
		depthImageMemory = VK_NULL_HANDLE;

		for (auto imageView : swapChainImageViews)
			vkDestroyImageView(context.device, imageView, nullptr);

		swapChainImageViews.clear();
		swapChainImages.clear();

		vkDestroySwapchainKHR(context.device, swapChain, nullptr);
		swapChain = VK_NULL_HANDLE;
	}

	void SwapChain::initPersistent()
	{
		// Create sync objects
		imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			VkSemaphoreCreateInfo semaphoreInfo = {};
			semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

			if (vkCreateSemaphore(context.device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS)
				throw std::runtime_error("Can't create 'image available' semaphore");

			if (vkCreateSemaphore(context.device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS)
				throw std::runtime_error("Can't create 'render finished' semaphore");

			VkFenceCreateInfo fenceInfo = {};
			fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

			if (vkCreateFence(context.device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS)
				throw std::runtime_error("Can't create in flight frame fence");
		}
	}

	void SwapChain::shutdownPersistent()
	{
		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkDestroySemaphore(context.device, imageAvailableSemaphores[i], nullptr);
			imageAvailableSemaphores[i] = VK_NULL_HANDLE;

			vkDestroySemaphore(context.device, renderFinishedSemaphores[i], nullptr);
			renderFinishedSemaphores[i] = VK_NULL_HANDLE;

			vkDestroyFence(context.device, inFlightFences[i], nullptr);
			inFlightFences[i] = VK_NULL_HANDLE;
		}
	}

	bool SwapChain::acquire(const UniformBufferObject& state)
	{
		vkWaitForFences(context.device, 1, &inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

		VkResult result = vkAcquireNextImageKHR(
			context.device,
			swapChain,
			std::numeric_limits<uint64_t>::max(),
			imageAvailableSemaphores[currentFrame],
			VK_NULL_HANDLE,
			&imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
			return false;

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
			throw std::runtime_error("Can't acquire swap chain image");

		return true;
	}

	bool SwapChain::present(VkCommandBuffer commandBuffer)
	{
		VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		vkResetFences(context.device, 1, &inFlightFences[currentFrame]);
		if (vkQueueSubmit(context.graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS)
			throw std::runtime_error("Can't submit command buffer");

		VkSwapchainKHR swapChains[] = { swapChain };
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr; // Optional

		VulkanUtils::transitionImageLayout(
			context,
			swapChainImages[imageIndex],
			swapChainImageFormat,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
		);

		VkResult result = vkQueuePresentKHR(context.presentQueue, &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
			return false;

		if (result != VK_SUCCESS)
			throw std::runtime_error("Can't present swap chain image");

		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
		return true;
	}

	void SwapChain::initFrames(VkDeviceSize uboSize)
	{

	}

	void SwapChain::shutdownFrames()
	{
	
	}

	// ----------------------- Helper Functions -----------------------------
	SwapChain::SwapChainSupportDetails SwapChain::fetchSwapChainSupportDetails() const
	{
		SwapChain::SwapChainSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context.physicalDevice, context.surface, &details.capabilities);

		uint32_t formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(context.physicalDevice, context.surface, &formatCount, nullptr);

		if (formatCount > 0)
		{
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(context.physicalDevice, context.surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(context.physicalDevice, context.surface, &presentModeCount, nullptr);

		if (presentModeCount > 0)
		{
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(context.physicalDevice, context.surface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}

	SwapChain::SwapChainSettings SwapChain::selectOptimalSwapChainSettings(const SwapChain::SwapChainSupportDetails& details, int width, int height) const
	{
		assert(!details.formats.empty());
		assert(!details.presentModes.empty());

		SwapChain::SwapChainSettings settings;

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
}