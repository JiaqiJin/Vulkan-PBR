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

class ImGuiRenderer
{
public:
	ImGuiRenderer(const RHI::VulkanContext* context,
		VkExtent2D extent,
		VkRenderPass renderPass);
	virtual ~ImGuiRenderer();

	void init(const RHI::UniformBufferObject* ubo, const RHI::RenderScene* scene, const RHI::SwapChain* swapChain);
	void resize(const RHI::SwapChain* swapChain);
	void update(RHI::UniformBufferObject* ubo, RHI::RenderScene* scene);
	void render(const RHI::UniformBufferObject* ubo, const RHI::RenderScene* scene, const RHI::VulkanRenderFrame& frame);
	void shutdown();

private:
	const RHI::VulkanContext* context{nullptr};
	VkExtent2D extent;
	VkRenderPass renderPass;
};

