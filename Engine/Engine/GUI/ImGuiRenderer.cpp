#include "ImGuiRenderer.h"
#include "../Application.h"
#include "../RHI/SwapChain.h"
#include "../RHI/VulkanContext.h"
#include "../RHI/VulkanUtils.h"
#include "../Common/RenderScene.h"

#include "../Vendor/imgui/imgui.h"
#include "../Vendor/imgui/imgui_impl_vulkan.h"

#include <array>

ImGuiRenderer::ImGuiRenderer(const RHI::VulkanContext* context, VkExtent2D extent, VkRenderPass renderPass)
	: context(context)
	, extent(extent)
	, renderPass(renderPass)
{

}

ImGuiRenderer::~ImGuiRenderer()
{
	shutdown();
}

void ImGuiRenderer::init(const RHI::UniformBufferObject* ubo, const RHI::RenderScene* scene, const RHI::SwapChain* swapChain)
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

void ImGuiRenderer::update(RHI::UniformBufferObject* ubo, RHI::RenderScene* scene)
{
	static float f = 0.0f;
	static int counter = 0;
	static bool show_demo_window = false;
	static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	ImGui::Begin("Material Parameters");

	int oldCurrentEnvironment = ubo->currentEnvironment;
	if (ImGui::BeginCombo("Choose Your Destiny", scene->getHDRTexturePath(ubo->currentEnvironment)))
	{
		for (int i = 0; i < scene->getNumHDRTextures(); i++)
		{
			bool selected = (i == ubo->currentEnvironment);
			if (ImGui::Selectable(scene->getHDRTexturePath(i), &selected))
				ubo->currentEnvironment = i;
			if (selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	ImGui::Checkbox("Demo Window", &show_demo_window);

	ImGui::SliderFloat("Lerp User Material", &ubo->lerpUserValues, 0.0f, 1.0f);
	ImGui::SliderFloat("Metalness", &ubo->userMetalness, 0.0f, 1.0f);
	ImGui::SliderFloat("Roughness", &ubo->userRoughness, 0.0f, 1.0f);

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
}

void ImGuiRenderer::render(const RHI::UniformBufferObject* ubo, const RHI::RenderScene* scene, const RHI::VulkanRenderFrame& frame)
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