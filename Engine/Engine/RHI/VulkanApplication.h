#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <optional>

#include "VulkanRendererContext.h"

struct GLFWwindow;
class Renderer;
class RenderScene;

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily{ std::nullopt };
	std::optional<uint32_t> presentFamily{ std::nullopt };

	inline bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

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
	

private:
	
};