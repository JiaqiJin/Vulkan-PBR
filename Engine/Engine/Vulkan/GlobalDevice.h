#pragma once

#include <vulkan/vulkan.h>
#include <memory>

struct GLFWwindow;

namespace Vulkan
{
	class Instance;
	class Surface;
	class Device;
	class Queue;
	class PresentQueue;

	class GlobalDevice
	{
	public:
		GlobalDevice(GLFWwindow* m_window);
		~GlobalDevice();

	private:
		// base
		std::shared_ptr<Instance> m_instance;
		std::shared_ptr<Surface> m_surface;
		std::shared_ptr<Device> m_device;
		std::shared_ptr<Queue> m_main_queue, m_loader_queue;
		std::shared_ptr<PresentQueue> m_present_queue;
	};
}