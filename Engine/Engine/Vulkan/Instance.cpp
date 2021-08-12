#include "Instance.h"
#include "../Common/Logger.h"

#include <GLFW/glfw3.h>

namespace Vulkan
{
	Instance::Instance(bool use_validation_layer, PFN_vkDebugUtilsMessengerCallbackEXT debug_callback) 
	{
		VkApplicationInfo app_info = {};
		app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		app_info.apiVersion = VK_API_VERSION_1_2;

		VkInstanceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		create_info.pApplicationInfo = &app_info;

		uint32_t glfw_extension_count = 0;
		const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
		std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);
		if (use_validation_layer)
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		create_info.enabledExtensionCount = extensions.size();
		create_info.ppEnabledExtensionNames = extensions.data();

		VkValidationFeatureEnableEXT validation_features_enabled[] =
		{
			VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT, VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT
		};

		VkValidationFeaturesEXT validation_features = {};
		validation_features.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
		validation_features.enabledValidationFeatureCount =
			sizeof(validation_features_enabled) / sizeof(VkValidationFeatureEnableEXT);

		validation_features.pEnabledValidationFeatures = validation_features_enabled;

		if (use_validation_layer)
		{
			create_info.enabledLayerCount = sizeof(kValidationLayers) / sizeof(const char*);
			create_info.ppEnabledLayerNames = kValidationLayers;

			validation_features.pNext = create_info.pNext;
			create_info.pNext = &validation_features;
		}
		else
			create_info.enabledLayerCount = 0;

		if (vkCreateInstance(&create_info, nullptr, &m_instance) != VK_SUCCESS)
			K_ERROR("vkCreateInstance Failed");
	}

	Instance::~Instance() 
	{
		if (m_instance)
		{
			vkDestroyInstance(m_instance, nullptr);
		}
	}
}