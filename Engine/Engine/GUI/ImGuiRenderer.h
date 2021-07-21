#pragma once

#include <vulkan/vulkan.h>

namespace RHI
{
	class RenderScene;
	class SwapChain;
	struct UniformBufferObject;
	struct VulkanRenderFrame;
	class VulkanContext;
}

struct ImGuiContext;

class ImGuiRenderer
{
public:
	ImGuiRenderer(const RHI::VulkanContext* context,
		ImGuiContext* imguiContext,
		VkExtent2D extent,
		VkRenderPass renderPass);
	virtual ~ImGuiRenderer();

	void init( const RHI::SwapChain* swapChain);
	void resize(const RHI::SwapChain* swapChain);
	void render(const RHI::VulkanRenderFrame& frame);
	void shutdown();

private:
	const RHI::VulkanContext* context{nullptr};
	ImGuiContext* imguiContext{ nullptr };
	VkExtent2D extent;
	VkRenderPass renderPass;
};

