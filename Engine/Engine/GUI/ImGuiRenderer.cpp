#include "ImGuiRenderer.h"
#include "../Application.h"
#include "../RHI/SwapChain.h"
#include "../RHI/VulkanContext.h"
#include "../RHI/VulkanUtils.h"
#include "../Common/RenderScene.h"

#include "../Vendor/imgui/imgui.h"
#include "../Vendor/imgui/imgui_impl_vulkan.h"

#include <array>

ImGuiRenderer::ImGuiRenderer(const RHI::VulkanContext* context, ImGuiContext* imguiContext, VkExtent2D extent, VkRenderPass renderPass)
	: context(context)
	, imguiContext(imguiContext)
	, extent(extent)
	, renderPass(renderPass)
{

}

ImGuiRenderer::~ImGuiRenderer()
{
	shutdown();
}

void ImGuiRenderer::init(const RHI::SwapChain* swapChain)
{
	// Init ImGui bindings for Vulkan
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = context->getInstance();
	init_info.PhysicalDevice = context->getPhysicalDevice();
	init_info.Device = context->getDevice();
	init_info.QueueFamily = context->getGraphicsQueueFamily();
	init_info.Queue = context->getGraphicsQueue();
	init_info.DescriptorPool = context->getDescriptorPool();
	init_info.MSAASamples = context->getMaxMSAASamples();
	init_info.MinImageCount = static_cast<uint32_t>(swapChain->getNumImages());
	init_info.ImageCount = static_cast<uint32_t>(swapChain->getNumImages());

	ImGui_ImplVulkan_Init(&init_info, renderPass);

	VkCommandBuffer imGuiCommandBuffer = RHI::VulkanUtils::beginSingleTimeCommands(context);
	ImGui_ImplVulkan_CreateFontsTexture(imGuiCommandBuffer);
	RHI::VulkanUtils::endSingleTimeCommands(context, imGuiCommandBuffer);
}

void ImGuiRenderer::resize(const RHI::SwapChain* swapChain)
{
	extent = swapChain->getExtent();
	ImGui_ImplVulkan_SetMinImageCount(swapChain->getNumImages());
}

void ImGuiRenderer::render(const RHI::VulkanRenderFrame& frame)
{
	VkCommandBuffer commandBuffer = frame.commandBuffer;
	VkFramebuffer frameBuffer = frame.frameBuffer;
	VkDeviceMemory uniformBufferMemory = frame.uniformBufferMemory;
	VkDescriptorSet descriptorSet = frame.descriptorSet;

	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = frameBuffer;
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = extent;

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), frame.commandBuffer);

	vkCmdEndRenderPass(commandBuffer);
}

void ImGuiRenderer::shutdown()
{
	ImGui_ImplVulkan_Shutdown();
}