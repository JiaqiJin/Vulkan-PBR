#define VK_USE_PLATFORM_WIN32_KHR
#include "Application.h"
#include "RHI/Renderer.h"
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
	
	renderer->update(&ubo, scene);
	imguiRenderer->update(&ubo, scene);
}


void Application::render()
{
	VulkanRenderFrame frame;
	if (!swapChain->acquire(&ubo, frame))
	{
		recreateVulkanSwapChain();
		return;
	}

	renderer->render(&ubo, scene, frame);
	imguiRenderer->render(&ubo, scene, frame);

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
	renderer->init(&ubo, scene);

	imguiRenderer = new ImGuiRenderer(context, swapChain->getExtent(), swapChain->getNoClearRenderPass());
	imguiRenderer->init(&ubo, scene, swapChain);
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
