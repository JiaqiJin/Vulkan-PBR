#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <array>
#include <memory>
#include <vector>

#include "Vulkan/Device.h"
#include "Vulkan/Instance.h"
#include "Vulkan/Surface.h"
#include "Vulkan/Queue.h"

class Application
{
public:
	Application();
	~Application();

	void Run();

private:
	void create_window();
	void initialize_vulkan();
	void resize();
	void draw_frame();

	static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void glfw_framebuffer_resize_callback(GLFWwindow* window, int width, int height);
private:
	GLFWwindow* m_window{ nullptr };

	// base
	std::shared_ptr<Vulkan::Instance> m_instance;
	std::shared_ptr<Vulkan::Surface> m_surface;
	std::shared_ptr<Vulkan::Device> m_device;
	std::shared_ptr<Vulkan::Queue> m_main_queue, m_loader_queue;
	std::shared_ptr<Vulkan::PresentQueue> m_present_queue;
};