#include "Application.h"
#include "Vulkan/PhysicalDevice.h"
#include "Vulkan/Device.h"
#include "Vulkan/QueueSelector.h"
#include "Vulkan/DeviceCreateInfo.h"

#include "Common/Config.h"
#include "Common/Logger.h"

Application::Application()
{
	create_window();
	initialize_vulkan();
}

Application::~Application()
{
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void Application::Run()
{
	double lst_time = glfwGetTime();
	while (!glfwWindowShouldClose(m_window)) {
		double cur_time = glfwGetTime();

		glfwPollEvents();

		
		draw_frame();
		lst_time = cur_time;
	}

}

void Application::create_window()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	m_window = glfwCreateWindow(kDefaultWidth, kDefaultHeight, kAppName, nullptr, nullptr);
	glfwSetWindowUserPointer(m_window, this);
	glfwSetKeyCallback(m_window, glfw_key_callback);
	glfwSetFramebufferSizeCallback(m_window, glfw_framebuffer_resize_callback);
}

void Application::initialize_vulkan()
{
	m_global_device = std::make_shared<Vulkan::GlobalDevice>(m_window);
}

void Application::resize()
{

}

void Application::draw_frame()
{
}

void Application::glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
}

void Application::glfw_framebuffer_resize_callback(GLFWwindow* window, int width, int height)
{
}
