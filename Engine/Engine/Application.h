#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <optional>

#include <glm/glm.hpp>

#include "RHI/RendererContext.h"

// Forward declaration
struct GLFWwindow;
class ImGuiRenderer;

namespace RHI
{
	class Renderer;
	class RenderScene;
	class SwapChain;
	class VulkanContext;

	struct UniformBufferObject
	{
		glm::mat4 world;
		glm::mat4 view;
		glm::mat4 proj;
		glm::vec3 cameraPosWS;
		float lerpUserValues{ 0.0f };
		float userMetalness{ 0.0f };
		float userRoughness{ 0.0f };
		int currentEnvironment{ 0 };
	};
}

class Application
{
public:
	void run();

private:
	void initWindow();
	void shutdownWindow();

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
	bool windowResized{ false };

	RHI::RenderScene* scene{ nullptr };
	RHI::UniformBufferObject ubo;

	RHI::Renderer* renderer{ nullptr };
	ImGuiRenderer* imguiRenderer{ nullptr };

	RHI::SwapChain* swapChain{ nullptr };
	RHI::VulkanContext* context{ nullptr };
};