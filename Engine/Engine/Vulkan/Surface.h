#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <memory>

namespace Vulkan
{
	class Instance;

	class Surface
	{
	public:
		Surface(const std::shared_ptr<Instance>& instance, GLFWwindow* window);
		~Surface();

		VkSurfaceKHR GetHandle() const { return m_surface; }

		GLFWwindow* GetGlfwWindow() const { return m_window; }

	private:
		std::shared_ptr<Instance> m_instance;
		GLFWwindow* m_window{ nullptr };

		VkSurfaceKHR m_surface{ VK_NULL_HANDLE };

	};
}