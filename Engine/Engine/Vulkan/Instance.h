#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

namespace Vulkan
{
	constexpr const char* kValidationLayers[] = { "VK_LAYER_KHRONOS_validation" };

	class Instance
	{
	public:
		Instance(bool use_validation_layer = false, PFN_vkDebugUtilsMessengerCallbackEXT debug_callback = nullptr);

		~Instance();

		VkInstance GetHandle() const { return m_instance; }
	private:
		VkInstance m_instance{ VK_NULL_HANDLE };
	};
}