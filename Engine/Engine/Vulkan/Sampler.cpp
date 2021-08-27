#include "Sampler.h"
#include "Device.h"
#include "PhysicalDevice.h"

#include "../Common/Logger.h"

namespace Vulkan
{
	Sampler::Sampler(const std::shared_ptr<Device>& device, const VkSamplerCreateInfo& create_info)
		: m_device(device)
	{
		if (vkCreateSampler(device->GetHandle(), &create_info, nullptr, &m_sampler) != VK_SUCCESS)
			K_ERROR("Failed to create RenderPass");
	}

	Sampler::Sampler(const std::shared_ptr<Device>& device, VkFilter filter,
		VkSamplerAddressMode address_mode, VkSamplerMipmapMode mipmap_mode,
		uint32_t mipmap_level, bool request_anisotropy, float max_anisotropy) : m_device(device)
	{
		VkSamplerCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		create_info.magFilter = filter;
		create_info.minFilter = filter;
		create_info.mipmapMode = mipmap_mode;
		create_info.addressModeU = address_mode;
		create_info.addressModeV = address_mode;
		create_info.addressModeW = address_mode;
		create_info.mipLodBias = 0.0f;
		create_info.compareOp = VK_COMPARE_OP_NEVER;
		create_info.minLod = 0.0f;
		create_info.maxLod = (float)mipmap_level;

		if (m_device->GetPhysicalDevicePtr()->GetFeatures().samplerAnisotropy)
		{
			create_info.anisotropyEnable = request_anisotropy ? VK_TRUE : VK_FALSE;
			create_info.maxAnisotropy = max_anisotropy;
		}
		else
		{
			create_info.anisotropyEnable = VK_FALSE;
			create_info.maxAnisotropy = 1.0;
		}

		create_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		if (vkCreateSampler(device->GetHandle(), &create_info, nullptr, &m_sampler) != VK_SUCCESS)
			K_ERROR("Failed to create Sampler");
	}

	Sampler::~Sampler()
	{
		if (m_sampler)
			vkDestroySampler(m_device->GetHandle(), m_sampler, nullptr);
	}
}