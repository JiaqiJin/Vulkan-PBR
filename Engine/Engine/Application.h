#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <optional>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Common/Camera.h"

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

struct CameraState
{
	double phi{ 0.0f };
	double theta{ 0.0f };
	double radius{ 2.0f };
	glm::vec3 target;
};

struct InputState
{
	const double rotationSpeed{ 0.01 };
	const double scrollSpeed{ 1.5 };
	bool rotating{ false };
	double lastMouseX{ 0.0 };
	double lastMouseY{ 0.0 };
};

class Application
{
public:
	void run();

	static int GetWindowWidth() { return width; }
	static int GetWindowHeight() { return height; }
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
	static void onMousePosition(GLFWwindow* window, double mouseX, double mouseY);
	static void onMouseButton(GLFWwindow* window, int button, int action, int mods);
	static void onScroll(GLFWwindow* window, double deltaX, double deltaY);
	static void processInput(GLFWwindow* window);
private:
	GLFWwindow* window{ nullptr };
	static int width, height;
	bool windowResized{ false };

	RHI::RenderScene* scene{ nullptr };
	RHI::UniformBufferObject ubo;

	RHI::Renderer* renderer{ nullptr };
	ImGuiRenderer* imguiRenderer{ nullptr };

	RHI::SwapChain* swapChain{ nullptr };
	RHI::VulkanContext* context{ nullptr };

	// Input 
	CameraState camera;
	InputState input;

	static Camera FPSCamera;
	// timing
	static float deltaTime;	// time between current frame and last frame
	static float lastFrame;
	static float lastX;
	static float lastY;
	static bool firstMouse;
};