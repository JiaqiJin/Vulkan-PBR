#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <array>
#include <memory>
#include <vector>

#include "Vulkan/GlobalDevice.h"

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

	std::shared_ptr<Vulkan::GlobalDevice> m_global_device;
};