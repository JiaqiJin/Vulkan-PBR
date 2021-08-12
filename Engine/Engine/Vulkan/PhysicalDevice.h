#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

namespace Vulkan
{
	class Instance;
	class Surface;

	class PhysicalDevice
	{
	public:
		static std::vector<std::shared_ptr<PhysicalDevice>> Fetch(const std::shared_ptr<Instance>& instance);

		const std::shared_ptr<Instance>& GetInstancePtr() const { return m_instance; }

		VkPhysicalDevice GetHandle() const { return m_physical_device; }

		const VkPhysicalDeviceProperties& GetProperties() const { return m_properties; }

		const VkPhysicalDeviceMemoryProperties& GetMemoryProperties() const { return m_memory_properties; }

		const VkPhysicalDeviceFeatures& GetFeatures() const { return m_features; }

		const std::vector<VkQueueFamilyProperties>& GetQueueFamilyProperties() const { return m_queue_family_properties; }

		bool GetSurfaceSupport(uint32_t queue_family_index, const std::shared_ptr<Surface>& surface);

	private:
		void initialize(const std::shared_ptr<Instance>& instance, VkPhysicalDevice physical_device);

	private:
		std::shared_ptr<Instance> m_instance;

		VkPhysicalDevice m_physical_device{ VK_NULL_HANDLE };

		VkPhysicalDeviceProperties m_properties;
		VkPhysicalDeviceMemoryProperties m_memory_properties;
		VkPhysicalDeviceFeatures m_features;
		std::vector<VkQueueFamilyProperties> m_queue_family_properties;
	};
}