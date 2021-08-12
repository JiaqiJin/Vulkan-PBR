#include "Surface.h"
#include "Instance.h"
#include "../Common/Logger.h"

namespace Vulkan
{
	Surface::Surface(const std::shared_ptr<Instance>& instance, GLFWwindow* window)
	{
		m_instance = instance;
		m_window = window;
		if (glfwCreateWindowSurface(m_instance->GetHandle(), window, nullptr, &m_surface) != VK_SUCCESS)
			K_ERROR("Failed creating Surface");
	}

	Surface::~Surface()
	{
		if (m_surface)
			vkDestroySurfaceKHR(m_instance->GetHandle(), m_surface, nullptr);
	}
}
