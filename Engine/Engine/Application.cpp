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
	m_instance = std::make_shared<Vulkan::Instance>();

	if (!m_instance)
	{
		K_ERROR("Failed to create instance!");
		exit(EXIT_FAILURE);
	}

	std::vector<std::shared_ptr<Vulkan::PhysicalDevice>> physical_devices = Vulkan::PhysicalDevice::Fetch(m_instance);
	if (physical_devices.empty()) 
	{
		K_ERROR("Failed to find physical device with vulkan support!");
		exit(EXIT_FAILURE);
	}

	m_surface = std::make_shared<Vulkan::Surface>(m_instance, m_window);
	if (!m_surface)
	{
		K_ERROR("Failed to create surface!");
		exit(EXIT_FAILURE);
	}

	Vulkan::DeviceCreateInfo device_create_info;
	device_create_info.Initialize(
		physical_devices[0],
		[&](const std::shared_ptr<Vulkan::PhysicalDevice>& physical_device,
			std::vector<Vulkan::QueueSelection>* const out_queue_selections,
			std::vector<Vulkan::PresentQueueSelection>* const out_present_queue_selections) -> bool 
		{
				const auto& families = physical_device->GetQueueFamilyProperties();
				if (families.empty())
					return false;

				Vulkan::PresentQueueSelection present_queue = { &m_present_queue, m_surface, UINT32_MAX };
				Vulkan::QueueSelection main_queue = { &m_main_queue, UINT32_MAX }, loader_queue = { &m_loader_queue, UINT32_MAX };
					
				// main queue and present queue
				for (uint32_t i = 0; i < families.size(); ++i) {
					VkQueueFlags flags = families[i].queueFlags;
					if ((flags & VK_QUEUE_GRAPHICS_BIT) && (flags & VK_QUEUE_TRANSFER_BIT)) {
						main_queue.family = i;
						main_queue.index_specifier = 0;

						if (physical_device->GetSurfaceSupport(i, present_queue.surface)) {
							present_queue.family = i;
							present_queue.index_specifier = 0;
							break;
						}
					}
				}

				// present queue fallback
				if (present_queue.family == UINT32_MAX)
					for (uint32_t i = 0; i < families.size(); ++i)
					{
						if (physical_device->GetSurfaceSupport(i, present_queue.surface))
						{
							present_queue.family = i;
							present_queue.index_specifier = 0;
							break;
						}
					}

				// loader queue
				for (uint32_t i = 0; i < families.size(); ++i) {
					VkQueueFlags flags = families[i].queueFlags;
					if ((flags & VK_QUEUE_GRAPHICS_BIT) && (flags & VK_QUEUE_COMPUTE_BIT) &&
						(flags & VK_QUEUE_TRANSFER_BIT)) {
						loader_queue.family = i;
						loader_queue.index_specifier = 1;

						if (i != main_queue.family)
							break; // prefer independent queue
					}
				}

				(*out_queue_selections) = { main_queue, loader_queue };
				(*out_present_queue_selections) = { present_queue };

				return (~main_queue.family) && (~loader_queue.family) &&
					(~present_queue.family);
		},
		{ VK_KHR_SWAPCHAIN_EXTENSION_NAME });
	if (!device_create_info.QueueSupport()) 
	{
		spdlog::error("Failed to find queues!");
		exit(EXIT_FAILURE);
	}
	if (!device_create_info.ExtensionSupport())
	{
		spdlog::error("Failed to find extension support!");
		exit(EXIT_FAILURE);
	}
	VkPhysicalDeviceDescriptorIndexingFeatures descriptor_indexing_features = {};
	descriptor_indexing_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
	descriptor_indexing_features.descriptorBindingPartiallyBound = VK_TRUE;

	m_device = Vulkan::Device::Create(device_create_info, &descriptor_indexing_features);
	if (!m_device) 
	{
		spdlog::error("Failed to create logical device!");
		exit(EXIT_FAILURE);
	}
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
