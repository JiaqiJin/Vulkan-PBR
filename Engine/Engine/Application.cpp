#define VK_USE_PLATFORM_WIN32_KHR
#include "Application.h"
#include "Renderer/Renderer.h"
#include "RHI/VulkanUtils.h"
#include "RHI/SwapChain.h"
#include "RHI/VulkanContext.h"

#include "Common/RenderScene.h"

#include "Vendor/imgui/imgui.h"
#include "Vendor/imgui/imgui_impl_glfw.h"

#include "GUI/ImGuiRenderer.h"

#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <iostream>
#include <chrono>
#include <algorithm>

using namespace RHI;

void Application::run()
{
	initWindow();
	initImGui();
	initVulkan();
	initVulkanSwapChain();
	initRenderScene();
	initRenderers();
	mainloop();
	shutdownRenderers();
	shutdownRenderScene();
	shutdownVulkanSwapChain();
	shutdownVulkan();
	shutdownImGui();
	shutdownWindow();
}

void Application::update()
{
	static auto startTime = std::chrono::high_resolution_clock::now();
	auto currentTime = std::chrono::high_resolution_clock::now();

	const float rotationSpeed = 0.3f;
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	const glm::vec3& up = { 0.0f, 0.0f, 1.0f };
	const glm::vec3& zero = { 0.0f, 0.0f, 0.0f };

	VkExtent2D extent = swapChain->getExtent();

	const float aspect = extent.width / (float)extent.height;
	const float zNear = 0.1f;
	const float zFar = 1000.0f;

	glm::vec3 cameraPos;
	cameraPos.x = static_cast<float>(glm::cos(camera.phi) * glm::cos(camera.theta) * camera.radius);
	cameraPos.y = static_cast<float>(glm::sin(camera.phi) * glm::cos(camera.theta) * camera.radius);
	cameraPos.z = static_cast<float>(glm::sin(camera.theta) * camera.radius);

	ubo.world = glm::mat4(1.0f);
	ubo.view = glm::lookAt(cameraPos, zero, up);
	ubo.proj = glm::perspective(glm::radians(60.0f), aspect, zNear, zFar);
	ubo.proj[1][1] *= -1;
	ubo.cameraPosWS = cameraPos;

	static float f = 0.0f;
	static int counter = 0;
	static bool show_demo_window = false;

	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	ImGui::Begin("Material Parameters");

	if (ImGui::Button("Reload Shaders"))
	{
		scene->reloadShaders();
		renderer->reload(scene);
		renderer->setEnvironment(scene->getHDRTexture(ubo.currentEnvironment));
	}

	int oldCurrentEnvironment = ubo.currentEnvironment;
	if (ImGui::BeginCombo("Choose Your Destiny", scene->getHDRTexturePath(ubo.currentEnvironment)))
	{
		for (int i = 0; i < scene->getNumHDRTextures(); i++)
		{
			bool selected = (i == ubo.currentEnvironment);
			if (ImGui::Selectable(scene->getHDRTexturePath(i), &selected))
			{
				ubo.currentEnvironment = i;
				renderer->setEnvironment(scene->getHDRTexture(ubo.currentEnvironment));
			}
			if (selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	ImGui::Checkbox("Demo Window", &show_demo_window);

	ImGui::SliderFloat("Lerp User Material", &ubo.lerpUserValues, 0.0f, 1.0f);
	ImGui::SliderFloat("Metalness", &ubo.userMetalness, 0.0f, 1.0f);
	ImGui::SliderFloat("Roughness", &ubo.userRoughness, 0.0f, 1.0f);

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
}


void Application::render()
{
	VulkanRenderFrame frame;
	if (!swapChain->acquire(&ubo, frame))
	{
		recreateVulkanSwapChain();
		return;
	}

	renderer->render(scene, frame);
	imguiRenderer->render(scene, frame);

	if (!swapChain->present(frame) || windowResized)
	{
		windowResized = false;
		recreateVulkanSwapChain();
	}
}

void Application::mainloop()
{
	if (!window)
		return;

	while (!glfwWindowShouldClose(window))
	{
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		update();

		ImGui::Render();

		render();
		glfwPollEvents();
	}
	context->wait();
}

void Application::initWindow()
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window = glfwCreateWindow(1024, 768, "Vulkan", nullptr, nullptr);

	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, &Application::onFramebufferResize);
	glfwSetCursorPosCallback(window, &Application::onMousePosition);
	glfwSetMouseButtonCallback(window, &Application::onMouseButton);
	glfwSetScrollCallback(window, &Application::onScroll);
}

void Application::shutdownWindow()
{
	glfwDestroyWindow(window);
	window = nullptr;
}

void Application::onFramebufferResize(GLFWwindow* window, int width, int height)
{
	Application* app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
	assert(app != nullptr);
	app->windowResized = true;
}


void Application::initVulkan()
{
	if (!context)
		context = new VulkanContext();

	context->init(window, "Vulkan Kawaii", "Excalibu Engine");
}

void Application::shutdownVulkan()
{
	if (context)
		context->shutdown();

	delete context;
	context = nullptr;
}


void Application::initVulkanSwapChain()
{
	VkDeviceSize ubosize = sizeof(UniformBufferObject);
	if (!swapChain)
		swapChain = new SwapChain(context, ubosize);

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	swapChain->init(width, height);
}

void Application::shutdownVulkanSwapChain()
{
	delete swapChain;
	swapChain = nullptr;
}

void Application::recreateVulkanSwapChain()
{
	int width = 0, height = 0;
	while (width == 0 || height == 0)
	{
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
	}
	context->wait();

	glfwGetWindowSize(window, &width, &height);
	swapChain->reinit(width, height);
	renderer->resize(swapChain);
	imguiRenderer->resize(swapChain);
}

void Application::initRenderScene()
{
	scene = new RenderScene(context);
	scene->init();
}

void Application::shutdownRenderScene()
{
	scene->shutdown();

	delete scene;
	scene = nullptr;
}

void Application::initRenderers()
{
	renderer = new Renderer(context, swapChain->getExtent(), swapChain->getDescriptorSetLayout(), swapChain->getRenderPass());
	renderer->init(scene);
	renderer->setEnvironment(scene->getHDRTexture(ubo.currentEnvironment));

	imguiRenderer = new ImGuiRenderer(context, swapChain->getExtent(), swapChain->getNoClearRenderPass());
	imguiRenderer->init(scene, swapChain);
}

void Application::shutdownRenderers()
{
	delete renderer;
	renderer = nullptr;

	delete imguiRenderer;
	imguiRenderer = nullptr;
}

void Application::initImGui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui::StyleColorsDark();

	// TODO: use own GLFW callbacks
	ImGui_ImplGlfw_InitForVulkan(window, true);
}

void Application::shutdownImGui()
{
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void Application::onMousePosition(GLFWwindow* window, double mouseX, double mouseY)
{
	Application* application = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
	assert(application != nullptr);

	if (application->input.rotating)
	{
		double deltaX = mouseX - application->input.lastMouseX;
		double deltaY = mouseY - application->input.lastMouseY;

		application->camera.phi -= deltaX * application->input.rotationSpeed;
		application->camera.theta += deltaY * application->input.rotationSpeed;

		application->camera.phi = std::fmod(application->camera.phi, glm::two_pi<double>());
		application->camera.theta = std::clamp<double>(application->camera.theta, -glm::half_pi<double>(), glm::half_pi<double>());
	}

	application->input.lastMouseX = mouseX;
	application->input.lastMouseY = mouseY;
}

void Application::onMouseButton(GLFWwindow* window, int button, int action, int mods)
{
	Application* application = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
	assert(application != nullptr);

	if (button == GLFW_MOUSE_BUTTON_RIGHT)
		application->input.rotating = (action == GLFW_PRESS);
}

void Application::onScroll(GLFWwindow* window, double deltaX, double deltaY)
{
	Application* application = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
	assert(application);

	application->camera.radius -= deltaY * application->input.scrollSpeed;
}
